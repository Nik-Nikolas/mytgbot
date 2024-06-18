#ifndef BOT_VERBOSE_H
#define BOT_VERBOSE_H

#include "Command.h"


// class IBot: public Bot{
//     public:
//         virtual ~IBot() = default;
//         getApi()

// }




class Bot_verbose: public Bot{

    public:

        Bot_verbose(const string& token, const string& name, const string& llm_file): Bot(token), m_name(name), m_llm_file(llm_file){
        }

        void sayWord(const string& word) const {  
            if(m_id)
                getApi().sendMessage(m_id, word);
        }

        void saveID(std::int64_t id){
            if(!m_id)
                getApi().sendMessage(id, m_name + " инициирует ID: " + to_string(id));

            m_id = id;            
        }

        void setCanaryDelay(std::int64_t d){
            m_canary_delay_seconds = d;            
        }

        std::int64_t canaryDelay() const {
            return m_canary_delay_seconds;            
        }

        string getName() const {
            return m_name;            
        }
        
        void saveName(std::string n){
            m_name = n;
        }

        void set_silent(bool flag){
            m_is_silent = flag;
        }

        bool is_silent() const {
            return m_is_silent;
        }

        LLM_manager& get_LLM_manager() {
            return m_llm;
        }

        void describe_LLM(int64_t chat_id){

            getApi().sendMessage(chat_id, "Текущая LLM модель: (" + 
            getName() + ") " +
            get_LLM_manager().get_launcher_descr() + "\n\n" +
            "запрос, суффикс: " + get_LLM_manager().get_current_llm_request_suffix() + "\n\n" +
            "запрос, задержка, сек.: " + to_string(get_LLM_manager().get_launcher_delay().count()) + "\n\n" +            
            "ответ, макс. байт: " + to_string(get_LLM_manager().get_output_max_bytes()) + "\n\n" +
            "ответ, макс, сек.: " + to_string(get_LLM_manager().get_response_duration()));
        }

        string start_llm(int64_t chat_id, string request){

            getApi().sendChatAction(chat_id, "typing");
            
            const auto token_detailed{"детально "s};
            
            if(StringTools::startsWith(request, token_detailed)){
                get_LLM_manager().set_response_duration(60*6);
                cout << "Set response duration: " << get_LLM_manager().get_response_duration() << endl;
                request.replace(request.find(token_detailed),token_detailed.length(), ""); 
            }
            else{
                get_LLM_manager().set_default_response_duration();                    
            }
            
            cout << getName() << " got a request: " << request << endl;

            subprocess::popen cmd(get_LLM_manager().get_launcher(), {});
            request.append(get_LLM_manager().get_current_llm_request_suffix());

            cout << getName() << " processing request with: " << get_LLM_manager().get_launcher() <<  endl;
                        
            this_thread::sleep_for(get_LLM_manager().get_launcher_delay());//allow LLM start in subprocess
            cmd.in() << request.c_str() << std::endl;

            size_t cycles{0};

            string response{};
            while(true){
                getApi().sendChatAction(chat_id, "typing");

                ifstream file(get_LLM_manager().get_output_file());
                std::ostringstream ss;

                ss << file.rdbuf();
                auto output = ss.str();

                auto it = output.find(get_LLM_manager().get_end_response_token());
                if(it != string::npos){
                    cout << "Stopped. Found end of LLM output." << endl;
                    cmd.close();
                    response = output;
                    break;                
                }else if(output.size() > get_LLM_manager().get_output_max_bytes()){
                    cout << "Stopped. Detected EXCESSIVE response size, bytes > " << get_LLM_manager().get_output_max_bytes() << endl;
                    cmd.close();
                    response = output;
                    break;
                }else if(cycles > get_LLM_manager().get_response_duration()){
                    cout << "Stopped. Detected EXCESSIVE processing time, seconds > " << get_LLM_manager().get_response_duration() << endl;
                    cmd.close();
                    response = output;
                    break;
                }
                else{
                    ++cycles;
                    this_thread::sleep_for(1s);
                    cout << "Output= " << output << endl;
                    cout << "Wait for LLM complete response ... " << cycles << "/" << get_LLM_manager().get_response_duration() << endl;
                }
            }

            auto it = response.find(get_LLM_manager().get_start_response_token());
            if(it != string::npos)
                response.replace(response.find(get_LLM_manager().get_start_response_token()),get_LLM_manager().get_start_response_token().length(),"");    
            it = response.find(get_LLM_manager().get_end_response_token());
            if(it != string::npos)           
                response.replace(response.find(get_LLM_manager().get_end_response_token()),get_LLM_manager().get_end_response_token().length(),"");    

            cout << getName() << " processed the request. Response is: " << response << endl;

            if(response.empty()){

                response.append(getName() + " не нашел ответ.");
            }

            cout << getName() << " sending the answer ...: " << response << endl;
            getApi().sendMessage(chat_id, response); 

            return response;
        }

        void register_command(const Command& c){
            m_commands.push_back(c);
        }

        const vector<Command>& get_commands(){
            return m_commands;
        }

        void process_command(int64_t chat_id, const string& req){

            for(const auto& el: m_commands){
                if(StringTools::startsWith(req, el.m_command)){
                    el.m_action(chat_id, req);
                }
            }
        }

    private:

        bool            m_is_silent{false};
        std::string     m_name;
        std::int64_t    m_id{0};
        std::int64_t    m_canary_delay_seconds{3600 * 12};
        std::string     m_llm_file{};
        vector<Command> m_commands;
        LLM_manager     m_llm{m_llm_file};
};

#endif
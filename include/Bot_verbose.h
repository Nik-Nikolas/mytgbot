#ifndef BOT_VERBOSE_H
#define BOT_VERBOSE_H

#include "Command.h"
#include <memory>

class BotVerbose{

    public:

        BotVerbose(std::shared_ptr<Bot> bot, const string& name, const string& llm_file): 
        m_bot(bot), 
        m_poll(*m_bot),
        m_name(name), 
        m_llm_file(llm_file){
            m_bot->getApi().deleteWebhook();
        }

        void sayWord(const string& word) const {  
            if(m_id)
                m_bot->getApi().sendMessage(m_id, word);
        }

        void saveID(std::int64_t id){
            if(!m_id)
                m_bot->getApi().sendMessage(id, m_name + " инициирует ID: " + std::to_string(id));

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

        void setSilent(bool flag){
            m_isSilent = flag;
        }

        bool isSilent() const {
            return m_isSilent;
        }

        LLM_manager& getLLMmanager() {
            return m_llm;
        }

        void describeLLM(int64_t chat_id){

            m_bot->getApi().sendMessage(chat_id, "Текущая LLM модель: (" + 
            getName() + ") " +
            getLLMmanager().geLauncherDescr() + "\n\n" +
            "запрос, суффикс: " + getLLMmanager().getCurrentLLMRequestSuffix() + "\n\n" +
            "запрос, задержка, сек.: " + to_string(getLLMmanager().geLauncherDelay().count()) + "\n\n" +            
            "ответ, макс. байт: " + to_string(getLLMmanager().getOutputMaxBytes()) + "\n\n" +
            "ответ, макс, сек.: " + to_string(getLLMmanager().getResponseDuration()));
        }

        string startLLM(int64_t chat_id, string request){

            m_bot->getApi().sendChatAction(chat_id, "typing");
            
            const auto token_detailed{"детально "s};
            
            if(StringTools::startsWith(request, token_detailed)){
                getLLMmanager().setResponseDuration(60*6);
                cout << "Set response duration: " << getLLMmanager().getResponseDuration() << endl;
                request.replace(request.find(token_detailed),token_detailed.length(), ""); 
            }
            else{
                getLLMmanager().setDefaultResponseDuration();                    
            }
            
            cout << getName() << " got a request: " << request << endl;

            subprocess::popen cmd(getLLMmanager().geLauncher(), {});
            request.append(getLLMmanager().getCurrentLLMRequestSuffix());

            cout << getName() << " processing request with: " << getLLMmanager().geLauncher() <<  endl;
                        
            this_thread::sleep_for(getLLMmanager().geLauncherDelay());//allow LLM start in subprocess
            cmd.in() << request.c_str() << std::endl;

            size_t cycles{0};

            string response{};
            while(true){
                m_bot->getApi().sendChatAction(chat_id, "typing");

                ifstream file(getLLMmanager().getOutputFile());
                std::ostringstream ss;

                ss << file.rdbuf();
                auto output = ss.str();

                auto it = output.find(getLLMmanager().getEndResponseToken());
                if(it != string::npos){
                    cout << "Stopped. Found end of LLM output." << endl;
                    cmd.close();
                    response = output;
                    break;                
                }else if(output.size() > getLLMmanager().getOutputMaxBytes()){
                    cout << "Stopped. Detected EXCESSIVE response size, bytes > " << getLLMmanager().getOutputMaxBytes() << endl;
                    cmd.close();
                    response = output;
                    break;
                }else if(cycles > getLLMmanager().getResponseDuration()){
                    cout << "Stopped. Detected EXCESSIVE processing time, seconds > " << getLLMmanager().getResponseDuration() << endl;
                    cmd.close();
                    response = output;
                    break;
                }
                else{
                    ++cycles;
                    this_thread::sleep_for(1s);
                    cout << "Output= " << output << endl;
                    cout << "Wait for LLM complete response ... " << cycles << "/" << getLLMmanager().getResponseDuration() << endl;
                }
            }

            auto it = response.find(getLLMmanager().getStartResponseToken());
            if(it != string::npos)
                response.replace(response.find(getLLMmanager().getStartResponseToken()),getLLMmanager().getStartResponseToken().length(),"");    
            it = response.find(getLLMmanager().getEndResponseToken());
            if(it != string::npos)           
                response.replace(response.find(getLLMmanager().getEndResponseToken()),getLLMmanager().getEndResponseToken().length(),"");    

            cout << getName() << " processed the request. Response is: " << response << endl;

            if(response.empty()){

                response.append(getName() + " не нашел ответ.");
            }

            cout << getName() << " sending the answer ...: " << response << endl;
            m_bot->getApi().sendMessage(chat_id, response); 

            return response;
        }

        void registerCommand(const Command& c){
            m_commands.push_back(c);
        }

        const vector<Command>& get_commands(){
            return m_commands;
        }

        void processCommand(int64_t chat_id, const string& req){

            for(const auto& el: m_commands){
                if(StringTools::startsWith(req, el.m_command)){
                    el.m_action(chat_id, req);
                }
            }
        }

        auto& getApi(){
            return m_bot->getApi();
        }

        auto& getEvents(){
            return m_bot->getEvents();
        }

        auto& getNativeBot(){
            return m_bot;
        }

        void startPoll(){
            m_poll.start();
        }

    private:

        std::shared_ptr<Bot>    m_bot;
        TgLongPoll              m_poll;
        bool                    m_isSilent{false};
        std::string             m_name;
        std::int64_t            m_id{0};
        std::int64_t            m_canary_delay_seconds{3600 * 12};
        std::string             m_llm_file{};
        vector<Command>         m_commands;
        LLM_manager             m_llm{m_llm_file};
};

#endif
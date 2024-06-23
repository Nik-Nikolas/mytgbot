#ifndef BOT_VERBOSE_H
#define BOT_VERBOSE_H

#include "Command.h"
#include <memory>
#include "Bot_manager.h"

const string NotFoundResponse{" не нашел ответ."};

template <typename BotManagerType, typename subproc>
class BotVerbose{

    public:

        BotVerbose(
        std::shared_ptr<Bot> bot, 
        const string& name, 
        const string& llm_file): 
        m_botManager(bot),
        m_name(name), 
        m_llm_file(llm_file){
        }

        void init() {  
            m_botManager.deleteWebhook();

            std::vector<std::string> dummy;
            m_subprocess = std::make_unique<subproc>(getLLMmanager().geLauncher(), dummy);
        }

        void sayWord(const string& word) const {  
            if(m_id)
                m_botManager.sendMessage(m_id, word);
        }

        void saveID(std::int64_t id){
            if(!m_id)
                m_botManager.sendMessage(id, m_name + " инициирует ID: " + std::to_string(id));

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

            m_botManager.sendMessage(chat_id, "Текущая LLM модель: (" + 
            getName() + ") " +
            getLLMmanager().geLauncherDescr() + "\n\n" +
            "запрос, суффикс: " + getLLMmanager().getCurrentLLMRequestSuffix() + "\n\n" +
            "запрос, задержка, сек.: " + to_string(getLLMmanager().geLauncherDelay().count()) + "\n\n" +            
            "ответ, макс. байт: " + to_string(getLLMmanager().getOutputMaxBytes()) + "\n\n" +
            "ответ, макс, сек.: " + to_string(getLLMmanager().getResponseDuration()));
        }

        string startLLM(int64_t chat_id, string request){

            m_botManager.sendChatAction(chat_id, "typing");
            
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
            std::vector<std::string> dummy;
            m_subprocess.reset(new subproc(getLLMmanager().geLauncher(), dummy));
            request.append(getLLMmanager().getCurrentLLMRequestSuffix());

            cout << getName() << " processing request with: " << getLLMmanager().geLauncher() <<  endl;
                        
            this_thread::sleep_for(getLLMmanager().geLauncherDelay());//allow LLM start in subprocess
            m_subprocess->in() << request.c_str() << std::endl;

            size_t cycles{0};

            string response{};
            while(true){
                m_botManager.sendChatAction(chat_id, "typing");

                ifstream file(getLLMmanager().getOutputFile());
                std::ostringstream ss;

                ss << file.rdbuf();
                auto output = ss.str();

                auto it = output.find(getLLMmanager().getEndResponseToken());
                if(it != string::npos){
                    cout << "Stopped. Found end of LLM output." << endl;
                    m_subprocess->close();
                    response = output;
                    break;                
                }else if(output.size() > getLLMmanager().getOutputMaxBytes()){
                    cout << "Stopped. Detected EXCESSIVE response size, bytes > " << getLLMmanager().getOutputMaxBytes() << endl;
                    m_subprocess->close();
                    response = output;
                    break;
                }else if(cycles > getLLMmanager().getResponseDuration()){
                    cout << "Stopped. Detected EXCESSIVE processing time, seconds > " << getLLMmanager().getResponseDuration() << endl;
                    m_subprocess->close();
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

            m_subprocess.reset();

            auto it = response.find(getLLMmanager().getStartResponseToken());
            if(it != string::npos)
                response.replace(response.find(getLLMmanager().getStartResponseToken()),getLLMmanager().getStartResponseToken().length(),"");    
            it = response.find(getLLMmanager().getEndResponseToken());
            if(it != string::npos)           
                response.replace(response.find(getLLMmanager().getEndResponseToken()),getLLMmanager().getEndResponseToken().length(),"");    

            cout << getName() << " processed the request. Response is: " << response << endl;

            if(response.empty()){

                response.append(getName() + NotFoundResponse);
            }

            cout << getName() << " sending the answer ...: " << response << endl;
            m_botManager.sendMessage(chat_id, response); 

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
            return m_botManager.getApi();
        }

        auto& getEvents(){
            return m_botManager.getEvents();
        }   

        void startPoll(){
            m_botManager.startPoll();
        }

        BotManagerType& getManager(){
            return m_botManager;
        }

        std::unique_ptr<subproc>& getSubprocess(){
            return m_subprocess;
        }

    private:
        std::unique_ptr<subproc>            m_subprocess;
        BotManagerType                      m_botManager;
        bool                                m_isSilent{false};
        std::string                         m_name;
        std::int64_t                        m_id{0};
        std::int64_t                        m_canary_delay_seconds{3600 * 12};
        std::string                         m_llm_file{};
        vector<Command>                     m_commands;
        LLM_manager                         m_llm{m_llm_file};
};

#endif
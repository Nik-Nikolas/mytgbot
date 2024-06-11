#ifndef LLM_MANAGER_H
#define LLM_MANAGER_H

class LLM_manager{
    public:

        LLM_manager(const string& f): m_output_file{f}{
            set_launcher(m_default_llm);
        }

        void set_launcher(size_t index){
            cout << __PRETTY_FUNCTION__ << " : llm_launcher=" << index << endl;

            clear_output_file();

            switch(index){
                case 0:  //fall through
                case 1:  //fall through
                case 2:  //fall through
                default: //fall through
                case 3:
                m_launcher = m_commands_register[index].second.second;
                m_descr = m_commands_register[index].second.first;
                m_launcher_delay = std::chrono::seconds(5);
                m_request_suffix = ". Напиши ответ по Русски";
                m_response_duration = m_response_default_duration = 30;
                break;

                case 4:
                m_launcher = m_commands_register[index].second.second;
                m_descr = m_commands_register[index].second.first;
                m_launcher_delay = std::chrono::seconds(15);
                m_request_suffix = "";
                m_response_duration = m_response_default_duration = 60;
                break;
            }
        }

        void clear_output_file(){
            fstream ofs;
            ofs.open(m_output_file, ios::out | ios::trunc);
            ofs << " ";
        }

        string get_launcher(){
            return m_launcher;
        }

        string get_launcher_descr(){
            return m_descr;
        }

        const vector<pair<string,pair<string,string>>>& get_all_descr(){
            return m_commands_register;
        }

        string get_output_file(){
            return m_output_file;
        }

        size_t get_output_max_bytes(){
            return m_output_max_bytes;
        }

        string get_start_response_token(){
            return m_start_response_token;
        }

        string get_end_response_token(){
            return m_end_response_token;
        }

        std::chrono::seconds get_launcher_delay(){
            return m_launcher_delay;
        }

        string get_current_llm_request_suffix(){
            return m_request_suffix;
        }

        size_t get_response_duration(){
            return m_response_duration;
        }

        void set_response_duration(size_t d){
            m_response_duration = d;
        }

        void set_default_response_duration(){
            m_response_duration = m_response_default_duration;
        }     

    private:
        const vector<pair<string,pair<string,string>>> m_commands_register{
        {"llm1", {"llama-2-7b OR DELETED", path_prefix + "llm/my_chat.sh"}},
        {"llm2", {"llama-2-13b-chat.Q4_0 (ENG)", path_prefix + "llm/my_chat2.sh"}},
        {"llm3", {"firefly-llama2-13b-chat.Q6_K (CHINESE)", path_prefix + "llm/my_chat3.sh"}},
        {"llm4", {"llama2-13b-estopia.Q8_0 (ENG)", path_prefix + "llm/my_chat4_estopia_13_8K.sh"}},
        {"llm5", {"ruGPT-3.5-13B-Q8_0 (RUS)", path_prefix + "llm/my_chat5_ruGPT_13.sh"}}
        };

        const size_t            m_default_llm{3};            
        string                  m_launcher;
        string                  m_descr;
        string                  m_request_suffix;
        size_t                  m_response_duration;
        size_t                  m_response_default_duration;
        std::chrono::seconds    m_launcher_delay;

        const string            m_output_file;
        const size_t            m_output_max_bytes{8192};            
        const string            m_start_response_token{"<s>"};
        const string            m_end_response_token{"</s>"};

};

#endif
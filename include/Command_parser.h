#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

class CommandParser{
    public:
        static bool parseDelayMessage(const std::string& message, const std::string& canary_token){

            std::vector<std::string> tokens;
            std::stringstream  mySstream(message);
            std::string tempStr;
            while(getline( mySstream, tempStr, ' ') ) {            
                tokens.push_back( tempStr );
            }

            if(!tokens.empty() && tokens[0] == canary_token){
                m_val = stoi(tokens[1]);

                std::cout << "isDelayMessage=TRUE val=" << m_val << std::endl;
                return true;
            }

            std::cout << "isDelayMessage=FALSE" << std::endl;
            return false;
        }

        static std::int64_t delayValue(){
            return m_val;
        }

    private:
        static inline std::int64_t m_val{-1};
};

#endif
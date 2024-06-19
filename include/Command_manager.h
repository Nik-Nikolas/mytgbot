#ifndef COMMAND_MANAGER_H
#define COMMAND_MANAGER_H

class CommandManager{
    public:

        static const vector<pair<string,pair<string,string>>>& get_register(){
            return commands_register;
        }
        
    private:
        inline static const vector<pair<string,pair<string,string>>> commands_register{
        {"stat", {"Общая статистика чата", ""}}
        };
};

#endif
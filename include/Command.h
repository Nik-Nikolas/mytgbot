#ifndef COMMAND_H
#define COMMAND_H

#include <cstdint>
struct Command{
    string                                  m_command;
    string                                  m_desctipt;
    function<void(int64_t, const string&)>  m_action;
};

#endif
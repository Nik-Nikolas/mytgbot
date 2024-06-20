#ifndef COMMAND_H
#define COMMAND_H

#include <cstdint>
struct Command{
    std::string                                             m_command;
    std::string                                             m_desctipt;
    std::function<void(std::int64_t, const std::string&)>   m_action;
};

#endif
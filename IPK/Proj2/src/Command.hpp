/**
 * @file Command.hpp
 * @author Radim Dvořák (xdvorar00)
 * @date 2025-04-19
 * @brief Declaration of class Command, representation of command
 */

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <vector>

typedef enum {
    CMD_AUTH,       // Authentication command, used to authenticate the user
    CMD_JOIN,       // Join command, used to join a channel
    CMD_RENAME,     // Locally rename the user
    CMD_MSG,        // Message command, used to send a message
    CMD_HELP,       // Prints help
    CMD_EXIT,       // Exit command, same functionality as SIGINT signal
    CMD_ERR         // Command was not recognized
} CommandType_t;

/**
 * @class Command
 * @brief Represents a command given by the user.
 */
class Command{
public:
    CommandType_t type;
    std::vector<std::string> parameters;

    Command(CommandType_t type,std::vector<std::string> parameters);
};

#endif
/**
 * @file Command.cpp
 * @author Radim Dvořák (xdvorar00)
 * @date 2025-04-17
 * @brief Implementation of command
 */

#include "Command.hpp"

Command::Command(CommandType_t type,std::vector<std::string> parameters){
    this->type = type;
    this->parameters = parameters;
}
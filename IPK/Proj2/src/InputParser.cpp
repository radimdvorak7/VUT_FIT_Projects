/**
 * @file InputParser.cpp
 * @author Radim Dvořák (xdvorar00)
 * @date 2025-04-20
 * @brief Implementation of parsing of commands from user
 */

#include "InputParser.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

Command InputParser::Parse() {
    std::string input;
    std::getline(std::cin, input);

    // Remove leading and trailing whitespace
    size_t pos = input.find(' ');
    // Set first word as main part of the command
    std::string command = input.substr(0, pos);

    std::vector<std::string> parameters;

    // Parse the remaining words as parameters
    while (pos != std::string::npos)
    {
        size_t next_pos = input.find(' ', pos + 1);
        std::string arg = input.substr(pos + 1, next_pos - pos - 1);

        parameters.push_back(arg);

        pos = next_pos;
    }

    // Check the commnand type and return the corresponding Command object
    if(command == "/auth"){
        if (parameters.size() != 3){
            throw std::invalid_argument("Wrong number of arguments");
        }
        else{
            return Command{CMD_AUTH, parameters};
        }
    }
    else if(command == "/join"){
        if (parameters.size() != 1){
            throw std::invalid_argument("Wrong number of arguments");
        }
        else{
            return Command{CMD_JOIN, parameters};
        }
    }
    else if(command == "/rename"){
        if (parameters.size() != 1){
            throw std::invalid_argument("Wrong number of arguments");
        }
        else{
            return Command{CMD_RENAME, parameters};
        }
    }
    else if(command == "/help"){
        if (parameters.size() != 0){
            throw std::invalid_argument("Wrong number of arguments");
        }
        else{
            return Command(CMD_HELP, {});
        }
    }
    // Extra command for terminating the program
    else if(command == "/exit"){
        if (parameters.size() != 0){
            throw std::invalid_argument("Wrong number of arguments");
        }
        else{
            return Command(CMD_EXIT, {});
        }
    }
    // Incorrect command
    else if(command.at(0) == '/'){
        parameters.insert(parameters.begin(), command);
        return Command(CMD_ERR, parameters);
    }

    // The input isn't a command
    parameters.insert(parameters.begin(), command);
    return Command(CMD_MSG, parameters);
}
/**
 * @file InputParser.hpp
 * @author Radim Dvořák (xdvorar00)
 * @date 2025-04-20
 * @brief Declaration of class InputParser, parsing commands from user
 */

#ifndef INPUT_PARSER_HPP
#define INPUT_PARSER_HPP
 
#include "Command.hpp"

#include <regex>
 
/**
 * @class InputParser
 * @brief A class responsible for parsing user commands into Command objects.
 */
class InputParser{
public:
    InputParser() {};     
    /**
     * @brief Parses the user input and returns a Command object.
     *
     * @return Command The parsed command object.      
     */
    static Command Parse();
};
 
#endif
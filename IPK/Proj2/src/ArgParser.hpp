/**
 * @file ArgParser.hpp
 * @author Radim Dvořák (xdvorar00)
 * @date 2025-04-20
 * @brief Declaration of class ArgParser, parsing command line options
 */

#ifndef ARG_PARSER_HPP
#define ARG_PARSER_HPP

#include "Options.hpp"

class ArgParser{
public:
    ArgParser() {};  

    /**
     * @brief Parses the command-line arguments.
     *
     * @param argc The number of command-line arguments.
     * @param argv The array of command-line argument strings.
     * @return Options A structured representation of the parsed options.
     */
    Options Parse(int argc, char *argv[]);
    /**
     * @brief Displays help information for using the program.
     */
    static void Help(); 
};

#endif
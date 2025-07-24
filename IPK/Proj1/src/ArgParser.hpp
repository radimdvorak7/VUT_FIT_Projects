/**
 * @file ArgParser.hpp
 * @author Radim Dvořák (xdvorar00)
 * @date 27/03/2025
 * @brief Declaration of class ArgParser, parsing command line options
 */

#ifndef ARG_PARSER_HPP
#define ARG_PARSER_HPP

#include <iostream>
#include <vector>

/**
 * @class ArgParser
 * @brief A class parsing command line arguments.
 */
class ArgParser{
    public:
        typedef struct {
            std::string inteface;
            std::vector<unsigned int> tcp_port;
            std::vector<unsigned int> udp_port;
            std::string domain;
            int timeout;
            bool interface_specified;
            bool help;
        } Options;

        static ArgParser::Options Parse(int argc, char *argv[]);
        static void Help();      
    private:
        static std::vector<unsigned int> ParsePorts(std::string ports_string);
};

#endif
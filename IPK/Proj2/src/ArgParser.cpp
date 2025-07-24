/**
 * @file ArgParser.cpp
 * @author Radim Dvořák (xdvorar00)
 * @date 2025-04-20
 * @brief Implementation of command line parsing
 */

#include "ArgParser.hpp"

#include <iostream>
#include <stdexcept>
#include <getopt.h>

Options ArgParser::Parse(int argc, char *argv[]){

    Options options;

    int opt = 0;

    // Disable error message from function getopt
    opterr = 0;
    while((opt = getopt(argc, argv, "t:s:p:d:r:h")) != -1){
        switch (opt)
        {
        // Option for protocol
        case 't':
            if(std::string(optarg) == "udp"){
                options.protocol = UDP;
            }
            else if(std::string(optarg) == "tcp"){
                options.protocol = TCP;
            }
            else{
                throw std::invalid_argument(std::string("Entered wrong protocol"));
            }
            break;
        // Option for server address or hostname
        case 's':
            options.hostname = optarg;
            break;
        // Option for port
        case 'p':
            options.port = atoi(optarg);
            break;
        // Option for timeout
        case 'd':
            options.timeout = atoi(optarg);
            break;
        // Option for retransmission
        case 'r':
            options.retries = atoi(optarg);
            break;
        case 'h':
            options.help = true;
            break;
        case '?':
            throw std::invalid_argument(std::string("Invalid option"));
        default:
            // Should not happen.
            throw std::invalid_argument(std::string("Invalid option"));
        }
    }
    // Check if the help option was set
    if(options.help){
        this->Help();
        exit(0);
    }
    // Check if the mandatory options (protocol, hostname) were set
    if(options.hostname.empty() || options.protocol == UNDEFINED){
        throw std::invalid_argument(std::string("Missing mandatory option"));
    }

    return options;
}

void ArgParser::Help(){
    std::cout << "Client for a chat server using the IPK25-CHAT protocol" << std::endl 
    << std::endl <<
    "Usage: ./ipk25chat-client [-t protocol] [-s domain-name | ip-address] {-p port}"
    " {-d timeout} {-r retransmission} {-h}"<< std::endl
    << std::endl <<
    "Option:" << std::endl <<
    "  -t <PROTOCOL>                            Transport protocol used for connection"<< std::endl <<
    "  -s <DOMAIN> | <IP_ADRESS>                Server IP or hostname"<< std::endl <<
    "  -p <PORT>                                Server port (default 4567)"<< std::endl <<
    "  -d <TIMEOUT>                             UDP confirmation timeout in milliseconds (default 250)"<< std::endl <<
    "  -r <RETRANSMISSION>                      Maximum number of UDP retransmissions (default 3)"<< std::endl <<
    "  -h                                       Print this message." << std::endl;
};

/**
 * @file ArgParser.cpp
 * @author Radim Dvořák (xdvorar00)
 * @date 27/03/2025
 * @brief Implementation of command line parsing
 */

#include "ArgParser.hpp"
#include "Network.hpp"
#include <getopt.h>
#include <vector>
#include <sstream>
#include <bits/stdc++.h>

/**
 * @brief Parses command line arguments.
 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @return A struct containing the parsed options.
 */
ArgParser::Options ArgParser::Parse(int argc, char *argv[]){

    ArgParser::Options options = {
        .inteface = "",
        .tcp_port = {},
        .udp_port = {},
        .domain = "",
        .timeout = 5'000,
        .interface_specified = false,
        .help = false,
    };

    const struct option longOptions[] = {
        // Argument 'interface' must be defined here to not have argument, optional argument is then handled separately
        {"interface", no_argument, nullptr, 'i'},
        {"pu", required_argument, nullptr, 'u'},
        {"pt", required_argument, nullptr, 't'},
        {"wait",required_argument,nullptr,'w'},
        {"help", no_argument, nullptr, 'h'}
    };

    int opt = 0;
    int optIndex = 0;

    // Disable error message from function getopt_long
    opterr = 0;
    while((opt = getopt_long(argc, argv, "iu:t:w:h", longOptions, &optIndex)) != -1){
        switch (opt)
        {
        case 'i':
            // Handling of optional argument to option '-i' or '--interface',
            // for which i am using solution found here: https://cfengine.com/blog/2021/optional-arguments-with-getopt-long/
            if (optarg == nullptr && optind < argc && argv[optind][0] != '-') {
                optarg = argv[optind++];  
            }
            if(optarg != nullptr){
                options.inteface = optarg;
                options.interface_specified = true;
                if(!Network::IsValidInterface(options.inteface)){
                    throw std::invalid_argument(std::string("Invalid interface"));
                }
            }
            break;
        case 'u':
            options.udp_port = ArgParser::ParsePorts(optarg);
            break;
        case 't':
            options.tcp_port = ArgParser::ParsePorts(optarg);
            break;
        case 'h':
            options.help = true;
            break;
        case 'w':
            options.timeout = atoi(optarg);
            break;
        case '?':
            throw std::invalid_argument(std::string("Invalid option"));
        default:
            // Should not happen.
            throw std::invalid_argument(std::string("Invalid option"));
        }
    }

    if(argv[optind] != nullptr){
        options.domain = std::string(argv[optind]);
    }
    
    if(options.interface_specified && (options.domain.empty() || (options.tcp_port.empty() && options.udp_port.empty()))){
        throw std::invalid_argument(std::string("Missing mandatory option"));
    }

    return options;
}

/**
 * @brief Parses a string representing port ranges into a ordered vector of integers.
 *
 * @param ports_string A string representing the port or range of ports.
 * @return A sorted vector of ports.
 */
std::vector<unsigned int> ArgParser::ParsePorts(std::string ports_string){
    std::vector<unsigned int> vec;

    if (ports_string.find('-') != std::string::npos) {
        size_t hyphenPos = ports_string.find('-');
        int start = std::stoi(ports_string.substr(0, hyphenPos));
        int end = std::stoi(ports_string.substr(hyphenPos + 1));

        // Add all numbers in the range to the vector
        for (int i = start; i <= end; i++) {
            vec.push_back(i);
        }
    }

    else if (ports_string.find(',') != std::string::npos) {
        std::stringstream ss(ports_string);
        std::string token;
        while (std::getline(ss, token, ',')) {
            vec.push_back(std::stoi(token));
        }
    }

    else {
        vec.push_back(std::stoi(ports_string));
    }

    if(vec.empty()){
        throw std::invalid_argument(std::string("Invalid format of argument"));
    }

    // Sort ports for better readbility
    std::sort(vec.begin(),vec.end());

    return vec;
}

/**
 * @brief Displays the help message for the TCP and UDP network L4 scanner.
 *
 * This function prints usage instructions and details about the available
 * command-line options for the scanner.
 */
void ArgParser::Help(){
    std::cout << "TCP and UDP network L4 scanner\n"
    "\n"
    "Usage: ./ipk-l4-scan [-i interface | --interface interface]"
    " [--pu port-ranges | --pt port-ranges | -u port-ranges | -t port-ranges]"
    " {-w timeout} [domain-name | ip-address] [-h | --help]\n"
    "\n"
    "Option:\n"
    "  -i, --interface <INTERFACE>             Choose the interface to scan through.\n"
    "  -t, --pt <PORT>                         TCP ports to be scanned.\n"
    "  -u, --pu <PORT>                         UDP ports to be scanned.\n"
    "  -w, --wait <NUMBER>                     Wait time for response of each scan in milliseconds (default 5000).\n"
    "  -h, --help                              Print this message." << std::endl;
};

/**
 * @file main.cpp
 * @author Radim Dvořák (xdvorar00)
 * @date 27/03/2025
 * @brief Main function of L4 scanner
 */

#include "ArgParser.hpp"
#include "Network.hpp"
#include "Udp.hpp"
#include "Tcp.hpp"
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

/**
 * @brief Iterates over a list of ports and calls scanning function.
 * 
 * @param ports A vector containing the list of port numbers to be scanned.
 * @param Scanner A pointer to the Scanner object, which can do UDP or TCP scanning
 * @param adress The address to be scanned.
 */
void ForEachPort(std::vector<unsigned int> ports, Scanner *Scanner,std::string adress) {
    if (ports.empty()) {
        return;
    }

    for(auto port : ports){
        Scanner->ScanPort(port,adress);
    }
}

/**
 * @brief Handles the CTRL+C signal.
 */
void HandleSigint(int sig) { 
    (void)sig;
    exit(EXIT_FAILURE);
} 

int main(int argc, char *argv[]){
    srand(time(0));
    
    ArgParser::Options options;

    signal(SIGINT, HandleSigint); 

    try
    {
        options = ArgParser::Parse(argc,argv);

        if(options.help){
            ArgParser::Help();
            return EXIT_SUCCESS;
        }

        if(!options.interface_specified){
            Network::ListInterfaces();
            return EXIT_SUCCESS;
        }

        struct sockaddr_storage src_addr = {};
        //socklen_t src_len = sizeof(struct sockaddr_storage);
        auto DstAdresses = Network::GetDstAdress(options.domain);

        // For each IP address
        for(auto dst_addr = DstAdresses; dst_addr != nullptr; dst_addr = dst_addr->ai_next){

            memcpy(&src_addr, dst_addr->ai_addr, dst_addr->ai_addrlen);

            sockaddr_in *adress_IP4;
            sockaddr_in6 *adress_IP6;

            Network::GetInterfaceAdress(dst_addr->ai_family, options.inteface,&src_addr);

            // Initialization of scanner, and both objects for UDP and TCP scanner
            Scanner *scanner;
            UDPScanner udpScanner(&src_addr, (struct sockaddr_storage *)dst_addr->ai_addr);
            udpScanner.SetupScanner(options);
            TCPScanner tcpScanner(&src_addr, (struct sockaddr_storage *)dst_addr->ai_addr);
            tcpScanner.SetupScanner(options);

            // For each family of IP adress of host
            switch (dst_addr->ai_family)
            {
            case AF_INET:
                // Gets string version of IPV4 of host
                adress_IP4 = (struct sockaddr_in *)dst_addr->ai_addr;
                char ipv4[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(adress_IP4->sin_addr), ipv4, INET_ADDRSTRLEN);

                // Performing of UDP and TCP scanning
                scanner = &udpScanner;
                ForEachPort(options.udp_port, scanner,ipv4);
                scanner = &tcpScanner;
                ForEachPort(options.tcp_port, scanner, ipv4);
                break;

            case AF_INET6:
                // Gets string version of IPV4 of host
                adress_IP6 = (struct sockaddr_in6 *)dst_addr->ai_addr;
                char ipv6[INET6_ADDRSTRLEN];
                inet_ntop(AF_INET6, &(adress_IP6->sin6_addr), ipv6, INET6_ADDRSTRLEN);

                // Performing of UDP and TCP scanning
                scanner = &udpScanner;
                ForEachPort(options.udp_port, scanner, ipv6);
                scanner = &tcpScanner;
                ForEachPort(options.tcp_port, scanner, ipv6);
                break;
            
            default:
                break;
            }
        }
        
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        ArgParser::Help();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

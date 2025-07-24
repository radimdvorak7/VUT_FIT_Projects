/**
 * @file Network.hpp
 * @author Radim Dvořák (xdvorar00)
 * @date 27/03/2025
 * @brief Declaration of class Network, all network related functions
 */

#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>

/**
 * @class ArgParser
 * @brief A class for network purposes, getting IP adresses, checking availability of interfaces
 */
class Network{
    public:
        struct PseudoHeaderIPv4 {
        uint32_t src;
        uint32_t dst;
        uint8_t zeroes;
        uint8_t protocol;
        uint16_t len;
    };

    struct PseudoHeaderIPv6 {
        struct in6_addr src;
        struct in6_addr dst;
        uint32_t len;
        uint8_t zeroes[3];
        uint8_t protocol;
    };

        static void ListInterfaces();
        static void GetInterfaceAdress(sa_family_t family, const std::string name,struct sockaddr_storage* src_addr);
        static bool IsValidInterface(const std::string name);
        static int CreateSocket(const int addr_family,int socket_type, const int protocol);
        static void SetupSocket(int socketfd, int socket_level,int socket_name, const void* socket_value);
        static uint16_t GetPort(const struct sockaddr_storage* addr);
        static void SetPort(struct sockaddr_storage* addr, const uint16_t port);
        static struct addrinfo* GetDstAdress(std::string name);
    private:
        static std::string GetSubnetMaskIP4(const char* iface);
        static std::string ConvertToCIDR(const std::string& mask,char delim);
};

#endif
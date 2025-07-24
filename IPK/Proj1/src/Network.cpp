/**
 * @file Network.cpp
 * @author Radim Dvořák (xdvorar00)
 * @date 27/03/2025
 * @brief Implementation of network functions
 */

#include "Network.hpp"
#include <bitset>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <memory>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h> 
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

/**
  * @brief Retrieves the subnet mask for a given network interface.
  * @param interface_name The name of the network interface.
  * @return The subnet mask in string format.
  */
std::string Network::GetSubnetMaskIP4(const char* interface_name) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd <= 0) {
        throw std::runtime_error(std::string("Could not create socket"));
    }

    struct ifreq ifr;
    std::strncpy(ifr.ifr_name, interface_name, IFNAMSIZ - 1);

    if (ioctl(fd, SIOCGIFNETMASK, &ifr) == -1) {
        close(fd);
        throw std::runtime_error(std::string("Ioctl failed"));
    }

    struct sockaddr_in* mask = (struct sockaddr_in*)&ifr.ifr_netmask;;
    std::string subnet_mask(inet_ntoa(mask->sin_addr));
    close(fd);

    return subnet_mask;
}

/**
 * @brief Converts a subnet mask to CIDR notation.
 * @param mask The subnet mask in string format.
 * @param delim The delimiter used in the subnet mask.
 * @return The CIDR notation as a string.
 */
std::string Network::ConvertToCIDR(const std::string& mask,char delim) {
    std::istringstream mask_stream(mask);
    std::string octet;
    int cidr = 0;

    while (std::getline(mask_stream ,octet, delim)) {  
        int num = std::stoi(octet);
        cidr += std::bitset<8>(num).count();
    }

    return std::to_string(cidr);
}

/**
 * @brief Lists all network interfaces and their associated IP addresses.
 */
void Network::ListInterfaces(){
    struct ifaddrs *addrs;
    getifaddrs(&addrs);
    std::cout << "***********************INTERFACES***********************"<< std::endl;
    std::cout << std::left << std::setw(15) <<  "DEV" << std::setw(15) <<
    "IP TYPE" << std::setw(15) << "IP ADRESS" << std::endl;

    for (struct ifaddrs *addr = addrs; addr != nullptr; addr = addr->ifa_next) {
        if (addr->ifa_addr && addr->ifa_addr->sa_family == AF_INET) {

            // Find IPV4 addres of interface
            auto tmpAddrPtr = &((struct sockaddr_in *)addr->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];

            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);

            // Output name, IPV4 address and mask of interface
            std::string string_mask(addressBuffer);
            auto mask = Network::GetSubnetMaskIP4(addr->ifa_name);
            string_mask.append("/").append(Network::ConvertToCIDR(mask,'.'));

            std::cout << std::left << std::setw(15) << addr->ifa_name << std::setw(15) 
            << "IPV4" << std::setw(15) << string_mask << std::endl;
        }
        else if (addr->ifa_addr && addr->ifa_addr->sa_family == AF_INET6) {

            // Find IPV6 addres of interface
            auto tmpAddrPtr = &((struct sockaddr_in6 *)addr->ifa_addr)->sin6_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
            
            // Output name and IPV6 address of interface
            std::cout << std::left << std::setw(15) << addr->ifa_name << std::setw(15) 
            << "IPV6" << std::setw(15) << addressBuffer << std::endl;
        }
    }

    freeifaddrs(addrs);
}

/**
 * @brief Retrieves the address of a specific network interface.
 * @param family The internet address family.
 * @param name The name of the network interface.
 * @param src_addr A pointer to a structure to store the address.
 */
void Network::GetInterfaceAdress(sa_family_t family, const std::string name,struct sockaddr_storage* src_addr){
    struct ifaddrs *addrs;
    getifaddrs(&addrs);
    for (struct ifaddrs *addr = addrs; addr != nullptr; addr = addr->ifa_next) {
        if(name.compare(addr->ifa_name)){
                continue;
        }
        if (addr->ifa_addr && addr->ifa_addr->sa_family == AF_INET && addr->ifa_addr->sa_family == family) {

            ((struct sockaddr_in *)src_addr)->sin_addr = ((struct sockaddr_in *)addr->ifa_addr)->sin_addr;
            return;
        }
        else if (addr->ifa_addr && addr->ifa_addr->sa_family == AF_INET6 && addr->ifa_addr->sa_family == family) {

            ((struct sockaddr_in6 *)src_addr)->sin6_addr = ((struct sockaddr_in6 *)addr->ifa_addr)->sin6_addr;
            return;
            
        }
    }
    throw std::runtime_error(std::string("Didnt find any interface"));
}

/**
 * @brief Checks if a given network interface is valid.
 * @param name The name of the network interface.
 * @return True if the interface is valid, false otherwise.
 */
bool Network::IsValidInterface(const std::string name){
    return if_nametoindex(name.data());
}

/**
 * @brief Creates a socket with the specified parameters.
 * @param addr_family The internet address family.
 * @param socket_type The socket type.
 * @param protocol The protocol to be used.
 * @return The file descriptor of the created socket.
 */
int Network::CreateSocket(const int addr_family,int socket_type, const int protocol){
    int fd = socket(addr_family,socket_type, protocol);
    if (fd == -1) {
        throw std::runtime_error(std::string("Could not create socket"));
    }
    
    return fd;
}

/**
 * @brief Configures a socket with the specified options.
 * @param socketfd The file descriptor of the socket.
 * @param socket_level The protocol level at which the option resides.
 * @param socket_name The name of the option to set.
 * @param socket_value A pointer to the value for the option.
 */
void Network::SetupSocket(int socketfd, int socket_level, int socket_name, const void* socket_value) {
    if (setsockopt(socketfd, socket_level, socket_name, socket_value, sizeof(socket_value)) == -1) {
        close(socketfd);
        throw std::runtime_error(std::string("Could not set up the socket"));
    }
}

/**
 * @brief Retrieves the port number from a given structure of adress.
 * @param addr A pointer to the sockaddr_storage structure.
 * @return The port number.
 */
uint16_t Network::GetPort(const struct sockaddr_storage* addr) {
    switch (addr->ss_family) {
        case AF_INET: {
            struct sockaddr_in* ipv4 = (struct sockaddr_in*)addr;
            return ntohs(ipv4->sin_port);
        }
        case AF_INET6: {
            struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)addr;
            return ntohs(ipv6->sin6_port);
        }
        default:
            throw std::runtime_error(std::string("Invalid family of address"));
    }
}

/**
 * @brief Sets the port number in a sockaddr_storage structure.
 * @param addr A pointer to the sockaddr_storage structure.
 * @param port The port number to set.
 */
void Network::SetPort(struct sockaddr_storage* addr, const uint16_t port) {
    switch (addr->ss_family) {
        case AF_INET: {
            struct sockaddr_in* ipv4 = (struct sockaddr_in*)addr;
            ipv4->sin_port = htons(port);
            break;
        }
        case AF_INET6: {
            struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)addr;
            ipv6->sin6_port = htons(port);
            break;
        }
        default:
            throw std::runtime_error(std::string("Invalid family of address"));
    }
}
/**
 * @brief Resolves by DNS request the destination address for a given hostname.
 * @param name The hostname to resolve.
 * @return A pointer to a linked list of addrinfo structures.
 */
struct addrinfo* Network::GetDstAdress(const std::string name){
    struct addrinfo hints;
    struct addrinfo *result;
    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = 0;

    if (getaddrinfo(name.data(), nullptr, &hints, &result) != 0) {
        throw std::runtime_error(std::string("Could not get address info"));
    }
    return result;
}
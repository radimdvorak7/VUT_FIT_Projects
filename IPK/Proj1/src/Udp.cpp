/**
 * @file Udp.cpp
 * @author Radim Dvořák (xdvorar00)
 * @date 27/03/2025
 * @brief Implementation of UDP scanning
 */

#include "Udp.hpp"
#include "Network.hpp"
#include <unistd.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/ip_icmp.h>
#include <netinet/icmp6.h>
#include <netinet/udp.h>

/**
 * @brief Destructor for the UDPScanner class. Closes the send and receive sockets.
 */
UDPScanner::~UDPScanner()
{
    close(this->sendfd);
    close(this->recvfd);
}

/**
 * @brief Sets up the UDP scanner by creating sockets for sending and receiving packets.
 * 
 * @param options Command-line options parsed used to setup timeout of scanner.
 */
void UDPScanner::SetupScanner(ArgParser::Options options){
    this->timeout = {.tv_sec = 0, .tv_usec = options.timeout * 1000};

    auto recv_family = this->src_addr->ss_family == int(AF_INET) ? int(IPPROTO_ICMP) : int(IPPROTO_ICMPV6);
    this->sendfd = Network::CreateSocket(this->src_addr->ss_family,SOCK_DGRAM, IPPROTO_UDP);

    this->recvfd = Network::CreateSocket(this->dst_addr->ss_family,SOCK_RAW,recv_family);
}

/**
 * @brief Scans a specific UDP port on a given address.
 * 
 * @param Port The port number to scan.
 * @param adress The target address to scan.
 */
void UDPScanner::ScanPort(unsigned int Port,std::string adress){
    Network::SetPort(this->dst_addr, Port);
    sendto(this->sendfd, "", 0, 0,(struct sockaddr *)this->dst_addr,(this->dst_addr->ss_family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6));
    bool isOpen = this->HandlePacket(Port);

    printf("%s %d udp %s\n",adress.data(),Port, isOpen? "open":"closed");
}

/**
 * @brief Handles incoming ICMP packets to determine if the scanned port is open or closed.
 * 
 * @param Port The port number being scanned.
 * @return true If the port is open or false If the port is closed.
 */
bool UDPScanner::HandlePacket(unsigned int Port){

    fd_set readfds;

    struct timeval timeout;
    timeout.tv_sec = this->timeout.tv_sec;
    timeout.tv_usec = this->timeout.tv_usec;

    FD_ZERO(&readfds);
    FD_SET(this->recvfd, &readfds);

    int received;
    switch (this->dst_addr->ss_family)
    {
    case AF_INET:
        uint8_t recv4_packet[sizeof(struct iphdr) + sizeof(struct icmphdr) + sizeof(udphdr)];
        

        received = select(this->recvfd + 1, &readfds, NULL, NULL, &timeout);
        if (received < 0) {
            throw std::runtime_error(std::string("Select failed "));
        } else if (received == 0) {
            return true;
        } else {
            socklen_t addr_len = sizeof(this->dst_addr);
            recvfrom(this->recvfd, recv4_packet, sizeof(recv4_packet), 0, (struct sockaddr *)this->dst_addr, &addr_len);

            auto ip_header = (iphdr*) recv4_packet;
            auto icmp_header = (icmphdr*)(recv4_packet + (ip_header->ihl * 4));
            auto udp_header = (udphdr*)(recv4_packet + (ip_header->ihl * 4) + sizeof(icmphdr));

            if(Port != udp_header->uh_dport){
                return false;
            }
            if (icmp_header->code == ICMP_PORT_UNREACH && icmp_header->type == ICMP_DEST_UNREACH){
                return false;
            }
            else{
                throw std::runtime_error(std::string("Incorrect ICMP type or code"));
            }
        }
    case AF_INET6:
        uint8_t recv6_packet[sizeof(iphdr) + sizeof(icmp6_hdr) + sizeof(udphdr)];
        
        received = select(this->recvfd + 1, &readfds, NULL, NULL, &timeout);
        if (received < 0) {
            throw std::runtime_error(std::string("Select failed "));
        } else if (received == 0) {
            return true;
        } else {
            socklen_t addr_len = sizeof(this->dst_addr);
            recvfrom(this->recvfd, recv6_packet, sizeof(recv6_packet), 0, (struct sockaddr *)this->dst_addr, &addr_len);

            auto ip_header = (iphdr*) recv6_packet;
            auto icmp6_header = (icmp6_hdr*)(recv6_packet + (ip_header->ihl * 4));
            if (icmp6_header->icmp6_code == ICMP6_DST_UNREACH && icmp6_header->icmp6_code == ICMP6_DST_UNREACH_NOPORT){
                return false;
            }
            else{
                throw std::runtime_error(std::string("Incorrect ICMP type or code"));
            }
        }
    
    default:
        throw std::runtime_error(std::string("Unknown family of adresses"));
    }
    
}
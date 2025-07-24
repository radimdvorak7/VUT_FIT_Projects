/** 
 * @file Tcp.cpp
 * @author Radim Dvořák (xdvorar00)
 * @date 27/03/2025
 * @brief Implementation of TCP scanning
 */

#include "Tcp.hpp"
#include "Network.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/tcp.h>

/**
 * @brief Destructor for the TCPScanner class.
 * 
 * Closes the file descriptors used for sending and receiving packets.
 */
TCPScanner::~TCPScanner()
{
    close(this->sendfd);
    close(this->recvfd);
}

/**
  * @brief Constructs a pseudo-header for checksum calculation.
  * 
  * @param buffer Pointer to the buffer where the pseudo-header will be stored.
  * @param protocol The protocol number AF_INET or AF_INET6.
  * @param len The length of the TCP segment.* 
  * @return The size of the pseudo-header, or -1 if the address family is unsupported.
  */
int TCPScanner::MakePseudoHeader(uint8_t *buffer, uint8_t protocol, uint16_t len) {
    switch (src_addr->ss_family) {
        case AF_INET: {
            Network::PseudoHeaderIPv4 *header = (Network::PseudoHeaderIPv4 *)buffer;
            header->src = ((struct sockaddr_in *)this->src_addr)->sin_addr.s_addr;
            header->dst = ((struct sockaddr_in *)this->src_addr)->sin_addr.s_addr;
            header->zeroes = 0;
            header->protocol = protocol;
            header->len = htons(len);

            return sizeof(Network::PseudoHeaderIPv4);
        }

        case AF_INET6: {
            struct sockaddr_in6 *src = (struct sockaddr_in6 *)this->src_addr;
            struct sockaddr_in6 *dst = (struct sockaddr_in6 *)this->dst_addr;
            Network::PseudoHeaderIPv6 *header = (Network::PseudoHeaderIPv6 *)buffer;

            memcpy(&header->src, &src->sin6_addr, sizeof(struct in6_addr));
            memcpy(&header->dst, &dst->sin6_addr, sizeof(struct in6_addr));
            memset(header->zeroes, 0, sizeof(header->zeroes));
            header->len = htonl((uint32_t)len);
            header->protocol = protocol;

            return sizeof(Network::PseudoHeaderIPv6);
        }

        default:
            return -1;
    }
}

/**
 * @brief Computes the TCP checksum.
 *
 * @param buf Pointer to the TCP header.
 * @param protocol The internet protocol number, AF_INET or AF_INET6.
 * @return The computed checksum.
 */
uint16_t TCPScanner::ComputeChecksum(uint8_t *buf, uint8_t protocol) {

    auto buf_len = sizeof(buf);
    auto data = (uint8_t *)malloc(buf_len + sizeof(Network::PseudoHeaderIPv6));
    if (!data) {
        throw std::runtime_error(std::string("Allocation failed"));
    }

    memset(data, 0, buf_len + sizeof(Network::PseudoHeaderIPv6));

    int pseudo_header_len = MakePseudoHeader(data, protocol, buf_len);

    memcpy(data + pseudo_header_len, buf, buf_len);

    int size = (pseudo_header_len + buf_len);

    uint32_t sum = 0;
    int i;

    for (i = 0; i < size - 1; i += 2) {
        uint16_t word16 = *(uint16_t *)&data[i];
        sum += word16;
    }
    
    // Handle odd-sized case.
    if (size & 1) {
        uint16_t word16 = (uint8_t)data[i];
        sum += word16;
    }
    
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    free(data);
    return (uint16_t)(~sum);
}

/**
 * @brief Sets up the TCP scanner with the provided options.
 * 
 * @param options The options parsed from the command-line arguments.
 */
void TCPScanner::SetupScanner(ArgParser::Options options){
    this->timeout = {.tv_sec = 0, .tv_usec = options.timeout * 1000};
    // Random port
    this->send_port =  rand() % (65535 - 2000 + 1) + 2000;
    this->sendfd = Network::CreateSocket(this->src_addr->ss_family,SOCK_RAW, IPPROTO_TCP);
    Network::SetupSocket(this->sendfd,SOL_SOCKET, SO_BINDTODEVICE, options.inteface.c_str());

    bind(sendfd,(struct sockaddr *)this->dst_addr,sizeof(struct sockaddr));

    this->recvfd = this->sendfd;
}

/**
 * @brief Scans a specific TCP port on a given address.
 * 
 * @param Port The port number to scan.
 * @param adress The target address as a string.
 */
void TCPScanner::ScanPort(unsigned int Port,std::string adress){

    Network::SetPort(this->dst_addr, Port);
    uint8_t packet[sizeof(struct tcphdr)];
    this->MakeHeader(Port,packet,TH_SYN);
    
    sendto(this->sendfd, packet, sizeof(packet), 0,(struct sockaddr *)this->dst_addr,sizeof(struct sockaddr_in));
    auto received = this->HandlePacket(Port);
    printf("%s %d tcp %s\n",adress.data(),Port,received == 0 ? "closed" : received == 1 ? "open" : "filtered");
}


/**
 * @brief Creates TCP header.
 * 
 * @param Port The destination port number.
 * @param packet Pointer to the buffer where the TCP header will be stored.
 * @param type The TCP flags, TH_SYN or TH_RST
 */
void TCPScanner::MakeHeader(unsigned int Port,uint8_t *packet, uint8_t type){

    struct tcphdr *tcp_header = (struct tcphdr *)packet;

    tcp_header->th_sport = htons(this->send_port);
    tcp_header->th_dport = htons(Port); // Destination port
    tcp_header->th_seq = htonl(this->SEQ_NUMBER++); // Sequence number
    tcp_header->th_ack = htonl(0); // Acknowledgment number
    tcp_header->th_off = 5; // Data offset (5 words = 20 bytes)
    tcp_header->th_flags = type;
    tcp_header->res1 = 0;
    tcp_header->th_win = htons(0xFFFF); // Window size
    tcp_header->th_sum = 0; // Checksum (will be computed later)
    tcp_header->th_urp = 0; // Urgent pointer

    tcp_header->th_sum = ComputeChecksum(
        packet,
        IPPROTO_TCP
    );
}

/**
 * @brief Handles the receive of TCP packet from host.
 * 
 * @param Port The port number being scanned.
 * @return  0 when port is closed, 1 if open, if filtered (no response).
 */
int TCPScanner::HandlePacket(unsigned int Port){
    fd_set readfds;

    struct timeval timeout;
    timeout.tv_sec = this->timeout.tv_sec;
    timeout.tv_usec = this->timeout.tv_usec;

    FD_ZERO(&readfds);
    FD_SET(this->recvfd, &readfds);


    uint8_t recv_packet[sizeof(iphdr) + sizeof(tcphdr)];

    int received;
        
    received = select(this->recvfd + 1, &readfds, NULL, NULL, &timeout);
    if (received < 0) {
        throw std::runtime_error(std::string("Select failed "));
    } else if (received == 0) {
        return 2;
    } else {
        socklen_t addr_len = sizeof(this->dst_addr);
        recvfrom(this->recvfd, recv_packet, sizeof(recv_packet), 0, (struct sockaddr *)this->dst_addr, &addr_len);

        auto ip_header = (iphdr*) recv_packet;
        auto tcp_header = (tcphdr*)(recv_packet + (ip_header->ihl * 4));

        if(tcp_header->th_flags & TH_RST){
            return 0;
        }
        else{
            uint8_t packet[sizeof(tcphdr)];
            this->MakeHeader(Port,packet,TH_RST);
            sendto(this->sendfd, packet, sizeof(packet), 0,(struct sockaddr *)this->dst_addr,sizeof(struct sockaddr));
            return 1;
        }
    }

}
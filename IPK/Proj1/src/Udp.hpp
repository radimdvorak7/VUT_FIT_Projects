/**
 * @file Udp.hpp
 * @author Radim Dvořák (xdvorar00)
 * @date 27/03/2025
 * @brief Declaration of class UDPScanner
 */

#ifndef UDP_HPP
#define UDP_HPP

#include "ArgParser.hpp"
#include "Network.hpp"
#include "Scanner.hpp"
#include <cstdint>
#include <cstring>
#include <arpa/inet.h>

class UDPScanner : public Scanner {
public:
    int sendfd;
    int recvfd;
    struct timeval timeout;

     /**
    * @brief Constructor for the UDPScanner class.
    */
    UDPScanner( struct sockaddr_storage *src_addr, struct sockaddr_storage *dst_addr) : Scanner{src_addr, dst_addr} {}
    ~UDPScanner();

    void SetupScanner(ArgParser::Options options);
    void ScanPort(unsigned int Port,std::string adress) override;
    bool HandlePacket(unsigned int Port);

};

#endif
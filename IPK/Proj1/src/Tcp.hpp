/**
 * @file Tcp.hpp
 * @author Radim Dvořák (xdvorar00)
 * @date 27/03/2025
 * @brief Declaration of class TCPScanner
 */

#ifndef TCP_HPP
#define TCP_HPP

#include "ArgParser.hpp"
#include "Network.hpp"
#include "Scanner.hpp"
#include <cstdint>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>

/**
 * @class TCPScanner
 * @brief A class for TCP scanning.
 */
class TCPScanner : public Scanner {
public:
    int sendfd;
    int recvfd;
    uint16_t SEQ_NUMBER;
    uint32_t send_port;

    TCPScanner( struct sockaddr_storage *dst_addr,struct sockaddr_storage *src_addr) : Scanner{dst_addr,src_addr} {}
    ~TCPScanner();

    void SetupScanner(ArgParser::Options options);
    void ScanPort(unsigned int Port,std::string adress) override;
    int HandlePacket(unsigned int Port);

private:
    void MakeHeader(unsigned int Port,uint8_t *packet, uint8_t type);
    int MakePseudoHeader(uint8_t *buffer, uint8_t protocol, uint16_t len);
    uint16_t ComputeChecksum(uint8_t *buf, uint8_t protocol);
};

#endif
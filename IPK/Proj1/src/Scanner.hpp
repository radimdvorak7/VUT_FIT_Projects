/**
 * @file Scanner.hpp
 * @author Radim Dvořák (xdvorar00)
 * @date 27/03/2025
 * @brief Declaration of abstract class Scanner
 */

#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <sys/socket.h>

class Scanner {
public:
    struct sockaddr_storage *src_addr;    ///< Source address
    struct sockaddr_storage *dst_addr;    ///< Destination address
    struct timeval timeout;

    // Constructor
    Scanner(struct sockaddr_storage *src_addr, struct sockaddr_storage *dst_addr) : src_addr{src_addr}, dst_addr{dst_addr} , timeout{0, 5000}{};
     
    virtual void ScanPort(unsigned int Port,std::string adress) = 0;
    virtual ~Scanner() = default;
};

#endif
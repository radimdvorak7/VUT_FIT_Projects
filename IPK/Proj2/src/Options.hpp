/**
 * @file Options.hpp
 * @author Radim Dvořák (xdvorar00)
 * @date 2025-04-19
 * @brief Declaration of class Options, which contains all parsed arguments
 */

#ifndef ARG_OPTIONS_HPP
#define ARG_OPTIONS_HPP

#include <cstdint>
#include <string>

typedef enum {
    UNDEFINED = -1,     // Initial state, not set
    UDP,                // UDP protocol
    TCP                 // TCP protocol
} Protocol_t;

/**
 * @class Options
 * @brief Represents configuration options for a set up of client.
 *
 */
class Options{
public:
    Protocol_t protocol;
    std::string hostname;
    uint16_t port ;
    long long timeout;
    uint8_t retries;
    bool help;

    Options(){
        this->protocol = UNDEFINED;
        this->hostname = std::string();
        this->port = 4567;
        this->timeout = 250;
        this->retries = 3;
        this->help = false;
    }
};

#endif
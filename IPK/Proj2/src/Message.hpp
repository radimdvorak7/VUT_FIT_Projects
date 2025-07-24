/**
 * @file Message.hpp
 * @author Radim Dvořák (xdvorar00)
 * @date 2025-04-19
 * @brief Declaration of class Message, representation of message to be sent or received
 */

#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <cstdint>
#include <string>

typedef enum MessageTypeEnum : uint8_t{
    CONFIRM = 0x00,      // Confirmation message
    REPLY = 0x01,        // Reply message
    AUTH = 0x02,         // Authentication message
    JOIN = 0x03,         // Join message
    MSG = 0x04,          // Message
    PING = 0xFD,         // Ping message
    ERR = 0xFE,          // Error message
    BYE = 0xFF,          // Bye message
    UNKNOWN = 0x05       // Unrecognized message    
} MessageType_t;

typedef enum{
    USER,       
    SERVER,
} PollResultType_t;

/**
 * @class Message
 * @brief Represents a message to be sent or received.
 */
class Message{
public:
    PollResultType_t poll_type;
    MessageType_t type = UNKNOWN;
    uint16_t ref_ID;
    uint16_t ID;
    bool result;
    std::string username;
    std::string display_name;
    std::string password;
    std::string channel_id;
    std::string contents;
};

#endif
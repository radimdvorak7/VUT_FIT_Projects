/**
 * @file Messenger.hpp
 * @author Radim Dvořák (xdvorar00)
 * @date 2025-04-20
 * @brief Declaration of class Messenger, abstract class that functions as receiver and sender
 */

#ifndef MESSENGER_HPP
#define MESSENGER_HPP

#include "Command.hpp"
#include "Message.hpp"

#include <deque>
#include <vector>

#include <netinet/in.h>

/**
 * @class Messenger
 * @brief Abstract class that serves as a base for both types of messengers (UDP, TCP).
 */
class Messenger {
public:
    Messenger() {display_name = "User";};
    virtual ~Messenger() = default;

    /**
     * @brief Authorizes the user with the server.
     *
     * @param message The message containing username, display name and secret.
     */
    virtual void Authorize(Message &message) = 0;
    /**
     * @brief Sends a bye message to the server.
     */
    virtual void Bye() = 0;
    /**
     * @brief Sends an error message to the server.
     *
     * @param message The message the malformed message from the server.
     */
    virtual void Err(Message &message) = 0;
    /**
     * @brief Sends a join message to the server.
     *
     * @param message The message containing channel ID.
     */
    virtual void Join(Message &message) = 0;
    /**
     * @brief Sends a message to the server.
     *
     * @param message The message to be sent.
     */
    virtual void Msg(Message &message) = 0;
    /**
     * @brief Polls for messages from the server and user input.
     *
     * @return A queue of messages received from the server and user input.
     */
    virtual std::deque<Message> PollMessage() = 0;
    /**
     * @brief Handles user commands and returns the corresponding message.
     *
     * @return The message corresponding to the user command.
     */
    virtual Message HandleUserCommand() = 0;
    /**
     * @brief Receives a message from the server. 
     *
     * @return The message received from the server.
     */
    virtual Message ReceiveFromServer() = 0;

protected:
    int socketfd;
    std::string display_name;
    struct sockaddr_in server_address;
    Message last_message;
};

#endif
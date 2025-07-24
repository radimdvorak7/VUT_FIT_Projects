/**
 * @file TCPMessenger.hpp
 * @author Radim Dvořák (xdvorar00)
 * @date 2025-04-20
 * @brief Declaration of class TCPMessenger, which communicates with server via TCP
 */

#ifndef TCP_MESSENGER_HPP
#define TCP_MESSENGER_HPP

#include "Messenger.hpp"
#include "Options.hpp"

/**
 * @class TCPMessenger
 * @brief A class that communicates with the server using TCP protocol.
 */
class TCPMessenger : public Messenger{
public:
    static bool hadSigint;

    TCPMessenger(Options &options);
    ~TCPMessenger();

    /**
     * @brief Handles the SIGINT signal.
     *
     * @param signal The signal number.
     */
    static void HandleSigint(int signal);
    
     /**
      * @brief Authorizes the user with the server.
      *
      * @param message The message containing username, display name and secret.
      */
    void Authorize(Message &message) override;
    /**
     * @brief Sends a bye message to the server.
     */
    void Bye() override;
    /**
     * @brief Sends an error message to the server.
     *
     * @param message The message the malformed message from the server.
     */
    void Err(Message &message) override;
    /**
     * @brief Sends a join message to the server.
     *
     * @param message The message containing channel ID.
     */
    void Join(Message &message) override;
    /**
     * @brief Sends a message to the server.
     *
     * @param message The message to be sent.
     */
    void Msg(Message &message) override;
    /**
     * @brief Polls for messages from the server and user input.
     *
     * @return A queue of messages received from the server and user input.
     */

    std::deque<Message> PollMessage() override;
    /**
     * @brief Handles user commands and returns the corresponding message.
     *
     * @return The message corresponding to the user command.
     */
    Message HandleUserCommand() override;
    /**
     * @brief Receives a message from the server. 
     *
     * @return The message received from the server.
     */
    Message ReceiveFromServer() override;
};

#endif
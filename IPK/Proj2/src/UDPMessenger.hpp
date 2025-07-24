/**
 * @file UDPMessenger.hpp
 * @author Radim Dvořák (xdvorar00)
 * @date 2025-04-20
 * @brief Declaration of class UDPMessenger, which communicates with server via UDP
 */

#ifndef UDP_MESSENGER_HPP
#define UDP_MESSENGER_HPP
 
#include "Messenger.hpp"
#include "Options.hpp"
 
/**
 * @class UDPMessenger
 * @brief A class that communicates with the server using UDP protocol.
 */
class UDPMessenger : public Messenger{
public:
    static bool hadSigint; 

    UDPMessenger(Options &options);
    ~UDPMessenger();

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
     * @brief Sends a confirmation message to the server.
     *
     * @param ref_message_id The ID of the message to confirm.
     */
    void Confirm(uint16_t ref_message_id);
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

private:
    // Socket file descriptor
    uint16_t message_id;
    // Max number of retrransmissions
    uint8_t max_retries;
    // Timeout for retransmission
    long long max_timeout;    
};
 
#endif
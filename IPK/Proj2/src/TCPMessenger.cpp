/**
 * @file TCPMessenger.cpp
 * @author Radim Dvořák (xdvorar00)
 * @date 2025-04-23
 * @brief Implementation of TCP messenger
 */

#include "TCPMessenger.hpp"
#include "Command.hpp"
#include "InputParser.hpp"

#include <iostream>
#include <cstring>
#include <csignal>
#include <memory>
#include <stdexcept>
#include <regex>

#include <unistd.h>
#include <poll.h>
#include <sys/socket.h>
#include <netdb.h>

bool TCPMessenger::hadSigint = false;

void TCPMessenger::HandleSigint(int signal){
    if (signal == SIGINT){
        TCPMessenger::hadSigint = true;
    }
}

TCPMessenger::TCPMessenger(Options &options) : Messenger::Messenger() {
    signal(SIGINT,this->HandleSigint);
    
    // Create socket
    this->socketfd = socket(AF_INET,SOCK_STREAM,0);
    if (this->socketfd < 0)
    {
        throw std::runtime_error("Can't open socket");
    }

    std::memset(&(this->server_address), 0, sizeof(this->server_address));

    // Set up server address
    struct hostent *server = gethostbyname(options.hostname.c_str());
    if (server == nullptr)
    {
        throw std::runtime_error("Coudn't resolve host");
    }
    
    server_address.sin_family = AF_INET;
    std::memcpy(&server_address.sin_addr.s_addr, server->h_addr, server->h_length);
    server_address.sin_port = htons(options.port);

    // Connect to the server
    if (connect(this->socketfd, (struct sockaddr *)&this->server_address, sizeof(this->server_address)) < 0)
    {
        throw std::runtime_error("Can't connect to the server");
    }
}

TCPMessenger::~TCPMessenger(){
    close(this->socketfd);
}

void TCPMessenger::Authorize(Message &message){
    // Set up the AUTH message content
    std::string content = "AUTH " + message.username + " AS " + message.display_name + " USING " + message.password + "\r\n";

    // Chnage the local display name
    this->display_name = message.display_name;

    // Send the message to the server
    int send_bytes = send(this->socketfd, content.c_str(), content.length(), 0);

    if (send_bytes < 0)
    {
        throw std::runtime_error("ERROR sending AUTH message");
    }
}
void TCPMessenger::Bye(){
    // Set up the BYE message content
    std::string content = "BYE FROM " + this->display_name + "\r\n";

    // Send the message to the server
    int send_bytes = send(this->socketfd, content.c_str(), content.length(), 0);

    if (send_bytes < 0)
    {
        throw std::runtime_error("ERROR sending BYE message");
    }
}

void TCPMessenger::Err(Message &message){
    // Set up the ERR message content
    std::string content = "ERR FROM " + this->display_name + " IS " + message.contents + "\r\n";

    // Send the message to the server
    int send_bytes = send(this->socketfd, content.c_str(), content.length(), 0);

    if (send_bytes < 0)
    {
        throw std::runtime_error("ERROR sending ERR message");
    }
}

void TCPMessenger::Join(Message &message){
    // Set up the JOIN message content
    std::string content = "JOIN " + message.channel_id + " AS " + this->display_name + "\r\n";

    // Send the message to the server
    int send_bytes = send(this->socketfd, content.c_str(), content.length(), 0);

    if (send_bytes < 0)
    {
        throw std::runtime_error("ERROR sending JOIN message");
    }
}

void TCPMessenger::Msg(Message &message){
    // Set up the MSG message content
    std::string content = "MSG FROM " + this->display_name + " IS " + message.contents + "\r\n";

    // Send the message to the server
    int send_bytes = send(this->socketfd, content.c_str(), content.length(), 0);

    if (send_bytes < 0)
    {
        throw std::runtime_error("ERROR sending MSG message");
    }
}

std::deque<Message> TCPMessenger::PollMessage(){

    std::deque<Message> messages;

        // Chek if the user pressed Ctrl+C
    if (TCPMessenger::hadSigint){
        Message message;
        message.poll_type = USER;
        message.type = BYE;
        messages.push_back(message);
        return messages;
    } 

    struct pollfd fds[2];

    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;

    // Socket input
    fds[1].fd = this->socketfd;
    fds[1].events = POLLIN;

    // Poll for messages from the server and the user
    int ret = poll(fds, 2, 50);
    if (ret == -1){
        // Was poll failure caused by SIGINT?
        if (TCPMessenger::hadSigint){
            TCPMessenger::hadSigint = false;
            Message message;
            message.poll_type = USER;
            message.type = BYE;
            messages.push_back(message);
            return messages;
        }
        else {
            throw std::runtime_error("Polling for messages");
        }
    }

    // Proccess the command from the user
    if (fds[0].revents & POLLIN){
        Message usr_message = this->HandleUserCommand();

        messages.push_back(usr_message);    
    }

    // Proccess the received message from the server
    if (fds[1].revents & POLLIN){
        Message server_message = this->ReceiveFromServer();

        messages.push_back(server_message);
    }
    return messages;
}

Message TCPMessenger::HandleUserCommand(){
    Command command(CMD_HELP,{});

    // Parse the command from the user
    // If the command is not recognized, it will throw an exception
    try
    {
        command = InputParser::Parse();
    }
    catch(const std::invalid_argument& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
     
    Message message;
    message.poll_type = USER;

    switch (command.type)
    {
    case CMD_AUTH:
        message.type = AUTH;
        message.username = command.parameters[0];
        message.password = command.parameters[1];
        message.display_name = command.parameters[2];
        break;
    case CMD_JOIN:
        message.type = JOIN;
        message.channel_id = command.parameters[0];
        break;
    case CMD_RENAME:
        this->display_name = command.parameters[0];
        break;
    case CMD_HELP:
        for (const auto& param : command.parameters) {
            std::cerr << param << " ";
        }
        std::cerr << "Available commands:" << std::endl;
        std::cerr << "/auth <username> <password> <display_name>" << std::endl;
        std::cerr << "/join <channel_id>" << std::endl;
        std::cerr << "/rename <new_display_name>" << std::endl;
        std::cerr << "/help" << std::endl;
        break;
    case CMD_EXIT:
        message.type = BYE;
        break;
    case CMD_ERR:
        message.type = ERR;
        message.contents = command.parameters.at(0);
        break;
    default:
        message.type = MSG;
        message.contents = command.parameters.at(0);
        for(size_t i = 1; i < command.parameters.size(); i++){
            message.contents += " " + command.parameters.at(i);
        }

        // If the message is too long, truncate the message to 60000 bytes
        if(message.contents.size() > 60000){
            message.contents.resize(60000);
        }

        break;
    }
    return message;
}

Message TCPMessenger::ReceiveFromServer(){
    // Create a buffer to receive the message
    char buffer[2024];
    memset(buffer, 0, sizeof(buffer));

    Message msg;
    msg.poll_type = SERVER;

    // Receive the message from the server
    auto bytes_received = recv(this->socketfd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received < 0){
        return msg;
    }
    
    // Regexes for checking which message was received
    std::vector<std::tuple<MessageType_t,std::regex>> patterns = {
        {REPLY,std::regex(R"(REPLY (OK|NOK) IS ([ -~\n]{1,100})\r\n)")},
        {MSG,std::regex(R"(MSG FROM ([a-zA-Z0-9_-]{1,20}) IS ([ -~\n]{1,100})\r\n)")},
        {ERR,std::regex(R"(ERR FROM ([a-zA-Z0-9_-]{1,20}) IS ([ -~\n]{1,100})\r\n)")},
        {BYE,std::regex(R"(BYE FROM ([a-zA-Z0-9_-]{1,20})\r\n)")}
    };

    // Cycle through all regexes and check if the message matches
    for(const auto &[msg_type, regex] : patterns){    
        std::cmatch cmatches;
        if(std::regex_search(buffer, cmatches, regex)){
            msg.poll_type = SERVER;
            switch (msg_type)
            {
            case REPLY:
                if (cmatches[1].str() == "OK"){
                    msg.result = true;
                }
                else if (cmatches[1].str() == "NOK"){
                    msg.result = false;
                }
                else{
                    break;
                }                
                msg.type = REPLY;
                msg.contents = cmatches[2].str();

                // If the message is too long, truncate the message to 60000 bytes
                if(msg.contents.size() > 60000){
                    msg.contents.resize(60000);
                }

                return msg;
            case MSG:
                msg.type = MSG;
                msg.display_name = cmatches[1].str();
                msg.contents = cmatches[2].str();

                // If the message is too long, truncate the message to 60000 bytes
                if(msg.contents.size() > 60000){
                    msg.contents.resize(60000);
                }

                return msg;
            case ERR:
                msg.type = ERR;
                msg.display_name = cmatches[1].str();
                msg.contents = cmatches[2].str();
                
                // If the message is too long, truncate the message to 60000 bytes
                if(msg.contents.size() > 60000){
                    msg.contents.resize(60000);
                }

                return msg;
            case BYE:
                msg.type = BYE;
                msg.display_name = cmatches[1].str();
                return msg;
            default:
                break;
            }
        }
    }

    msg.type = UNKNOWN;
    msg.contents = std::string(buffer);

    // Remove the last two characters if they are "\r\n" to avoid printing double new line
    if (msg.contents.size() >= 2 && msg.contents.substr(msg.contents.size() - 2) == "\r\n") {
        msg.contents.erase(msg.contents.size() - 2);
    }
    return msg;
}
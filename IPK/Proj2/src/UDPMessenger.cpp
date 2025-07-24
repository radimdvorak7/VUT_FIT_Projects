/**
 * @file UDPMessenger.cpp
 * @author Radim Dvořák (xdvorar00)
 * @date 2025-04-20
 * @brief Implementation of UDP messenger
 */

#include "UDPMessenger.hpp"
#include "InputParser.hpp"
#include "Command.hpp"

#include <csignal>
#include <cstring>
#include <iostream>
#include <stdexcept>

#include <unistd.h>
#include <poll.h>
#include <netdb.h>

#define BEG_OFFSET (sizeof(MessageType_t) + sizeof(uint16_t))
#define NULL_BYTE_OFFSET sizeof(uint8_t)


bool UDPMessenger::hadSigint = false;

void UDPMessenger::HandleSigint(int signal){
    if (signal == SIGINT){
        UDPMessenger::hadSigint = true;
    }
}

UDPMessenger::UDPMessenger(Options &options) : Messenger::Messenger() {
    this->message_id = 0;
    this->max_retries = options.retries;

    signal(SIGINT,UDPMessenger::HandleSigint);

    // Create socket
    this->socketfd = socket(AF_INET,SOCK_DGRAM,0);
    if (this->socketfd < 0)
    {
        throw std::runtime_error("ERROR opening socket");
    }
    
    std::memset(&(this->server_address), 0, sizeof(this->server_address));

    // Set up server address
    struct hostent *server = gethostbyname(options.hostname.c_str());
    if (server == nullptr)
    {   
        close(this->socketfd);
        throw std::runtime_error("Coudn't resolve host");
    }

    struct timeval timeout;
    timeout.tv_sec = options.timeout / 1000;
    timeout.tv_usec = (options.timeout % 1000) * 1000;
    
    // Set up timeout on socket
    if (setsockopt(this->socketfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        close(this->socketfd);
        throw std::runtime_error("Failed to set timeout");
    }

    std::memcpy(&server_address.sin_addr.s_addr, server->h_addr, server->h_length);
    this->server_address.sin_family = AF_INET;
    this->server_address.sin_port = htons(options.port);

    // Bind the socket to the address and port
    bind(this->socketfd, (struct sockaddr *)&this->server_address, sizeof(this->server_address));

}

UDPMessenger::~UDPMessenger(){
    close(this->socketfd);
}
 
void UDPMessenger::Authorize(Message &message){
    // Calculate the length of the AUTH message
    size_t message_len = BEG_OFFSET + message.username.length() + NULL_BYTE_OFFSET + 
    message.display_name.length() + NULL_BYTE_OFFSET + message.password.length() + NULL_BYTE_OFFSET;

    this->display_name = message.display_name;
    std::vector<uint8_t> data(message_len);

    // Fill the message fields of type and ID
    data[0] = AUTH;
    data[1] = (this->message_id >> 8) & 0xFF;
    data[2] = this->message_id & 0xFF;

    // Fill the message fields of username, display name and password
    std::memcpy(&data[BEG_OFFSET], message.username.c_str(), message.username.length());
    std::memcpy(&data[BEG_OFFSET + message.username.length() + NULL_BYTE_OFFSET], this->display_name.c_str(), this->display_name.length());
    std::memcpy(&data[BEG_OFFSET + message.username.length() + NULL_BYTE_OFFSET + this->display_name.length() + NULL_BYTE_OFFSET], message.password.c_str(), message.password.length());

    // Port can be changed at the server side
    int send_bytes = sendto(this->socketfd, data.data(), message_len, 0, (struct sockaddr *)&this->server_address,sizeof(this->server_address));
    
    if (send_bytes < 0)
    {
        throw std::runtime_error("ERROR sending AUTH message");
    }
    this->last_message = message;
    this->message_id++;
}

void UDPMessenger::Bye(){
    // Calculate the length of the BYE message
    size_t message_len = BEG_OFFSET + this->display_name.length() + NULL_BYTE_OFFSET;

    std::vector<uint8_t> data(message_len);

    // Fill the message fields of type and ID
    data[0] = BYE;
    data[1] = (this->message_id >> 8) & 0xFF;
    data[2] = this->message_id & 0xFF;

    // Fill the message fields of display name
    std::memcpy(&data[BEG_OFFSET], this->display_name.c_str(), this->display_name.length());

    // Port can be changed at the server side
    int send_bytes = sendto(this->socketfd, data.data(), message_len, 0, (struct sockaddr *)&this->server_address,sizeof(this->server_address));
    
    if (send_bytes < 0)
    {
        throw std::runtime_error("ERROR sending BYE message");
    }
    this->message_id++;
}

void UDPMessenger::Confirm(uint16_t ref_message_id){
    size_t message_len = BEG_OFFSET;

    std::vector<uint8_t> data(message_len);

    // Fill the message fields of type and ID
    data[0] = CONFIRM;
    data[1] = (ref_message_id >> 8) & 0xFF;
    data[2] = ref_message_id & 0xFF;

    // Port can be changed at the server side
    int send_bytes = sendto(this->socketfd, data.data(), message_len, 0, (struct sockaddr *)&this->server_address,sizeof(this->server_address));
    
    if (send_bytes < 0)
    {
        throw std::runtime_error("ERROR sending CONFIRM message");
    }
}

void UDPMessenger::Err(Message &message){
    // Calculate the length of the ERR message
    size_t message_len = BEG_OFFSET + this->display_name.length() + NULL_BYTE_OFFSET + message.contents.length() + NULL_BYTE_OFFSET;

    std::vector<uint8_t> data(message_len);

    // Fill the message fields of type and ID
    data[0] = ERR;
    data[1] = (this->message_id >> 8) & 0xFF;
    data[2] = this->message_id & 0xFF;

    // Fill the message fields of display name and contents
    std::memcpy(&data[BEG_OFFSET], this->display_name.c_str(), this->display_name.length());
    std::memcpy(&data[BEG_OFFSET + this->display_name.length() + NULL_BYTE_OFFSET], message.contents.c_str(), message.contents.length());

    // Port can be changed at the server side
    int send_bytes = sendto(this->socketfd, data.data(), message_len, 0, (struct sockaddr *)&this->server_address,sizeof(this->server_address));
    
    if (send_bytes < 0)
    {
        throw std::runtime_error("ERROR sending ERR message");
    }
    this->last_message = message;
    this->message_id++;
}

void UDPMessenger::Join(Message &message){
    // Calculate the length of the JOIN message
    size_t message_len = BEG_OFFSET + message.channel_id.length() + NULL_BYTE_OFFSET + this->display_name.length() + NULL_BYTE_OFFSET;

    std::vector<uint8_t> data(message_len);

    // Fill the message fields of type and ID
    data[0] = JOIN;
    data[1] = (this->message_id >> 8) & 0xFF;
    data[2] = this->message_id & 0xFF;
    
    // Fill the message fields of channel ID and display name
    std::memcpy(&data[BEG_OFFSET], message.channel_id.c_str(), message.channel_id.length());
    std::memcpy(&data[BEG_OFFSET + message.channel_id.length() + NULL_BYTE_OFFSET], this->display_name.c_str(), this->display_name.length());

    // Port can be changed at the server side
    int send_bytes = sendto(this->socketfd, data.data(), message_len, 0, (struct sockaddr *)&this->server_address,sizeof(this->server_address));
    
    if (send_bytes < 0)
    {
        throw std::runtime_error("ERROR sending JOIN message");
    }

    this->last_message = message;
    this->message_id++;
}

void UDPMessenger::Msg(Message &message){
    // Calculate the length of the MSG message
    size_t message_len = BEG_OFFSET + this->display_name.length() + NULL_BYTE_OFFSET + message.contents.length() + NULL_BYTE_OFFSET;

    std::vector<uint8_t> data(message_len);

    // Fill the message fields of type and ID
    data[0] = MSG;
    data[1] = (this->message_id >> 8) & 0xFF;
    data[2] = this->message_id & 0xFF;
    
    // Fill the message fields of display name and contents
    std::memcpy(&data[BEG_OFFSET], this->display_name.c_str(),this->display_name.length());
    std::memcpy(&data[BEG_OFFSET + this->display_name.length() + NULL_BYTE_OFFSET], message.contents.c_str(), message.contents.length());

    // Port can be changed at the server side
    int send_bytes = sendto(this->socketfd, data.data(), message_len, 0, (struct sockaddr *)&this->server_address,sizeof(this->server_address));
    
    if (send_bytes < 0)
    {
        throw std::runtime_error("ERROR sending MSG message");
    }

    this->last_message = message;
    this->message_id++;
}

std::deque<Message> UDPMessenger::PollMessage(){
    std::deque<Message> messages;
    
    // Chek if the user pressed Ctrl+C
    if (UDPMessenger::hadSigint){
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
        if (UDPMessenger::hadSigint){
            UDPMessenger::hadSigint = false;
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
        // If the message is a confirmation, we don't need to add it to the queue
        if (server_message.type != CONFIRM){
            // Send confirm on all types of messages
            this->Confirm(server_message.ID);
            messages.push_back(server_message);
        }
    }
    return messages;
}

Message UDPMessenger::HandleUserCommand(){
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

Message UDPMessenger::ReceiveFromServer(){
    // Create a buffer to receive the message
    auto buffer = new std::vector<uint8_t>(2024);
    std::fill(buffer->begin(), buffer->end(), 0);

    Message msg;
    msg.poll_type = SERVER;

    // For potential change of the port
    sockaddr new_adress;
    socklen_t new_address_len = sizeof(new_adress);

    // Receive the message from the server
    auto bytes_received = recvfrom(this->socketfd, buffer->data(), buffer->size(), 0, &new_adress, &new_address_len);

    if (bytes_received < 0){
        delete buffer;
        return msg;
    }

    msg.type = static_cast<MessageType_t>(buffer->at(0));

    std::string displayName {};
    std::string contents {};
    size_t idx = 0;

    switch (msg.type)
    {
    case CONFIRM:
        msg.type = CONFIRM;
        msg.ref_ID = buffer->at(1) << 8 | buffer->at(2);
        delete buffer;
        return msg;
    case REPLY:
        msg.type = REPLY;
        msg.ID = buffer->at(1) << 8 | buffer->at(2);
        msg.result = buffer->at(3);
        msg.ref_ID = buffer->at(4) << 8 | buffer->at(5);

        idx = 6;
        // Read the contents of the message
        while (buffer->at(idx) != '\0') {
            contents += static_cast<char>(buffer->at(idx));
            idx++;
        }
        msg.contents = contents;

        // Port can be changed at the server side
        this->server_address.sin_port = ((sockaddr_in *)(&new_adress))->sin_port;
        
        delete buffer;
        return msg;
    case MSG:
        msg.type = MSG;
        msg.ID = buffer->at(1) << 8 | buffer->at(2);

        idx = BEG_OFFSET;

        // Read the display name of the message
        while (buffer->at(idx) != '\0') {
            displayName += static_cast<char>(buffer->at(idx));
            idx++;
        }
        msg.display_name = displayName;
        idx += NULL_BYTE_OFFSET;

        // Read the contents of the message
        while (buffer->at(idx) != '\0') {
            contents += static_cast<char>(buffer->at(idx));
            idx++;
        }
        msg.contents = contents;

        delete buffer;
        return msg;
    case PING:
        msg.type = PING;
        msg.ID = buffer->at(1) << 8 | buffer->at(2);
        delete buffer;
        return msg;
    case ERR:
        msg.type = ERR;
        msg.ID = buffer->at(1) << 8 | buffer->at(2);

        idx = BEG_OFFSET;

        display_name = "";
        // Read the display name of the message
        while (buffer->at(idx) != '\0') {
            display_name += static_cast<char>(buffer->at(idx));
            idx++;
        }
        msg.display_name = display_name;
        idx += NULL_BYTE_OFFSET;

        // Read the contents of the message
        while (buffer->at(idx) != '\0') {
            contents += static_cast<char>(buffer->at(idx));
            idx++;
        }
        msg.contents = contents;

        delete buffer;
        return msg;
    case BYE:
        msg.type = BYE;
        msg.ID = buffer->at(1) << 8 | buffer->at(2);
        delete buffer;
        return msg;
    default:
        msg.type = UNKNOWN;
        msg.ID = buffer->at(1) << 8 | buffer->at(2);
        delete buffer;
        return msg;
    }
    return msg;
}
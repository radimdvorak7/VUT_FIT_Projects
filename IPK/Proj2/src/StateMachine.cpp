/**
 * @file StateMachine.cpp
 * @author Radim Dvořák (xdvorar00)
 * @date 2025-04-20
 * @brief Implementation of finite state machine controlling behaviour of client
 */

#include "StateMachine.hpp"
#include "Options.hpp"
#include "TCPMessenger.hpp"
#include "UDPMessenger.hpp"

#include <iostream>

StateMachine::StateMachine(Options &options){
    this->options = options;
    this->state = S_START;
    try{
        switch (this->options.protocol)
        {
        case UDP:
            this->messenger = std::make_unique<UDPMessenger>(this->options);
            break;
        case TCP:
            this->messenger = std::make_unique<TCPMessenger>(this->options);
            break;
        default:
            // This should never happen
            break;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        exit(1);
    }
    
}

void StateMachine::SetState(State_t new_state){
    this->state = new_state;
}

void StateMachine::StateStart(){
    // Poll for messages from the server or user
    std::deque<Message> results = this->messenger.get()->PollMessage();

    // Cycle through all messages, but they are handled in the order they were received
    for (auto &rst : results){
        switch (rst.poll_type)
        {   
        case USER:
            switch (rst.type)
            {
            case AUTH:
                this->SetState(S_AUTH);
                this->messenger.get()->Authorize(rst);
                break;
            case BYE:
                this->SetState(S_END);
                this->messenger.get()->Bye();
                break;
            default:
                std::cout << "ERROR: " << rst.contents << std::endl;
                break;
            }
            break;
        case SERVER:
            switch (rst.type)
            {
            case BYE:
                this->SetState(S_END);
                std::cout << "BYE FROM " << rst.display_name << std::endl;
                break;
            case ERR:
                this->SetState(S_END);
                std::cout << "ERROR FROM " << rst.display_name << ": " << rst.contents << std::endl;
                break;
            default:
                this->SetState(S_ERROR);
                std::cout << "ERROR: " << rst.contents << std::endl;
                this->messenger.get()->Err(rst);
                break;
            }
            break;
        default:
            break;
        }
    }
}

void StateMachine::StateAuth(){
    // Poll for messages from the server or user
    std::deque<Message> results = this->messenger.get()->PollMessage();

    // Cycle through all messages, but they are handled in the order they were received
    for (auto &rst : results){
        switch (rst.poll_type)
        {   
        case USER:
            switch (rst.type)
            {
            case AUTH:
                this->messenger.get()->Authorize(rst);
                break;
            case BYE:
                this->SetState(S_END);
                this->messenger.get()->Bye();
                break;
            default:
                std::cout << "ERROR: " << rst.contents << std::endl;
                break;
            }
            break;
        case SERVER:
            switch (rst.type)
            {
            case REPLY:
                if(rst.result){
                    this->SetState(S_OPEN);
                    std::cout << "Action Success: " << rst.contents << std::endl;
                }
                else{
                    std::cout << "Action Failure: " << rst.contents << std::endl;
                }
                break;
            case BYE:
                this->SetState(S_END);
                std::cout << "BYE FROM " << rst.display_name << std::endl;
                break;
            case ERR:
                this->SetState(S_END);
                std::cout << "ERROR FROM " << rst.display_name << ": " << rst.contents << std::endl;
                break;
            case MSG:
                this->SetState(S_END);
                break;
            default:
                this->SetState(S_ERROR);
                std::cout << "ERROR: " << rst.contents << std::endl;
                this->messenger.get()->Err(rst);
                break;
            }
        default:
            break;
        }
    }
}

void StateMachine::StateOpen(){
    // Poll for messages from the server or user
    std::deque<Message> results = this->messenger.get()->PollMessage();

    // Cycle through all messages, but they are handled in the order they were received
    for (auto &rst : results){
        switch (rst.poll_type)
        {   
        case USER:
            switch (rst.type)
            {
            case BYE:
                this->SetState(S_END);
                this->messenger.get()->Bye();
                break;
            case JOIN:
                this->SetState(S_JOIN);
                this->messenger.get()->Join(rst);
                break;
            case MSG:
                this->messenger.get()->Msg(rst);
                break;
            default:
                std::cout << "ERROR: " << rst.contents << std::endl;
                break;
            }
            break;
        case SERVER:
            switch (rst.type)
            {
            case BYE:
                this->SetState(S_END);
                std::cout << "BYE FROM " << rst.display_name << std::endl;
                break;
            case REPLY:
                this->SetState(S_ERROR);
                break;
            case MSG:
                std::cout << rst.display_name << ": " << rst.contents << std::endl;
                break;
            case ERR:
                this->SetState(S_END);
                std::cout << "ERROR FROM " << rst.display_name << ": " << rst.contents << std::endl;
                break;
            default:
                this->SetState(S_ERROR);
                std::cout << "ERROR: " << rst.contents << std::endl;
                this->messenger.get()->Err(rst);
                break;
            }
        default:
            break;
        }
    }
}

void StateMachine::StateJoin(){
    // Poll for messages from the server or user
    std::deque<Message> results = this->messenger.get()->PollMessage();

    // Cycle through all messages, but they are handled in the order they were received
    for (auto &rst : results){
        switch (rst.poll_type)
        {   
        case USER:
            switch (rst.type)
            {
            case BYE:
                this->SetState(S_END);
                this->messenger.get()->Bye();
                break;
            default:
                std::cout << "ERROR: " << rst.contents << std::endl;
                break;
            }
            break;
        case SERVER:
            switch (rst.type)
            {
            case BYE:
                this->SetState(S_END);
                std::cout << "BYE FROM " << rst.display_name << std::endl;
                break;
            case REPLY:
                this->SetState(S_OPEN);
                if(rst.result){
                    std::cout << "Action Success: " << rst.contents << std::endl;
                }
                else{
                    std::cout << "Action Failure: " << rst.contents << std::endl;
                }
                break;
            case MSG:
                std::cout << rst.display_name << ": " << rst.contents << std::endl;
                break;
            case ERR:
                this->SetState(S_END);
                std::cout << "ERROR FROM " << rst.display_name << ": "<< rst.contents << std::endl;
                break;
            default:
                this->SetState(S_ERROR);
                std::cout << "ERROR: " << rst.contents << std::endl;
                this->messenger.get()->Err(rst);
                break;
            }
        default:
            break;
        }
    }
}

void StateMachine::StateError(){
    return;
}
void StateMachine::StateEnd(){
    return;
}

int StateMachine::Run(){
    try{
        while (1){
            switch (this->state)
            {
            case S_START:
                this->StateStart();
                break;
            case S_AUTH:
                this->StateAuth();
                break;
            case S_OPEN:
                this->StateOpen();
                break;
            case S_JOIN:
                this->StateJoin();
                break;
            case S_END:
                this->StateEnd();
                return EXIT_SUCCESS;
            case S_ERROR:
            default:
                this->StateError();
                return EXIT_FAILURE;
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "ERROR: " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    
}
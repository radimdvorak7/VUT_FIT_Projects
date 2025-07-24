/**
 * @file StateMachine.hpp
 * @author Radim Dvořák (xdvorar00)
 * @date 2025-04-20
 * @brief Declaration of class StateMachine, finite state machine controlling behaviour of client
 */

#ifndef STATE_MACHINE_HPP
#define STATE_MACHINE_HPP

#include "Options.hpp"
#include "Messenger.hpp"

#include <memory>

typedef enum {
    S_START,        // Initial state
    S_AUTH,         // State for waiting to confirm authentication
    S_OPEN,         // State for for communication with server
    S_JOIN,         // State for waiting to confirm joining a channel
    S_ERROR,        // Error state 
    S_END           // Final state
} State_t;

/**
 * @class StateMachine
 * @brief A class representing a finite state machine for the client.
 */
class StateMachine{
public:
    StateMachine(Options &options);

    /**
     * @brief Runs the state machine.
     *
     * @return int The exit status of the program.
     */
    int Run();
private:
    State_t state;
    Options options;
    std::unique_ptr<Messenger> messenger;

    /**
     * @brief Represents the starting state of the state machine.
     */
    void StateStart();
    /**
     * @brief Represents the authentication state of the state machine.
     */
    void StateAuth();
    /**
     * @brief Represents the open state of the state machine.
     */
    void StateOpen();
    /**
     * @brief Represents the joining state of the state machine.
     */
    void StateJoin();
    /**
     * @brief Represents the error state of the state machine.
     */
    void StateError();
    /**
     * @brief Represents the end state of the state machine.
     */
    void StateEnd();
    /**
     * @brief Sets the current state of the state machine.
     *
     * @param new_state The new state to set.
     */
    void SetState(State_t new_state);
};

#endif
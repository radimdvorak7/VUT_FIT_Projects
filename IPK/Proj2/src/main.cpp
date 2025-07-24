/**
 * @file main.cpp
 * @author Radim Dvořák (xdvorar00)
 * @date 2025-04-17
 * @brief Main function of chat client
 */

#include "ArgParser.hpp"
#include "StateMachine.hpp"

#include <iostream>

int main(int argc, char *argv[]){

    Options Options;

    try
    {   
        ArgParser Parser;
        Options = Parser.Parse(argc,argv);
    }
    catch(const std::invalid_argument& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        ArgParser::Help();
        exit(1);
    }

    StateMachine FSM(Options);
    FSM.Run();

    return 0;
}
/**
 * @file main.cpp
 * @brief Header file for the Transition class, representing a transition in a Finite State Machine (FSM).
 *
 * This file defines the main entry point for the FSM editor application. It initializes the
 * QApplication, creates the main window, and starts the event loop.
 *
 * @author Tomáš Bordák [xborda01]
 * @author Erik Roják [xrojak00]
 * @author Radim Dvořák [xdvorar00]
 */

#include "main_window.h"
#include <QApplication>


int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}
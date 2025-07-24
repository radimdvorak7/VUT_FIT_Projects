/**
 * @file main_window.h
 * @brief Header file for the MainWindow class, providing the main GUI for a Finite State Machine (FSM) editor.
 *
 * This file declares the MainWindow class, which serves as the primary interface for a graphical
 * FSM editor. It manages user interactions for creating, editing, and running FSMs, including
 * state and transition manipulation, FSM loading/saving, and communication with an external FSM process.
 *
 * @author Tomáš Bordák [xborda01]
 * @author Erik Roják [xrojak00]
 * @author Radim Dvořák [xdvorar00]
 */

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "automaton_model.h"
#include "transition.h"
#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>
#include <QMap>
#include <QUdpSocket>
#include <QProcess>
#include <QTableWidget>

namespace Ui {
class MainWindow;
}

class UdpMonitor;
class State;
class Transition;

/**
 * @class MainWindow
 * @brief The main GUI window for a Finite State Machine (FSM) editor.
 *
 * The MainWindow class provides the primary user interface for creating, editing, and running
 * FSMs in a graphical environment. It integrates with the AutomatonModel to manage states and
 * transitions, handles user interactions with Qt-based UI. The class supports actions like loading/saving FSMs, adding/deleting
 * states and transitions, and updating runtime inputs/outputs.
 *
 * @see AutomatonModel
 * @see State
 * @see Transition
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    /**
     * @brief Constructs the MainWindow.
     *
     * Initializes the main window with a UI for the FSM editor.
     *
     * @param parent The parent widget.
     */
    explicit MainWindow(QWidget* parent = nullptr);

    /**
     * @brief Destroys the MainWindow.
     *
     * Cleans up resources, including the UI and FSM process.
     */
    ~MainWindow();

    /**
     * @brief Handles a state being clicked by the user.
     *
     * Updates the UI and model state based on the clicked state for starting a transition.
     *
     * @param state Pointer to the clicked State.
     */
    void onStateClicked(State* state);

    /**
     * @brief Loads an FSM from a file.
     *
     * Reads an FSM configuration and updates the graphical scene and model.
     */
    void loadFsm();

    /**
     * @brief Saves the current FSM to a file.
     *
     * Writes the FSM configuration to a file.
     */
    void saveFsm();

private:
    /**
     * @brief Sets up the graphical scene and displays the FSM.
     *
     * Configures the QGraphicsScene for rendering states and transitions.
     *
     * @param scene Pointer to the QGraphicsScene to set up.
     */
    void setSettingsAndDisplayFsm(QGraphicsScene* scene);

    /**
     * @brief Logs a message to the UI.
     *
     * Adds a message to the log display for user feedback.
     *
     * @param message The QString message to log.
     */
    void newLog(const QString& message);

    /**
     * @brief Processes messages received from the FSM process.
     *
     * Handles communication from the external FSM process via UDP.
     *
     * @param message The QString message received.
     */
    void receiveFromFsm(const QString& message);

    /**
     * @brief Updates a value in a table widget.
     *
     * Updates the specified table with a name-value pair for inputs or outputs.
     *
     * @param name The QString name of the value.
     * @param value The QString value to display.
     * @param table Pointer to the QTableWidget to update.
     */
    void updateValue(const QString& name, const QString& value, QTableWidget* table);

private slots:
    /**
     * @brief Slot triggered to start the FSM process.
     */
    void on_start_act_triggered();

    /**
     * @brief Slot triggered to add a new state.
     */
    void on_add_state_clicked();

    /**
     * @brief Slot triggered to load an FSM from a file.
     */
    void on_load_act_triggered();

    /**
     * @brief Slot triggered to save the current FSM to a file.
     */
    void on_save_act_triggered();

    /**
     * @brief Slot triggered to create a new FSM.
     */
    void on_create_act_triggered();

    /**
     * @brief Slot triggered to save changes in the state editing menu.
     */
    void on_save_state_edit_clicked();

    /**
     * @brief Slot triggered to return from the state editing menu.
     */
    void on_back_state_edit_clicked();

    /**
     * @brief Slot triggered to delete the selected state.
     */
    void on_delete_state_clicked();

    /**
     * @brief Slot triggered to set the selected state as the start state.
     */
    void on_set_start_state_clicked();

    /**
     * @brief Slot triggered to start creating a new transition.
     */
    void on_add_transition_clicked();

    /**
     * @brief Slot triggered to return from the transition editing menu.
     */
    void on_back_transition_edit_clicked();

    /**
     * @brief Slot triggered to save changes in the transition editing menu.
     */
    void on_save_transition_edit_clicked();

    /**
     * @brief Slot triggered to delete the selected transition.
     */
    void on_delete_transition_clicked();

    /**
     * @brief Slot triggered to save FSM settings.
     */
    void on_save_settings_clicked();

    /**
     * @brief Slot triggered to terminate the FSM process.
     */
    void on_terminateButton_clicked();

    /**
     * @brief Slot triggered to update FSM input values.
     */
    void on_updateInputButton_clicked();

private:
    /// Pointer to the Qt UI for the main window.
    Ui::MainWindow* ui;
    /// Pointer to the AutomatonModel managing the FSM.
    AutomatonModel* automatonModel_;
    /// Flag indicating if a transition creation is pending.
    bool isTransitionPending_;
    /// Pointer to the source state for a pending transition.
    State* stateFrom_;
    /// Pointer to the external FSM process.
    QProcess* fsmProcess_;
};

#endif
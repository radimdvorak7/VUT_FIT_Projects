/**
 * @file automaton_model.h
 * @brief The AutomatonModel class represents the finite state machine (FSM) model.
 * 
 * This file declares the AutomatonModel class, which contains all the states,
 * transitions, and settings of the FSM. It provides methods to add, delete,
 * and manipulate states and transitions.
 * 
 * @date 2023-05-11
 * 
 * @author Tomáš Bordák [xborda01]
 * @author Radim Dvořák [xdvorar00]
 * @author Erik Roják [xrojak00]
 */

#ifndef AUTOMATON_MODEL_H
#define AUTOMATON_MODEL_H

#include "state.h"
#include "transition.h"
#include <QObject>
#include <QList>
#include <QDebug>
#include <QStackedWidget>
#include <QFile>
#include <QDir>

class State;
class Transition;
class MainWindow;

/**
 * @class AutomatonModel
 * 
 * @brief The AutomatonModel class represents the finite state machine (FSM) model.
 * It contains all the states, transitions, and settings of the FSM.
 */
class AutomatonModel : public QObject
{

public:

    /**
     * @brief AutomatonModel constructor
     * 
     * @param leftMenu QStackedWidget pointer to the left menu
     * @param mainWindow MainWindow pointer to the main window
     */
    AutomatonModel(QStackedWidget* leftMenu, MainWindow* mainWindow);

    /**
     * @brief Save the FSM into the file
     * 
     * @return QString with all the data of the FSM
     */
    QString saveData() const;

    /**
     * @brief Load the FSM from the file
     * 
     * @param data QString with all the data of the FSM
     * @return true if loaded successfully, false otherwise
     */
    bool loadData(QString* data);

    /**
     * @brief Reset the FSM model
     * 
     * @return AutomatonModel* pointer to the new AutomatonModel
     */
    AutomatonModel* reset();

    /**
     * @brief Generate the FSM code
     * 
     * @return path to the generated code
     */
    QString generateCode();

    
    /**
     * @brief Add a state to the FSM
     * 
     * @param state pointer to the state to be added
     */
    void addState(State* state);

    /**
     * @brief Delete a state from the FSM
     * 
     * @param state pointer to the state to be deleted
     */
    void deleteState(State* state);

    /**
     * @brief Get the list of states in the FSM
     * 
     * @return list of states
     */
    const QList<State*>& states() const;

    /**
     * @brief Set the selected state
     * 
     * @param state state to be set as selected
     */
    void setSelectedState(State* state);

    /**
     * @brief Get the selected state
     * 
     * @return pointer to the selected state
     */
    State* selectedState() const;

    /**
     * @brief Set the start state
     * 
     * @param state state to be set as start
     */
    void setStartState(State* state);
    /**
     * @brief Get the start state
     * 
     * @return pointer to the start state
     */
    State* startState();

    /**
     * @brief If the selected state is not nullptr, update it and set it to nullptr
     */
    void updateAndSetNullptrSelectedState();

    /**
     * @brief Add a transition to the FSM
     * 
     * @param tr transition to be added
     */
    void addTransition(Transition* tr);

    /**
     * @brief Delete a transition from the FSM
     * 
     * @param tr transition to be deleted
     */
    void deleteTransition(Transition* tr);

    /**
     * @brief Get the list of transitions in the FSM
     * 
     * @return list of transitions
     */
    const QList<Transition*>& transitions() const;

    /**
     * @brief Set the selected transition
     * 
     * @param tr transition to be set as selected
     */
    void setSelectedTransition(Transition* tr);

    /**
     * @brief Get the selected transition
     * 
     * @return pointer to the selected transition
     */
    Transition* selectedTransiton() const;

    /**
     * @brief If the selected transition is not nullptr, update it and set it to nullptr
     */
    void updateAndSetNullptrSelectedTransition();

    /**
     * @brief Set the name of the FSM
     * 
     * @param name name of the FSM
     */
    void setName(QString& name);

    /**
     * @brief Get the name of the FSM
     * 
     * @return name of the FSM
     */
    QString name() const;

    /**
     * @brief Set the comment of the FSM
     * 
     * @param comment comment of the FSM
     */
    void setComment(QString& comment);

    /**
     * @brief Get the comment of the FSM
     * 
     * @return comment of the FSM
     */
    QString comment() const;

    /**
     * @brief Get the left menu
     * 
     * @return pointer to the left menu
     */
    void addInput(QString& input);

    /**
     * @brief Get the list of inputs
     * 
     * @return list of inputs
     */
    QStringList inputs() const;

    /**
     * @brief Reset the inputs
     */
    void resetInputs();

    /**
     * @brief Get the list of outputs
     * 
     * @return list of outputs
     */
    void addOutput(QString& output);

    /**
     * @brief Get the list of outputs
     * 
     * @return list of outputs
     */
    QStringList outputs() const;

    /**
     * @brief Reset the outputs
     */
    void resetOutputs();

    /**
     * @brief Get the list of variables
     * 
     * @return list of variables
     */
    void addVariable(QString& variable);

    /**
     * @brief Get the list of variables
     * 
     * @return list of variables
     */
    QStringList variables() const;

    /**
     * @brief Reset the variables
     */
    void resetVariables();

    /**
     * @brief Set the current state
     * 
     * @param stateName name of the state to be set as current
     */
    void setCurrentState(const QString& stateName);

    /**
     * @brief Get the current state
     * 
     * @return pointer to the current state
     */
    State* currentState();

private:
    /// QStackedWidget pointer to the left menu
    QStackedWidget* leftMenu_;
    /// MainWindow pointer to the main window
    MainWindow* mainWindow_;

    /// List of states
    QList<State*> states_;
    /// Selected state
    State* selectedState_ = nullptr;
    /// Start state
    State* startState_ = nullptr;

    /// Selected transition
    Transition* selectedTransition_ = nullptr;
    /// List of transitions
    QList<Transition*> transitions_;

    /// Name of the FSM
    QString name_;
    /// Comment of the FSM
    QString comment_;
    /// List of inputs
    QStringList inputs_;
    /// List of outputs
    QStringList outputs_;
    /// List of variables
    QStringList variables_;

    /// Current state
    State* currentState_ = nullptr;
};

#endif // AUTOMATON_MODEL_H

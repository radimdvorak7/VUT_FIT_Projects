/**
 * @file transition.h
 * @brief Header file for the Transition class, representing a transition in a Finite State Machine (FSM).
 *
 * This file declares the Transition class, which models a transition between two states
 * in a FSM editor. The class supports interactive editing of conditions and custom
 * rendering with an arrowhead to indicate direction.
 *
 * @author Tomáš Bordák [xborda01]
 * @author Erik Roják [xrojak00]
 * @author Radim Dvořák [xdvorar00]
 */


#ifndef TRANSITION_H
#define TRANSITION_H


#include <QGraphicsLineItem>
#include <QPen>
#include "state.h"
#include <QStackedWidget>


class State;
class AutomatonModel;

/**
 * @class Transition
 * @brief Represents a transition between two states in a Finite State Machine (FSM).
 *
 * The Transition class is a QGraphicsLineItem that connects two State objects in a graphical
 * FSM editor. It stores a condition for the transition, supports double-click editing via a UI
 * menu, and renders an arrowhead to indicate direction. The class interacts with the
 * AutomatonModel to manage FSM state and the QStackedWidget for editing.
 *
 * @see State
 * @see AutomatonModel
 */
class Transition : public QGraphicsLineItem {
public:
    /**
     * @brief Constructs a Transition between two states.
     *
     * Initializes the transition as a line from the source state to the target state, with
     * references to the AutomatonModel and QStackedWidget for FSM management and UI interaction.
     *
     * @param from Pointer to the source State.
     * @param to Pointer to the target State.
     * @param automatonModel Pointer to the AutomatonModel managing the FSM.
     * @param leftMenu Pointer to the QStackedWidget for the editing menu.
     */
    Transition(State* from, State* to, AutomatonModel* automatonModel, QStackedWidget* leftMenu);

    /**
     * @brief Sets the transitions condition.
     *
     * @param condition The QString representing the condition for the transition.
     */
    void setCondition(const QString& condition);

    /**
     * @brief Gets the transitions condition.
     *
     * @return The QString representing the condition.
     */
    QString condition();

    /**
     * @brief Gets the source state of the transition.
     *
     * @return Pointer to the source State.
     */
    State* fromState();

    /**
     * @brief Gets the target state of the transition.
     *
     * @return Pointer to the target State.
     */
    State* toState();

protected:
    /**
     * @brief Handles double-click events to open the transition editing menu.
     *
     * Opens the transition editing menu and populates it with the transitions condition.
     *
     * @param event The QGraphicsSceneMouseEvent triggered by the double-click.
     */
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

    /**
     * @brief Custom painting for the transitions appearance.
     *
     * Draws the transition as a line with an arrowhead to indicate direction.
     *
     * @param painter The QPainter used for rendering.
     * @param option Style options for the item.
     * @param widget The widget being painted on (optional).
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    /// Pointer to the source State.
    State* from_;
    /// Pointer to the target State.
    State* to_;
    /// Condition for the transition.
    QString condition_;
    /// Pointer to the AutomatonModel managing the FSM.
    AutomatonModel* automatonModel_;
    /// Pointer to the QStackedWidget for the editing menu.
    QStackedWidget* leftMenu_;
    /// Pointer to the QGraphicsPolygonItem for the arrowhead.
    QGraphicsPolygonItem* arrowHead_;
};

#endif // TRANSITION_H
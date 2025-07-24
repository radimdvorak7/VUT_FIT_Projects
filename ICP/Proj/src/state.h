/**
 * @file state.h
 * @brief Header file for the State class, representing a state in a Finite State Machine (FSM).
 *
 * This file declares the State class, which is used to model states in a graphical FSM editor.
 * The class supports interactive features like dragging, selection, and editing via a UI menu,
 * and manages transitions and state properties.
 *
 * @author Tomáš Bordák [xborda01]
 * @author Erik Roják [xrojak00]
 * @author Radim Dvořák [xdvorar00]
 */

// 1. Header Guard
#ifndef STATE_H
#define STATE_H

// 2. Includes
#include "automaton_model.h"
#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QStackedWidget>
#include <QString>
#include <QLineEdit>

// 4. Forward Declarations
class AutomatonModel;
class MainWindow;
class Transition;

/**
 * @class State
 * @brief Represents a state in a Finite State Machine (FSM) visualized in a graphical scene.
 *
 * The State class is a QGraphicsEllipseItem that represents a state in an FSM. It supports
 * dragging, selection, and double-click editing via a UI menu. Each state has a unique name,
 * an associated action, and manages incoming and outgoing transitions. The state’s appearance
 * changes when selected or active in the FSM runtime.
 *
 * @see AutomatonModel
 * @see MainWindow
 * @see Transition
 */

class State : public QGraphicsEllipseItem
{
public:
    /// Static counter for generating unique state names.
    static int counter;

    /**
     * @brief Constructs a State with specified position, size, and associated UI components.
     *
     * Initializes the state as a movable and selectable ellipse with a unique name based on a static counter.
     *
     * @param x X-coordinate of the state’s position in the scene.
     * @param y Y-coordinate of the state’s position in the scene.
     * @param w Width of the state’s ellipse.
     * @param h Height of the state’s ellipse.
     * @param leftMenu Pointer to the QStackedWidget for the editing menu.
     * @param automatonModel Pointer to the AutomatonModel managing the FSM.
     * @param mainWindow Pointer to the MainWindow for UI interactions.
     */
    State(qreal x, qreal y, qreal w, qreal h,
          QStackedWidget* leftMenu, AutomatonModel* automatonModel, MainWindow* mainWindow);

    /**
     * @brief Gets the state’s name.
     *
     * @return The QString representing the state’s unique name.
     */
    QString name() const;

    /**
     * @brief Sets the state’s name.
     *
     * @param name The new name for the state.
     */
    void setName(const QString& name);

    /**
     * @brief Gets the state’s action.
     *
     * @return The QString representing the action associated with the state.
     */
    QString action() const;

    /**
     * @brief Sets the state’s action.
     *
     * @param action The new action for the state.
     */
    void setAction(const QString& action);

    /**
     * @brief Adds an outgoing transition from this state.
     *
     * @param tr Pointer to the Transition to add.
     */
    void addOutgoingTransition(Transition* tr);

    /**
     * @brief Removes an outgoing transition from this state.
     *
     * @param tr Pointer to the Transition to remove.
     */
    void removeOutgoingTransition(Transition* tr);

    /**
     * @brief Adds an incoming transition to this state.
     *
     * @param tr Pointer to the Transition to add.
     */
    void addIncomingTransition(Transition* tr);

    /**
     * @brief Removes an incoming transition from this state.
     *
     * @param tr Pointer to the Transition to remove.
     */
    void removeIncomingTransition(Transition* tr);

    /**
     * @brief Checks if the state has any transitions.
     *
     * @return True if the state has incoming or outgoing transitions, false otherwise.
     */
    bool hasTransitions();

    /**
     * @brief Gets the X-coordinate of the state’s position.
     *
     * @return The qreal X-coordinate.
     */
    qreal X() const;

    /**
     * @brief Gets the Y-coordinate of the state’s position.
     *
     * @return The qreal Y-coordinate.
     */
    qreal Y() const;

    /**
     * @brief Handles changes to the state’s position during dragging.
     *
     * Restricts the state’s movement within the scene boundaries and updates connected transitions.
     *
     * @param change The type of change (e.g., ItemPositionChange).
     * @param value The new value associated with the change (e.g., new position).
     * @return The adjusted position or the original value.
     */
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    /**
     * @brief Custom painting for the state’s appearance.
     *
     * Draws the state as an ellipse with a dynamic border color and displays the state’s name.
     *
     * @param painter The QPainter used for rendering.
     * @param option Style options for the item.
     * @param widget The widget being painted on defaults with nullptr.
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

protected:
    /**
     * @brief Handles double-click events to open the state editing menu.
     *
     * Opens the State Edit menu and populates it with the state’s name and action.
     *
     * @param event The QGraphicsSceneMouseEvent triggered by the double-click.
     */
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

    /**
     * @brief Handles mouse press events to notify the MainWindow.
     *
     * Notifies the MainWindow when the state is clicked, enabling further interactions.
     *
     * @param event The QGraphicsSceneMouseEvent triggered by the mouse press.
     */
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    /// Pointer to the QStackedWidget for the editing menu.
    QStackedWidget* leftMenu_;
    /// Pointer to the AutomatonModel managing the FSM.
    AutomatonModel* automatonModel_;
    /// Pointer to the MainWindow for UI interactions.
    MainWindow* mainWindow_;
    /// X-coordinate of the state’s position.
    qreal x_;
    /// Y-coordinate of the state’s position.
    qreal y_;
    /// Unique name of the state.
    QString name_;
    /// Action associated with the state (defaults to empty string).
    QString action_;
    /// List of outgoing transitions from this state.
    QList<Transition*> outgoing_;
    /// List of incoming transitions to this state.
    QList<Transition*> incoming_;
};

// 10. Closing Header Guard
#endif // STATE_H
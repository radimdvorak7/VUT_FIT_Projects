/**
 * @file state.cpp
 * @brief Header file for the State class, representing a state in a Finite State Machine (FSM).
 *
 * This file defines implementation for the State class, which is used to model states in a graphical FSM editor.
 * The class supports interactive features like dragging, selection, and editing via a UI menu,
 * and manages transitions and state properties.
 *
 * @author Tomáš Bordák [xborda01]
 * @author Erik Roják [xrojak00]
 * @author Radim Dvořák [xdvorar00]
 */

#include "state.h"
#include "main_window.h"
#include <QDebug>
#include <QPen>
#include <QGraphicsScene>
#include <QPainter>
#include <QTextEdit>

int State::counter = 0;

State::State(qreal x, qreal y,qreal w, qreal h, QStackedWidget* leftMenu, AutomatonModel* automatonModel, MainWindow* mainWindow)
    : QGraphicsEllipseItem(0, 0, w, h),
    leftMenu_(leftMenu), automatonModel_(automatonModel), mainWindow_(mainWindow), x_(x), y_(y)
{
    setFlag(ItemIsMovable); // can be dragged with mouse
    setFlag(ItemIsSelectable); // border around state when selected
    setFlag(ItemSendsGeometryChanges); // itemChange
    setBrush(Qt::lightGray);
    setPen(QPen(Qt::black, 2));

    counter++;
    name_ = QString::number(counter);
}

void State::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    qDebug() << "Double-clicked state";

    // in case user clicked on state without saving or backing from Transition Edit
    automatonModel_->updateAndSetNullptrSelectedTransition();

    automatonModel_->setSelectedState(this);
    this->update();

    // open 'State Edit' menu
    leftMenu_->setCurrentWidget(leftMenu_->findChild<QWidget*>("stateEdit"));

    QWidget* stateEdit = leftMenu_->currentWidget();

    // set inputs for name and action in 'State Edit' menu to this state name and action
    stateEdit->findChild<QLineEdit*>("state_name")->setText(name_);
    stateEdit->findChild<QTextEdit*>("state_action")->setText(action_);

    QGraphicsEllipseItem::mouseDoubleClickEvent(event);
}

void State::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        mainWindow_->onStateClicked(this);
    }

    QGraphicsEllipseItem::mousePressEvent(event);
}

QString State::name() const
{
    return name_;
}

void State::setName(const QString& name)
{
    name_ = name;
}

QString State::action() const
{
    return action_;
}

void State::setAction(const QString &action)
{
    action_ = action;
}

void State::addOutgoingTransition(Transition *tr)
{
    outgoing_.append(tr);
}

void State::removeOutgoingTransition(Transition* tr)
{
    outgoing_.removeOne(tr);
}

void State::addIncomingTransition(Transition *tr)
{
    incoming_.append(tr);
}

void State::removeIncomingTransition(Transition* tr)
{
    incoming_.removeOne(tr);
}

bool State::hasTransitions()
{
    return !outgoing_.empty() || !incoming_.empty();
}

qreal State::X() const
{
    return x_;
}

qreal State::Y() const
{
    return y_;
}

// when user drags the state
QVariant State::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange) {
        QPointF position = value.toPointF(); // mouse position as item is dragged
        QRectF sceneRect = scene()->sceneRect();

        // extremes
        qreal minX = sceneRect.left();
        qreal minY = sceneRect.top();
        qreal maxX = sceneRect.right() - this->rect().width();
        qreal maxY = sceneRect.bottom() - this->rect().height();

        // restrict item to move away from boundaries
        position.setX(qBound(minX, position.x(), maxX));
        position.setY(qBound(minY, position.y(), maxY));

        // update transitions
        for(Transition* tr : qAsConst(outgoing_)){
            tr->update();
        }

        for(Transition* tr : qAsConst(incoming_)){
            tr->update();
        }

        return position; // item position in boundaries
    }

    return QGraphicsItem::itemChange(change, value);
}

void State::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    QPen pen;
    pen.setWidth(2);

    // border color change if state is selected (for editing) or its runtime current state
    if(automatonModel_->selectedState() == this || automatonModel_->currentState() == this){
        pen.setColor(Qt::red);
    } else {
        pen.setColor(Qt::black);
    }

    setPen(pen);

    QGraphicsEllipseItem::paint(painter, option, widget);

    // font settings
    painter->setPen(Qt::black);
    QFont font = painter->font();
    font.setPointSize(16);
    painter->setFont(font);

    // paint the name in the center
    QRectF rect = boundingRect();
    painter->drawText(rect, Qt::AlignCenter, name_);
}

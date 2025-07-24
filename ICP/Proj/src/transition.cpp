/**
 * @file transition.cpp
 * @brief Header file for the Transition class, representing a transition in a Finite State Machine (FSM).
 *
 * This file defines implementation for the Transition class, which models a transition between two states
 * in a FSM editor. The class supports interactive editing of conditions and custom
 * rendering with an arrowhead to indicate direction.
 *
 * @author Tomáš Bordák [xborda01]
 * @author Erik Roják [xrojak00]
 * @author Radim Dvořák [xdvorar00]
 */

#include "transition.h"
#include <QPen>
#include <QPainter>
#include <QtMath>
#include <QTextEdit>
#include <QGraphicsScene>

Transition::Transition(State* from, State* to, AutomatonModel* automatonModel, QStackedWidget* leftMenu)
    : from_(from), to_(to), automatonModel_(automatonModel), leftMenu_(leftMenu)
{
    setFlag(ItemIsSelectable);

    // set line properties
    setPen(QPen(Qt::black, 2));
    setZValue(-1); // it goes behind states
}

void Transition::setCondition(const QString& condition)
{
    condition_ = condition;
}

QString Transition::condition()
{
    return condition_;
}

State* Transition::fromState()
{
    return from_;
}

State* Transition::toState()
{
    return to_;
}

void Transition::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    qDebug() << "Double-clicked transition";

    // in case user clicked on transiton without saving or backing from State Edit
    automatonModel_->updateAndSetNullptrSelectedState();

    automatonModel_->setSelectedTransition(this);

    // open 'Transition Edit' menu
    leftMenu_->setCurrentWidget(leftMenu_->findChild<QWidget*>("transitionEdit"));

    QWidget* transitionEdit = leftMenu_->currentWidget();

    // set input for condition in 'Transition Edit' menu
    transitionEdit->findChild<QTextEdit*>("transition_condition")->setText(condition_);

    QGraphicsLineItem::mouseDoubleClickEvent(event);
}

void Transition::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPen pen;
    pen.setWidth(2);
    pen.setColor(
        automatonModel_->selectedTransiton() == this ? Qt::red : Qt::black
    );

    if(from_ == to_){
        qreal dm = from_->rect().height();
        QPointF center = from_->sceneBoundingRect().center();

        // arc is not clickable if Transition is QGraphicsLineItem
        // so we make a fake line at the top, where the transition starts
        QLineF fakeLine(center, center + QPointF(0, -40));
        setLine(fakeLine);

        QRectF loopRect(center.x() - dm, center.y() - dm, dm, dm);

        QPainterPath path;
        path.arcMoveTo(loopRect, 0);
        path.arcTo(loopRect, 0, 270);

        painter->setPen(pen);
        painter->drawPath(path);
        update(); // manually update the path (arc) color
    } else {
        QPointF centerFrom = from_->sceneBoundingRect().center();
        QPointF centerTo = to_->sceneBoundingRect().center();

        // line from center to center
        QLineF line(centerFrom, centerTo);

        // sub state radius (so arrow will be shown just at where the to_ state starts)
        line.setLength(line.length() - 30.0);

        // set the line properties
        setLine(line);

        // draw the line
        painter->setPen(pen);
        painter->drawLine(line);

        // remove old arrowHead
        if(arrowHead_){
            scene()->removeItem(arrowHead_);
        }

        double angle = std::atan2(line.dy(), line.dx()); // angle of the line in radians
        double arrowSize = 8.0;

        QPointF arrowTip = line.p2(); // line end close to to_ state
        QPointF arrowP1 = arrowTip - QPointF(std::cos(angle - M_PI / 6) * arrowSize,
                                             std::sin(angle - M_PI / 6) * arrowSize);
        QPointF arrowP2 = arrowTip - QPointF(std::cos(angle + M_PI / 6) * arrowSize,
                                             std::sin(angle + M_PI / 6) * arrowSize);

        QPolygonF arrowHead;
        arrowHead << arrowTip << arrowP1 << arrowP2; // add the points to polygon

        // draw updated arrow head
        arrowHead_ = new QGraphicsPolygonItem(arrowHead, this);
        arrowHead_->setBrush(Qt::black);
    }
}


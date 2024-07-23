#include "customtextitem.h"
#include "customscene.h"
#include <QDebug>
#include <QTextCursor>


CustomTextItem::CustomTextItem(QGraphicsItem *parent)
    : QGraphicsTextItem(parent)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    positionLastTime = QPointF(0, 0);
}

CustomTextItem* CustomTextItem::clone()
{
    CustomTextItem* cloned = new CustomTextItem(nullptr);
    cloned->setPlainText(toPlainText());
    cloned->setFont(font());
    cloned->setTextWidth(textWidth());
    cloned->setDefaultTextColor(defaultTextColor());
    cloned->setPos(scenePos());
    cloned->setZValue(zValue());
    return cloned;
}

QVariant CustomTextItem::itemChange(GraphicsItemChange change,
                     const QVariant &value)
{
    if (change == QGraphicsItem::ItemSelectedHasChanged)
        emit selectedChange(this);
    return value;
}

void CustomTextItem::focusInEvent(QFocusEvent* event)
{
    qDebug() << "start editing";
    if (positionLastTime == QPointF(0, 0))
        // initialize positionLastTime to insertion position
        positionLastTime = scenePos();
    QGraphicsTextItem::focusInEvent(event);
}

void CustomTextItem::focusOutEvent(QFocusEvent *event)
{
    setTextInteractionFlags(Qt::NoTextInteraction);
    qDebug() << "after editing" << this;
    if (contentLastTime == toPlainText())
    {
        contentHasChanged = false;
    }
    else
    {
        contentLastTime = toPlainText();
        contentHasChanged = true;
    }
    emit lostFocus(this);
    QGraphicsTextItem::focusOutEvent(event);
}

void CustomTextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (textInteractionFlags() == Qt::NoTextInteraction)
    {
        setTextInteractionFlags(Qt::TextEditorInteraction);
    }
    QGraphicsTextItem::mouseDoubleClickEvent(event);
}

void CustomTextItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    qDebug() << "text begin move";
    QGraphicsTextItem::mousePressEvent(event);
}

void CustomTextItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (scenePos() != positionLastTime)
    {
        qDebug() << scenePos() << "::" << positionLastTime;
        isMoved = true;
    }
    positionLastTime = scenePos();
    qDebug() << "text end moving";
    QGraphicsTextItem::mouseReleaseEvent(event);
}

#include "customview.h"
#include "customitem.h"
#include "customtextitem.h"
#include <QGraphicsItem>


CustomView::CustomView(QGraphicsScene *scene, QWidget *parent)
    :QGraphicsView(scene, parent)
{
    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    setDragMode(QGraphicsView::RubberBandDrag);

}

void CustomView::keyPressEvent(QKeyEvent *event)
{
    if((event->modifiers() & Qt::KeyboardModifier::ControlModifier) != 0)
    {
        setDragMode(DragMode::RubberBandDrag);
    }
    QGraphicsView::keyPressEvent(event);

}

void CustomView::keyReleaseEvent(QKeyEvent *event)
{
    if((event->modifiers() & Qt::KeyboardModifier::ControlModifier) !=0)
    {
        setDragMode(DragMode::RubberBandDrag);
    }
}

void CustomView::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);
    bool needEmit = false;
    foreach(QGraphicsItem* item, scene()->selectedItems())
    {
        if(item->type() == CustomItem::Type)
        {
            CustomItem *p = qgraphicsitem_cast<CustomItem*>(item);
            if(p->isMoved)
            {
                needEmit = true;
                p->isMoved = false;
            }
            if(p->isResized)
            {
                needEmit = true;
                p->isResized = false;

            }
        }
        else if(item->type() == CustomTextItem::Type)
        {
            CustomTextItem *p = qgraphicsitem_cast<CustomTextItem*>(item);
            if(p->positionIsUpdated())
            {
                needEmit = true;
                p->setUpdated();
            }

        }
    }
    if(needEmit)
        emit needsUndoBackUp();

}

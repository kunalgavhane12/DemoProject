#include "customitem.h"
#include "arrow.h"

#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>
#include <QDebug>
#include <QStyleOptionGraphicsItem>
#include <QInputDialog>
#include <QMessageBox>

int CustomItem::idCounter = 0;

CustomItem::CustomItem(CustomType customType, QMenu *contextMenu, QGraphicsItem *parent)
    : QGraphicsPolygonItem(parent)
{
    myCustomType = customType;
    myContextMenu = contextMenu;
    myId = idCounter++;

    QPainterPath path;
    switch (myCustomType) {
    case Output:
        path.moveTo(200, 50);
        path.arcTo(150, 0, 50, 50, 0, 90);
        path.arcTo(50, 0, 50, 50, 90, 90);
        path.arcTo(50, 50, 50, 50, 180, 90);
        path.arcTo(150, 50, 50, 50, 270, 90);
        path.lineTo(200, 50);
        myPolygon = path.toFillPolygon().translated(-125, -50);
        break;
    case Diamond:
        myPolygon << QPointF(-75, 0) << QPointF(0, 75)
                  << QPointF(75, 0) << QPointF(0, -75)
                  << QPointF(-75, 0);
        break;
    case Rectangle:
        myPolygon << QPointF(-60, -60) << QPointF(60, -60)
                  << QPointF(60, 60) << QPointF(-60, 60)
                  << QPointF(-60, -60);
        break;
    case Triangle:
        myPolygon << QPointF(0, -75) << QPointF(65, 65)
                  << QPointF(-65, 65) << QPointF(0, -75);
        break;
    case Circle:
        path.addEllipse(QPointF(0, 0), 50, 50);
        myPolygon = path.toFillPolygon();
        break;
    default:
        myPolygon << QPointF(-60, -40) << QPointF(-35, 40)
                  << QPointF(60, 40) << QPointF(35, -40)
                  << QPointF(-60, -40);
        break;
    }
    setPolygon(myPolygon);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setAcceptHoverEvents(true);
}


void CustomItem::removeArrow(Arrow *arrow)
{
    int index = arrows.indexOf(arrow);

    if (index != -1)
        arrows.removeAt(index);
}

void CustomItem::removeArrows()
{
    foreach (Arrow *arrow, arrows)
    {
        arrow->startItem()->removeArrow(arrow);
        arrow->endItem()->removeArrow(arrow);
        scene()->removeItem(arrow);
        delete arrow;
    }
}

void CustomItem::addArrow(Arrow *arrow)
{
    arrows.append(arrow);
}

QPixmap CustomItem::image() const
{
    QPixmap pixmap(250, 250);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::black, 8));
    painter.translate(125, 125);
    painter.drawPolyline(myPolygon);

    return pixmap;
}

QList<QPointF> CustomItem::resizeHandlePoints()
{
    qreal width = resizeHandlePointWidth;
    QRectF rf = QRectF(boundingRect().topLeft() + QPointF(width/2, width/2),
                       boundingRect().bottomRight() - QPointF(width/2, width/2));
    qreal centerX = rf.center().x();
    qreal centerY = rf.center().y();
    return QList<QPointF>{rf.topLeft(), QPointF(centerX, rf.top()), rf.topRight(),
                QPointF(rf.left(), centerY), QPointF(rf.right(), centerY),
                rf.bottomLeft(), QPointF(centerX, rf.bottom()), rf.bottomRight()};
}

bool CustomItem::isCloseEnough(QPointF const& p1, QPointF const& p2)
{
    qreal delta = std::abs(p1.x() - p2.x()) + std::abs(p1.y() - p2.y());
    return delta < closeEnoughDistance;
}

CustomItem* CustomItem::clone()
{
    CustomItem* cloned = new CustomItem(myCustomType, myContextMenu, nullptr);
    cloned->myPolygon = myPolygon;
    cloned->setPos(scenePos());
    cloned->setPolygon(myPolygon);
    cloned->setBrush(brush());
    cloned->setZValue(zValue());
    return cloned;
}


void CustomItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    resizeMode = false;
    int index = 0;
    foreach (QPointF const& p, resizeHandlePoints())
    {
        if (isCloseEnough(event->pos(), p))
        {
            resizeMode = true;
            break;
        }
        index++;
    }
    scaleDirection = static_cast<Direction>(index);
    setFlag(GraphicsItemFlag::ItemIsMovable, !resizeMode);
    if (resizeMode)
    {
        qDebug() << "begin resizing";
        previousPolygon = polygon();
        event->accept();
    }
    else
    {
        qDebug() << "item type " << this->type() << " start moving from" << scenePos();
        movingStartPosition = scenePos();
        QGraphicsItem::mousePressEvent(event);
    }
}

void CustomItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (resizeMode)
    {
        prepareGeometryChange();
        myPolygon = scaledPolygon(myPolygon, scaleDirection, event->pos());
        setPolygon(myPolygon);
    }
    QGraphicsItem::mouseMoveEvent(event);
}

void CustomItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (resizeMode)
    {
        qDebug() << "after resizing";
        if (polygon() != previousPolygon)
        {
            isResized = true;
        }
    }
    else
    {
        qDebug() << "\tend moving in" << scenePos();
        if (scenePos() != movingStartPosition)
        {
            isMoved = true;
            qDebug() << "-- " << scenePos() << movingStartPosition;
        }
    }
    resizeMode = false;
    QGraphicsItem::mouseReleaseEvent(event);
}

void CustomItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
    setCursor(Qt::ArrowCursor);
    int index = 0;
    foreach (QPointF const& p, resizeHandlePoints())
    {
        if (isCloseEnough(p, event->pos()))
        {
            switch (static_cast<Direction>(index)) {
            case TopLeft:
            case BottomRight: setCursor(Qt::SizeFDiagCursor); break;
            case Top:
            case Bottom: setCursor(Qt::SizeVerCursor); break;
            case TopRight:
            case BottomLeft: setCursor(Qt::SizeBDiagCursor); break;
            case Left:
            case Right: setCursor(Qt::SizeHorCursor); break;
            }
            break;
        }
        index++;
    }
    QGraphicsItem::hoverMoveEvent(event);
}

void CustomItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                       QWidget* widget)
{
    // remove build-in selected state
    QStyleOptionGraphicsItem myOption(*option);
    myOption.state &= ~QStyle::State_Selected;
    QGraphicsPolygonItem::paint(painter, &myOption, widget);

    // add resize handles
    if (this->isSelected())
    {
        qreal width = resizeHandlePointWidth;
        foreach(QPointF const& point, resizeHandlePoints())
        {
            painter->drawEllipse(QRectF(point.x() - width/2, point.y() - width/2, width, width));
        }
    }
}

void CustomItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    scene()->clearSelection();
    setSelected(true);
    myContextMenu->exec(event->screenPos());
}

QVariant CustomItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionChange)
    {
        foreach (Arrow *arrow, arrows)
        {
            arrow->updatePosition();
        }
    }

    return value;
}

void CustomItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);

    switch (myCustomType) {
    case Output:
        break;

    case Diamond:
    {
        bool ok;
        QStringList operations = {"+", "-", "*", "/"};
        QString operation = QInputDialog::getItem(nullptr, "Set Operation",
                                                  "Select an operation:",
                                                  operations, 0, false, &ok);
        if (ok && !operation.isEmpty())
        {
            this->operation = operation;
        }
    }
        break;

    case Rectangle:
    {

        if (areConnectedToConditionalItems())
        {
            result = performArithmeticOperation();

            qDebug() << "Result: " << result;

        }
    }
        break;

    case Io:
    {
        bool ok;
        QString text = QInputDialog::getText(nullptr, "Set Value",
                                             "Enter the value:", QLineEdit::Normal,"", &ok);
        if (ok && !text.isEmpty()) {
            this->value = text;
        }
    }
        break;

    default:
        break;
    }
}



QPolygonF CustomItem::scaledPolygon(const QPolygonF& old, CustomItem::Direction direction,
                                    const QPointF& newPos)
{
    qreal oldWidth = old.boundingRect().width();
    qreal oldHeight = old.boundingRect().height();
    qreal scaleWidth, scaleHeight;
    switch(direction)
    {
    case TopLeft:
    {
        QPointF fixPoint = old.boundingRect().bottomRight();
        scaleWidth = (fixPoint.x() - newPos.x()) / oldWidth;
        scaleHeight = (fixPoint.y() - newPos.y()) / oldHeight;
        break;
    }
    case Top:
    {
        QPointF fixPoint = old.boundingRect().bottomLeft();
        scaleWidth = 1;
        scaleHeight = (fixPoint.y() - newPos.y()) / oldHeight;
        break;
    }
    case TopRight:
    {
        QPointF fixPoint = old.boundingRect().bottomLeft();
        scaleWidth = (newPos.x() - fixPoint.x()) / oldWidth;
        scaleHeight = (fixPoint.y() - newPos.y() ) / oldHeight;
        break;
    }
    case Right:
    {
        QPointF fixPoint = old.boundingRect().bottomLeft();
        scaleWidth = (newPos.x() - fixPoint.x()) / oldWidth;
        scaleHeight = 1;
        break;
    }
    case BottomRight:
    {
        QPointF fixPoint = old.boundingRect().topLeft();
        scaleWidth = (newPos.x() - fixPoint.x()) / oldWidth;
        scaleHeight = (newPos.y() - fixPoint.y()) / oldHeight;
        break;
    }
    case Bottom:
    {
        QPointF fixPoint = old.boundingRect().topLeft();
        scaleWidth = 1;
        scaleHeight = (newPos.y() - fixPoint.y()) / oldHeight;
        break;
    }
    case BottomLeft: {
        QPointF fixPoint = old.boundingRect().topRight();
        scaleWidth = (fixPoint.x() - newPos.x()) / oldWidth;
        scaleHeight = (newPos.y() - fixPoint.y()) / oldHeight;
        break;
    }
    case Left:
    {
        QPointF fixPoint = old.boundingRect().bottomRight();
        scaleWidth = (fixPoint.x() - newPos.x()) / oldWidth;
        scaleHeight = 1;
        break;
    }
    }
    QTransform trans;
    trans.scale(scaleWidth, scaleHeight);
    return trans.map(old);
}

bool CustomItem::areConnectedToConditionalItems()
{
    foreach (Arrow *arrow, arrows)
    {
        if (arrow->startItem()->type() == Diamond || arrow->endItem()->type() == Diamond)
        {
            return true;
        }
    }
    return false;
}

double CustomItem::performArithmeticOperation()
{
    double result = 0;
    foreach (Arrow *arrow, arrows)
    {
        CustomItem *startItem = dynamic_cast<CustomItem*>(arrow->startItem());
        CustomItem *endItem = dynamic_cast<CustomItem*>(arrow->endItem());

        if (startItem && endItem)
        {
            if (startItem->myCustomType == Diamond || endItem->myCustomType == Diamond)
            {
                value1 = startItem->value.toDouble();
                value2 = endItem->value.toDouble();
                if (operation == "+")
                    result = value1 + value2;
                else if (operation == "-")
                    result = value1 - value2;
                else if (operation == "*")
                    result = value1 * value2;
                else if (operation == "/")
                    result = value1 / value2;
            }
        }
    }
    return result;
}


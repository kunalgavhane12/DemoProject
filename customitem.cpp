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
double CustomItem::rectangleArea = 0;
double CustomItem::rectanglePerimeter = 0;
double CustomItem::circleArea = 0;
double CustomItem::circleCircumference = 0;
double CustomItem::triangleArea = 0;
double CustomItem::trianglePerimeter = 0;

CustomItem::CustomItem(CustomType customType, QMenu *contextMenu, QGraphicsItem *parent)
    : QGraphicsPolygonItem(parent)
{
    myCustomType = customType;
    myContextMenu = contextMenu;
    myId = idCounter++;
    QPainterPath path;
    QPixmap pixmap;
    switch (myCustomType) {
    case Output:
        path.moveTo(200, 50);
        path.arcTo(150, 0, 50, 50, 0, 90);
        path.arcTo(50, 0, 50, 50, 90, 90);
        path.arcTo(50, 50, 50, 50, 180, 90);
        path.arcTo(150, 50, 50, 50, 270, 90);
        path.lineTo(200, 50);
        myPolygon = path.toFillPolygon().translated(-125, -50);
        textItem = new QGraphicsTextItem("Result :", this);
        textItem->setPos(-40, -20);
        setPolygon(myPolygon);
        break;
    case Diamond:
        myPolygon << QPointF(-75, 0) << QPointF(0, 75)
                  << QPointF(75, 0) << QPointF(0, -75)
                  << QPointF(-75, 0);
        textItem = new QGraphicsTextItem("Diamond", this);
        textItem->setPos(-30, -10);
        setPolygon(myPolygon);
        break;
    case Rectangle:
        myPolygon << QPointF(-60, -60) << QPointF(60, -60)
                  << QPointF(60, 60) << QPointF(-60, 60)
                  << QPointF(-60, -60);
        textItem = new QGraphicsTextItem("Length : \nWidth :", this);
        textItem->setPos(-40, -40);
        setPolygon(myPolygon);
        break;
    case Triangle:
        myPolygon << QPointF(0, -75) << QPointF(65, 65)
                  << QPointF(-65, 65) << QPointF(0, -75);

        textItem = new QGraphicsTextItem("Base :\nAltitude :\nHypotenuse :\nHeight :", this);
        textItem->setPos(-40, 10);
        setPolygon(myPolygon);
        break;
    case Circle:
        path.addEllipse(QPointF(0, 0), 50, 50);
        myPolygon = path.toFillPolygon();
        textItem = new QGraphicsTextItem("Radius : ", this);
        textItem->setPos(-20, -10);
        setPolygon(myPolygon);
        break;

    default:
        myPolygon << QPointF(-60, -40) << QPointF(-35, 40)
                  << QPointF(60, 40) << QPointF(35, -40)
                  << QPointF(-60, -40);
        setPolygon(myPolygon);
        break;
    }

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setAcceptHoverEvents(true);
}

void CustomItem::setMainLabelText(const QString &text)
{
    if (textItem)
        textItem->setPlainText(text);
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

void CustomItem::setRectangleProperty()
{
    bool ok;

    double length = QInputDialog::getDouble(nullptr, "Enter Rectangle Length:", "Length:", 0, 0, 10000, 2, &ok);
    if (!ok) return;

    double width = QInputDialog::getDouble(nullptr, "Enter Rectangle Width:", "Width:", 0, 0, 10000, 2, &ok);
    if (!ok) return;

    rectangleArea = length * width;
    rectanglePerimeter = 2 * (length + width);

    textItem->setPlainText("Length : " + QString::number(length) + "\nWidth : " + QString::number(width));
    textItem->setPos(-40,-40);

    qDebug() << "Rectangle Property" << length << " " << width << rectangleArea << rectanglePerimeter;
}

void CustomItem::setCircleProperty()
{
    bool ok;
    double radius = QInputDialog::getDouble(nullptr, "Enter Circle Radius:", "Radius:", 0, 0, 10000, 2, &ok);
    if (!ok) return;

    textItem->setPlainText("Radius : " + QString::number(radius));
    textItem->setPos(-20,-10);
    circleArea = 3.14 * radius * radius;
    circleCircumference = 2 * 3.14 * radius;

    qDebug() << "Circle Property" << circleArea << " " << circleCircumference;
}

void CustomItem::setTriangleProperty()
{
    bool ok;

    double base = QInputDialog::getDouble(nullptr, "Enter Base:", "Base:", 0, 0, 10000, 2, &ok);
    if (!ok) return;

    double height = QInputDialog::getDouble(nullptr, "Enter Height:", "Height:", 0, 0, 10000, 2, &ok);
    if (!ok) return;

    double altitude = QInputDialog::getDouble(nullptr, "Enter Altitude:", "Altitude:", 0, 0, 10000, 2, &ok);
    if (!ok) return;

    double hypotenuse = QInputDialog::getDouble(nullptr, "Enter Hypotenuse:", "Hypotenuse:", 0, 0, 10000, 2, &ok);
    if (!ok) return;

    triangleArea = (base * height) / 2;
    trianglePerimeter = altitude + base + hypotenuse;

    textItem->setPlainText("Base : "+QString::number(base)+"\nAltitude : "+QString::number(altitude)+""
                                                                                                     "\nHypotenuse :"+QString::number(hypotenuse)+"\nHeight : " + QString::number(height));
    textItem->setPos(-40,-10);

    qDebug() << "Triangle Property " << base << " " << height << " " << triangleArea << " " << trianglePerimeter;
}

void CustomItem::setPolygonProperty()
{
    qDebug() << "Polygon Property";
}

void CustomItem::setDiamondProperty()
{
    qDebug() << "Diamond Property";
}


void CustomItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    resizeMode = false;
    int index = 0;

    if (dynamic_cast<QGraphicsPixmapItem*>(this))
    {
        QGraphicsItem::mousePressEvent(event);
        return;
    }
    QGraphicsPolygonItem::mousePressEvent(event);

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
    if (dynamic_cast<QGraphicsPixmapItem*>(this))
    {
        QGraphicsItem::mouseMoveEvent(event);
        return;
    }
    QGraphicsPolygonItem::mouseMoveEvent(event);
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
    if (dynamic_cast<QGraphicsPixmapItem*>(this))
    {
        QGraphicsItem::mouseReleaseEvent(event);
        return;
    }
    QGraphicsPolygonItem::mouseReleaseEvent(event);
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

void CustomItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
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
    return QGraphicsPolygonItem::itemChange(change, value);
}


void CustomItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);

    switch (myCustomType) {
    case Diamond:
        qDebug() << "Diamond double-clicked";
        setDiamondProperty();
        break;
    case Rectangle:
        qDebug() << "Rectangle double-clicked";
        setRectangleProperty();
        break;
    case Triangle:
        qDebug() << "Triangle double-clicked";
        setTriangleProperty();
        break;
    case Circle:
        qDebug() << "Circle double-clicked";
        setCircleProperty();
        break;
    case Polygon:
        qDebug() << "Polygon double-clicked";
        setPolygonProperty();
        break;
    case Output:
        qDebug() << "Output double-clicked";
        performArithmeticOperation();
        break;
    case Io:
        qDebug() << "IO double-clicked";
        bool ok;
        QString text = QInputDialog::getText(nullptr, "Set Value", "Enter the value:", QLineEdit::Normal, "", &ok);
        break;

    }
}


QPolygonF CustomItem::scaledPolygon(const QPolygonF& old, CustomItem::Direction direction, const QPointF& newPos)
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

void CustomItem::performArithmeticOperation() {
    qDebug() << "Performing arithmetic operation";
    foreach (Arrow *arrow, arrows) {
        CustomItem *startItem = dynamic_cast<CustomItem*>(arrow->startItem());
        CustomItem *endItem = dynamic_cast<CustomItem*>(arrow->endItem());

        if (startItem && endItem)
        {
            qDebug() << "Start Item Type: " << startItem->myCustomType << " End Item Type: " << endItem->myCustomType;
            if (startItem->myCustomType == Rectangle && endItem->myCustomType == Output)
            {
                qDebug() << "Rectangle Property" << rectangleArea << " " << rectanglePerimeter;
                textItem->setPlainText("Area : " + QString::number(rectangleArea) + "\nPerimeter : " + QString::number(rectanglePerimeter));
                textItem->setPos(-40, -20);

            }
            else if (startItem->myCustomType == Triangle && endItem->myCustomType == Output)
            {
                qDebug() << "Triangle Property" << triangleArea << " " << trianglePerimeter;
                textItem->setPlainText("Area : " + QString::number(triangleArea) + "\nPerimeter : " + QString::number(trianglePerimeter));
                textItem->setPos(-40, -20);
            }
            else if (startItem->myCustomType == Circle && endItem->myCustomType == Output)
            {
                qDebug() << "Circle Property" << circleArea << " " << circleCircumference;
                textItem->setPlainText("Area : " + QString::number(circleArea) + "\nPerimeter : " + QString::number(circleCircumference));
                textItem->setPos(-40, -20);
            }
            else if (startItem->myCustomType == Diamond && endItem->myCustomType == Output)
            {
                qDebug() << "Diamond item is connected to Output";
            }
            else if (startItem->myCustomType == Polygon && endItem->myCustomType == Output)
            {
                qDebug() << "Polygon item is connected to Output";
            }
            else
            {
                qDebug() << "Unhandled arithmetic operation case";
            }
        }
        else
        {
            qDebug() << "Arrow connection issue: startItem or endItem is null";
        }
    }
}

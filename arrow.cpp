#include "arrow.h"
#include "customitem.h"

#include <math.h>
#include <QPen>
#include <QPainter>

const qreal Pi = 3.14;

Arrow::Arrow(CustomItem *startItem, CustomItem *endItem, QGraphicsItem *parent)
    : QGraphicsLineItem(parent)
{
    myStartItem = startItem;
    myEndItem = endItem;
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    myColor = Qt::black;
    setPen(QPen(myColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}

QRectF Arrow::boundingRect() const
{
    qreal extra = (pen().width() + 20) / 2.0;

    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
                                      line().p2().y() - line().p1().y()))
            .normalized()
            .adjusted(-extra, -extra, extra, extra);
}

QPainterPath Arrow::shape() const
{
    QPainterPath path = QGraphicsLineItem::shape();
    path.addPolygon(arrowHead);
    return path;
}

bool Arrow::operator==(Arrow &arrow)
{
    return (startItem() == arrow.startItem()) && (endItem() == arrow.endItem());
}

void Arrow::updatePosition()
{
    QLineF line(mapFromItem(myStartItem, 0, 0), mapFromItem(myEndItem, 0, 0));
    setLine(line);
}

void Arrow::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                  QWidget *)
{
    if (myStartItem->collidesWithItem(myEndItem))
        return;

    QPen myPen = pen();
    myPen.setColor(myColor);
    qreal arrowSize = 10;
    painter->setPen(myPen);
    painter->setBrush(myColor);

    QLineF centerLine(myStartItem->pos(), myEndItem->pos());

    // Calculate intersection points with the start item
    QPolygonF startPolygon = myStartItem->polygon();
    QPointF startIntersectPoint = calculateIntersectionPoint(startPolygon, myStartItem, centerLine);
    if (!startIntersectPoint.isNull())
        centerLine.setP1(startIntersectPoint);

    // Calculate intersection points with the end item
    QPolygonF endPolygon = myEndItem->polygon();
    QPointF endIntersectPoint = calculateIntersectionPoint(endPolygon, myEndItem, centerLine);
    if (!endIntersectPoint.isNull())
        centerLine.setP2(endIntersectPoint);

    setLine(centerLine);

//    double angle = std::atan2(-line().dy(), line().dx());
    double angle = ::acos(line().dx() / line().length());
    if (line().dy() >= 0)
        angle = (Pi * 2) - angle;


    QPointF arrowP1 = line().p1() + QPointF(sin(angle + Pi / 3) * arrowSize,
                                            cos(angle + Pi / 3) * arrowSize);
    QPointF arrowP2 = line().p1() + QPointF(sin(angle + Pi - Pi / 3) * arrowSize,
                                            cos(angle + Pi - Pi / 3) * arrowSize);

    arrowHead.clear();
    arrowHead << line().p1() << arrowP1 << arrowP2;

    painter->drawLine(line());
//    painter->drawPolygon(arrowHead);

    // Draw circles at the connection points
    painter->setBrush(Qt::black);
    painter->drawEllipse(line().p1(), 3, 3);
    painter->drawEllipse(line().p2(), 3, 3);

    if (isSelected())
    {
        painter->setPen(QPen(myColor, 1, Qt::DashLine));
        QLineF myLine = line();
        myLine.translate(0, 4.0);
        painter->drawLine(myLine);
        myLine.translate(0, -8.0);
        painter->drawLine(myLine);
    }
}

QPointF Arrow::calculateIntersectionPoint(const QPolygonF &polygon, CustomItem *item, const QLineF &line)
{
    QPointF p1 = polygon.first() + item->pos();
    QPointF p2;
    QPointF intersectPoint;
    QLineF polyLine;
    for (int i = 1; i < polygon.count(); i++)
    {
        p2 = polygon.at(i) + item->pos();
        polyLine = QLineF(p1, p2);
        QLineF::IntersectType intersectType = polyLine.intersect(line, &intersectPoint);
        if (intersectType == QLineF::BoundedIntersection)
            return intersectPoint;
        p1 = p2;
    }
    return QPointF();
}

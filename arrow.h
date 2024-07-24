#ifndef ARROW_H
#define ARROW_H

#include <QGraphicsLineItem>

#include "customitem.h"
#include <QGraphicsLineItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QRectF>
#include <QGraphicsSceneMouseEvent>
#include <QPainterPath>


class Arrow : public QGraphicsLineItem
{
public:
    enum { Type = UserType + 4 };

    Arrow(CustomItem *startItem, CustomItem *endItem,QGraphicsItem *parent = 0);

    int type() const override { return Type; }
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void setColor(const QColor &color) { myColor = color; }
    QColor getColor() {return myColor;}
    CustomItem *startItem() const { return myStartItem; }
    CustomItem *endItem() const { return myEndItem; }
    bool operator==(Arrow &arrow);

    void updatePosition();

    QPointF calculateIntersectionPoint(const QPolygonF &polygon, CustomItem *item, const QLineF &line);
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;

private:
    CustomItem *myStartItem;
    CustomItem *myEndItem;
    QColor myColor;

    QPolygonF arrowHead;
};

#endif // ARROW_H

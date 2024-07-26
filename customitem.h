#ifndef customitem_H
#define customitem_H

#include <QGraphicsPixmapItem>
#include <QList>
#include <QPixmap>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QTextEdit>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QPolygonF>
#include "customtextitem.h"

class Arrow;

class CustomItem : public QGraphicsPolygonItem
{
    friend class CustomView;
public:
    enum { Type = UserType + 15 };

    enum CustomType { Rectangle, Circle, Triangle, Diamond, Polygon, Output, Io,
                      TractorBlack, TractorOk, TractorOnField, TractorOrange, TractorRed,
                      TractorTransperant, TractorYellow};

    enum Direction {TopLeft = 0, Top, TopRight, Left, Right, BottomLeft, Bottom, BottomRight };

    CustomItem(CustomType customType, QMenu *contextMenu, QGraphicsItem *parent = nullptr);
    QString title;
    void removeArrow(Arrow *arrow);
    void removeArrows();

    int id() const { return myId; }
    void setId(int id) { myId = id; }
    CustomType customType() const { return myCustomType; }

    QPolygonF polygon() const { return myPolygon; }

    void addArrow(Arrow *arrow);
    QList<Arrow*> getArrows() const { return arrows; }

    QPixmap image() const;

    int type() const override { return Type;}

    QList<QPointF> resizeHandlePoints();
    bool isCloseEnough(QPointF const& p1, QPointF const& p2);
    CustomItem* clone();

    void setRectangleProperty();
    void setCircleProperty();
    void setTriangleProperty();
    void setPolygonProperty();
    void setDiamondProperty();

    static double rectangleArea;
    static double rectanglePerimeter;
    static double circleArea;
    static double circleCircumference;
    static double triangleArea;
    static double trianglePerimeter;

    void setMainLabelText(const QString &text);
    void setPixmap(const QPixmap &pixmap);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QPolygonF scaledPolygon(QPolygonF const& old, Direction direction, QPointF const& newPos);

    QGraphicsTextItem *textItem;
    CustomType myCustomType;
    QMenu *myContextMenu;
    int myId;
    QList<Arrow *> arrows;
    static int idCounter;
    QPolygonF myPolygon;
    static constexpr qreal resizeHandlePointWidth = 5;
    static constexpr qreal closeEnoughDistance = 5;
    bool resizeMode = false;
    Direction scaleDirection;

    QPointF movingStartPosition;
    bool isMoved = false;
    QPolygonF previousPolygon;
    bool isResized = false;
    QString  operation;

    bool areConnectedToConditionalItems();
    void performArithmeticOperation();

};

#endif // customitem_H

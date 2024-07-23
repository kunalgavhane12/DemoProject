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

class Arrow;

class CustomItem : public QGraphicsPolygonItem
{
    friend class CustomView;
public:
    enum { Type = UserType + 15 };

    enum CustomType { Step, Conditional, StartEnd, Io };

    enum Direction {TopLeft = 0, Top, TopRight, Left, Right, BottomLeft, Bottom, BottomRight };

    CustomItem(CustomType customType, QMenu *contextMenu, QGraphicsItem *parent = nullptr);

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

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    QPolygonF scaledPolygon(QPolygonF const& old, Direction direction, QPointF const& newPos);

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
};

#endif // customitem_H

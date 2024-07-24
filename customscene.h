#ifndef CUSTOMSCENE_H
#define CUSTOMSCENE_H

#include "customitem.h"
#include "customtextitem.h"

#include <QDomDocument>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QPointF>
#include <QGraphicsLineItem>
#include <QFont>
#include <QGraphicsTextItem>
#include <QColor>

class CustomScene : public QGraphicsScene
{
    Q_OBJECT

public:
    enum Mode { InsertItem, InsertLine, InsertText, MoveItem };

    explicit CustomScene(QMenu *itemMenu, QObject *parent = nullptr);

    QFont font() const { return myFont; }
    QColor textColor() const { return myTextColor; }
    QColor itemColor() const { return myItemColor; }
    QColor lineColor() const { return myLineColor; }

    void setLineColor(const QColor &color);
    void setTextColor(const QColor &color);
    void setItemColor(const QColor &color);
    void setFont(const QFont &font);

    // utilities
    void deleteItems(QList<QGraphicsItem*> const& items);
    void saveToXml(QDomDocument &doc, QDomElement &root);
    void loadFromXml(const QDomElement &root);


public slots:
    void setMode(Mode mode);
    void setItemType(CustomItem::CustomType type);
    void editorLostFocus(CustomTextItem *item);

signals:
    void itemInserted(CustomItem *item);
    void textInserted(QGraphicsTextItem *item);
    void textChanged();
    void arrowInserted();
    void itemSelected(QGraphicsItem *item);
    void scaleChanging(int delta);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void wheelEvent(QGraphicsSceneWheelEvent* wheelEvent) override;

private:
    void mouseDraggingMoveEvent(QGraphicsSceneMouseEvent* event);
    void clearOrthogonalLines();
    inline bool closeEnough(qreal x, qreal y, qreal delta);
    enum LineAttr { Other = 0, Horizontal, Vertical, Both};

    LineAttr getPointsRelationship(QPointF const& p1, QPointF const& p2);
    void tryEnteringStickyMode(QGraphicsItem* item, QPointF const& target, QPointF const& mousePos);
    void tryLeavingStickyMode(QGraphicsItem* item, QPointF const& mousePos);

    CustomItem::CustomType myItemType;

    QMenu *myItemMenu;
    Mode myMode;
    QPointF startPoint;
    QGraphicsLineItem *line;
    QFont myFont;

    CustomTextItem *textItem;
    QColor myTextColor;
    QColor myItemColor;
    QColor myLineColor;

    bool horizontalStickyMode = false;
    bool verticalStickyMode = false;
    QPointF horizontalStickPoint;
    QPointF verticalStickPoint;
    QList<QGraphicsLineItem*> orthogonalLines;

    bool hasItemSelected = false;

    static const QPen penForLines;
    static constexpr qreal Delta = 0.1;
    static constexpr qreal stickyDistance = 5;
};

#endif // CUSTOMSCENE_H

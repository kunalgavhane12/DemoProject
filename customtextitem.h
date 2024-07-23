#ifndef CUSTOMTEXTITEM_H
#define CUSTOMTEXTITEM_H

#include <QGraphicsTextItem>
#include <QPen>
#include <QFocusEvent>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>


class CustomTextItem : public QGraphicsTextItem
{
    Q_OBJECT

public:
    enum { Type = UserType + 3 };

    CustomTextItem(QGraphicsItem *parent = nullptr);

    int type() const override { return Type; }

    bool contentIsUpdated() { return contentHasChanged; }
    bool positionIsUpdated() { return isMoved; }
    void setUpdated() { isMoved = false; }
    CustomTextItem* clone();

signals:
    void lostFocus(CustomTextItem *item);
    void selectedChange(QGraphicsItem *item);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QString contentLastTime;
    QPointF positionLastTime;
    bool isMoved = false;
    bool contentHasChanged = false;
};

#endif // CUSTOMTEXTITEM_H

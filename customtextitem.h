#ifndef CUSTOMTEXTITEM_H
#define CUSTOMTEXTITEM_H

#include <QGraphicsTextItem>
#include <QPen>
#include <QFocusEvent>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QColor>


class CustomTextItem : public QGraphicsTextItem
{
    Q_OBJECT

public:
    enum { Type = UserType + 3 };

    CustomTextItem(QGraphicsItem *parent = nullptr);

    int type() const override { return Type; }
    void setText(QString text) { contentLastTime = text; }
    QString getText() { return contentLastTime; }
    bool contentIsUpdated() { return contentHasChanged; }
    bool positionIsUpdated() { return isMoved; }
    void setUpdated() { isMoved = false; }

    CustomTextItem* clone();

    const QColor &getMyColor() const;
    void setMyColor(const QColor &newMyColor);

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
     QColor myColor;
    bool isMoved = false;
    bool contentHasChanged = false;
};

#endif // CUSTOMTEXTITEM_H

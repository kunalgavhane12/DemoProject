#include "customscene.h"
#include "arrow.h"

#include <QTextCursor>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

QPen const CustomScene::penForLines = QPen(QBrush(QColor(Qt::black)), 2, Qt::PenStyle::DashLine);

CustomScene::CustomScene(QMenu *itemMenu, QObject *parent)
    : QGraphicsScene(parent)
{
    myItemMenu = itemMenu;
    myMode = MoveItem;
    myItemType = CustomItem::Rectangle;
    line = nullptr;
    textItem = nullptr;
    myItemColor = Qt::white;
    myTextColor = Qt::black;
    myLineColor = Qt::black;
}

void CustomScene::setLineColor(const QColor &color)
{
    myLineColor = color;
    foreach (QGraphicsItem* p, selectedItems())
    {
        if (p->type() == Arrow::Type)
        {
            Arrow* item = qgraphicsitem_cast<Arrow*>(p);
            item->setColor(myLineColor);
            update();
        }
    }
}

void CustomScene::setTextColor(const QColor &color)
{
    myTextColor = color;
    foreach (QGraphicsItem* p, selectedItems())
    {
        if (p->type() == CustomTextItem::Type)
        {
            CustomTextItem* item = qgraphicsitem_cast<CustomTextItem*>(p);
            item->setDefaultTextColor(myTextColor);
        }
    }
}

void CustomScene::setItemColor(const QColor &color)
{
    myItemColor = color;
    foreach (QGraphicsItem* p, selectedItems())
    {
        if (p->type() == CustomItem::Type)
        {
            CustomItem* item = qgraphicsitem_cast<CustomItem*>(p);
            item->setBrush(myItemColor);
        }
    }
}

void CustomScene::setFont(const QFont &font)
{
    myFont = font;
    foreach (QGraphicsItem* p, selectedItems())
    {
        if (p->type() == CustomTextItem::Type)
        {
            CustomTextItem* item = qgraphicsitem_cast<CustomTextItem*>(p);
            item->setFont(myFont);
        }
    }
}

void CustomScene::deleteItems(QList<QGraphicsItem*> const& items)
{
    qDebug() << "delete items" << items;

    QList<QGraphicsItem*> customItems;
    foreach (QGraphicsItem *item, items)
    {
        if (item->type() == Arrow::Type)
        {
            removeItem(item);
            Arrow *arrow = qgraphicsitem_cast<Arrow *>(item);
            arrow->startItem()->removeArrow(arrow);
            arrow->endItem()->removeArrow(arrow);
            delete item;
        }
        else customItems.append(item);
    }

    foreach (QGraphicsItem *item, customItems)
    {
        if (item->type() == CustomItem::Type)
            qgraphicsitem_cast<CustomItem *>(item)->removeArrows();
        removeItem(item);
        delete item;
    }
}

void CustomScene::saveToXml(QDomDocument &doc, QDomElement &root)
{
    QDomElement sceneElement = doc.createElement("Scene");
    root.appendChild(sceneElement);

    foreach (QGraphicsItem *item, items())
    {
        if (CustomItem *customItem = qgraphicsitem_cast<CustomItem *>(item))
        {
            QDomElement itemElement = doc.createElement("CustomItem");
            itemElement.setAttribute("id", customItem->id());
            itemElement.setAttribute("type", customItem->customType());
            itemElement.setAttribute("x", customItem->pos().x());
            itemElement.setAttribute("y", customItem->pos().y());
            QPointF center = customItem->boundingRect().center();
            itemElement.setAttribute("centerX", center.x());
            itemElement.setAttribute("centerY", center.y());
            sceneElement.appendChild(itemElement);
        }
        else if (Arrow *arrow = qgraphicsitem_cast<Arrow *>(item))
        {
            QDomElement arrowElement = doc.createElement("Arrow");
            arrowElement.setAttribute("startItemId", arrow->startItem()->id());
            arrowElement.setAttribute("endItemId", arrow->endItem()->id());
            arrowElement.setAttribute("lineColor", arrow->getColor().name());
            QPointF intersectPoint = arrow->line().p1();
            arrowElement.setAttribute("intersectX", intersectPoint.x());
            arrowElement.setAttribute("intersectY", intersectPoint.y());
            sceneElement.appendChild(arrowElement);
        }
        else if (CustomTextItem *text = qgraphicsitem_cast<CustomTextItem *>(item))
        {
            QDomElement textElement = doc.createElement("Text");
            textElement.setAttribute("Name", text->getText());
            textElement.setAttribute("x", text->pos().x());
            textElement.setAttribute("y", text->pos().y());
            sceneElement.appendChild(textElement);
        }
    }
}

void CustomScene::loadFromXml(const QDomElement &root)
{
    QDomElement sceneElement = root.firstChildElement("Scene");
    QDomNodeList itemList = sceneElement.childNodes();

    // First pass: Create all CustomItems and store them in a map by their ID
    QMap<int, CustomItem*> itemMap;

    for (int i = 0; i < itemList.count(); i++)
    {
        QDomElement itemElement = itemList.at(i).toElement();
        QString tagName = itemElement.tagName();

        if (tagName == "CustomItem")
        {
            int id = itemElement.attribute("id").toInt();
            CustomItem::CustomType type = static_cast<CustomItem::CustomType>(itemElement.attribute("type").toInt());
            CustomItem* customItem = new CustomItem(type, myItemMenu);
            customItem->setId(id);
            customItem->setPos(itemElement.attribute("x").toFloat(), itemElement.attribute("y").toFloat());
            qreal centerX = itemElement.attribute("centerX").toFloat();
            qreal centerY = itemElement.attribute("centerY").toFloat();
            QPointF center(centerX, centerY);
            addItem(customItem);
            itemMap[id] = customItem;
        }
        else if (tagName == "CustomTextItem")
        {
            QString textName = itemElement.attribute("Name");
            qreal x = itemElement.attribute("x").toFloat();
            qreal y = itemElement.attribute("y").toFloat();

            CustomTextItem* textItem = new CustomTextItem();
            textItem->setText(textName);
            textItem->setPos(x, y);
            addItem(textItem);
        }
    }

    // Second pass: Create all Arrows using the previously created CustomItems
    for (int i = 0; i < itemList.count(); i++)
    {
        QDomElement itemElement = itemList.at(i).toElement();
        QString tagName = itemElement.tagName();

        if (tagName == "Arrow")
        {
            int startItemId = itemElement.attribute("startItemId").toInt();
            int endItemId = itemElement.attribute("endItemId").toInt();
            QColor color(itemElement.attribute("lineColor"));

            CustomItem* startItem = itemMap[startItemId];
            CustomItem* endItem = itemMap[endItemId];

            if (startItem && endItem)
            {
                Arrow* arrow = new Arrow(startItem, endItem);
                arrow->setColor(color);
                qreal intersectX = itemElement.attribute("intersectX").toFloat();
                qreal intersectY = itemElement.attribute("intersectY").toFloat();
                QPointF intersectPoint(intersectX, intersectY);

                addItem(arrow);
                startItem->addArrow(arrow);
                endItem->addArrow(arrow);
                arrow->updatePosition();
            }
        }
    }

}



void CustomScene::setMode(Mode mode)
{
    myMode = mode;
}

void CustomScene::setItemType(CustomItem::CustomType type)
{
    myItemType = type;
}


void CustomScene::editorLostFocus(CustomTextItem *item)
{
    QTextCursor cursor = item->textCursor();
    cursor.clearSelection();
    item->setTextCursor(cursor);

    if (item->toPlainText().isEmpty()) {
        removeItem(item);
        item->deleteLater();
    } else {
        if (item->contentIsUpdated()) {
            qDebug() << "content update ---";
            emit textChanged();
        }
    }

}

void CustomScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() != Qt::LeftButton)
        return;

    CustomItem *item;
    switch (myMode) {
    case InsertItem:
        item = new CustomItem(myItemType, myItemMenu);
        item->setBrush(myItemColor);
        addItem(item);
        item->setPos(mouseEvent->scenePos());
        qDebug() << "insert item at: " << mouseEvent->scenePos();
        qDebug() << "\ttype: " << myItemType << " color: " << myItemColor;
        emit itemInserted(item);
        hasItemSelected = itemAt(mouseEvent->scenePos(), QTransform()) != nullptr;
        break;

    case InsertLine:
        if (itemAt(mouseEvent->scenePos(), QTransform()) == nullptr) break;
        line = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(), mouseEvent->scenePos()));
        line->setPen(QPen(myLineColor, 2));
        addItem(line);
        break;

    case InsertText:
        textItem = new CustomTextItem();
        textItem->setFont(myFont);
        textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
        textItem->setZValue(1000.0);
        connect(textItem, SIGNAL(lostFocus(CustomTextItem*)), this, SLOT(editorLostFocus(DiagramTextItem*)));
        connect(textItem, SIGNAL(selectedChange(QGraphicsItem*)), this, SIGNAL(itemSelected(QGraphicsItem*)));
        addItem(textItem);
        textItem->setDefaultTextColor(myTextColor);
        textItem->setPos(mouseEvent->scenePos());
        emit textInserted(textItem);
        qDebug() << "text inserted at" << textItem->scenePos();
        break;

    default:
        hasItemSelected = itemAt(mouseEvent->scenePos(), QTransform()) != nullptr;
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}

void CustomScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (myMode == InsertLine && line != nullptr)
    {
        QLineF newLine(line->line().p1(), mouseEvent->scenePos());
        line->setLine(newLine);
    }
    else if (myMode == MoveItem)
    {
        if (hasItemSelected)
            mouseDraggingMoveEvent(mouseEvent);
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}

void CustomScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    hasItemSelected = false;

    horizontalStickyMode = false;
    verticalStickyMode = false;
    foreach(QGraphicsItem* p, selectedItems())
        p->setFlag(QGraphicsItem::ItemIsMovable);

    clearOrthogonalLines();
    if (line != nullptr && myMode == InsertLine)
    {
        QList<QGraphicsItem *> startItems = items(line->line().p1());
        if (startItems.count() && startItems.first() == line)
            startItems.removeFirst();
        QList<QGraphicsItem *> endItems = items(line->line().p2());
        if (endItems.count() && endItems.first() == line)
            endItems.removeFirst();

        removeItem(line);
        delete line;

        if (startItems.count() > 0 && endItems.count() > 0 &&
                startItems.first()->type() == CustomItem::Type &&
                endItems.first()->type() == CustomItem::Type &&
                startItems.first() != endItems.first())
        {
            CustomItem *startItem = qgraphicsitem_cast<CustomItem *>(startItems.first());
            CustomItem *endItem = qgraphicsitem_cast<CustomItem *>(endItems.first());
            Arrow *arrow = new Arrow(startItem, endItem);
            arrow->setColor(myLineColor);
            startItem->addArrow(arrow);
            endItem->addArrow(arrow);
            arrow->setZValue(-1000.0);
            addItem(arrow);
            arrow->updatePosition();
            emit arrowInserted();
        }
    }

    line = nullptr;
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void CustomScene::wheelEvent(QGraphicsSceneWheelEvent* wheelEvent)
{
    if ((wheelEvent->modifiers() & Qt::KeyboardModifier::ControlModifier) != 0)
    {
        emit scaleChanging(wheelEvent->delta());
        wheelEvent->accept();
    }
    else
    {
        QGraphicsScene::wheelEvent(wheelEvent);
    }
}

void CustomScene::mouseDraggingMoveEvent(QGraphicsSceneMouseEvent* event)
{
    clearOrthogonalLines();
    if ((event->buttons() & Qt::LeftButton) != 0 && selectedItems().size() == 1)
    {
        QGraphicsItem* itemUnderCursor = selectedItems().first();
        QPointF curCenter = itemUnderCursor->scenePos();
        QPointF const& mousePos = event->scenePos();

        foreach(QGraphicsItem* p, items())
        {
            if (p->type() != CustomItem::Type || p == itemUnderCursor) continue;

            CustomItem* item = qgraphicsitem_cast<CustomItem*>(p);
            QPointF const& objPoint = item->scenePos();
            LineAttr lineAttr;

            tryEnteringStickyMode(itemUnderCursor, objPoint, mousePos);

            if ((lineAttr = getPointsRelationship(objPoint, curCenter)) != Other)
            {
                if ((lineAttr & Horizontal) != 0) {
                    QGraphicsLineItem* newHLine = new QGraphicsLineItem();
                    newHLine->setLine(QLineF(QPointF(0, objPoint.y()),
                                             QPointF(sceneRect().width(), objPoint.y())));
                    newHLine->setPen(penForLines);
                    orthogonalLines.append(newHLine);
                }
                if ((lineAttr & Vertical) != 0)
                {
                    QGraphicsLineItem* newVLine = new QGraphicsLineItem();
                    newVLine->setLine(QLineF(QPointF(objPoint.x(), 0),
                                             QPointF(objPoint.x(), sceneRect().height())));
                    newVLine->setPen(penForLines);
                    orthogonalLines.append(newVLine);
                }
            }
        }
        tryLeavingStickyMode(itemUnderCursor, mousePos);
    }
    foreach(QGraphicsLineItem* p, orthogonalLines)
    {
        addItem(p);
    }
}

void CustomScene::clearOrthogonalLines()
{
    foreach(QGraphicsLineItem* p, orthogonalLines)
    {
        removeItem(p);
        delete p;
    }
    orthogonalLines.clear();
}

bool CustomScene::closeEnough(qreal x, qreal y, qreal delta)
{
    return std::abs(x - y) < delta;
}

CustomScene::LineAttr CustomScene::getPointsRelationship(const QPointF& p1, const QPointF& p2)
{
    int ret = Other;
    ret |= closeEnough(p1.x(), p2.x(), Delta) ? Vertical : Other;
    ret |= closeEnough(p1.y(), p2.y(), Delta) ? Horizontal : Other;
    return static_cast<CustomScene::LineAttr>(ret);
}

void CustomScene::tryEnteringStickyMode(QGraphicsItem* item, const QPointF& target, const QPointF& mousePos)
{
    QPointF const& itemPos = item->scenePos();
    if (!verticalStickyMode) {
        if (closeEnough(itemPos.x(), target.x(), stickyDistance))
        {  // enter stickyMode
            verticalStickyMode = true;
            verticalStickPoint = mousePos;
            item->setFlag(QGraphicsItem::ItemIsMovable, false);
            item->setPos(QPointF(target.x(), itemPos.y()));
        }
    }
    if (!horizontalStickyMode)
    {
        if (closeEnough(itemPos.y(), target.y(), stickyDistance))
        {
            horizontalStickyMode = true;
            horizontalStickPoint = mousePos;
            item->setFlag(QGraphicsItem::ItemIsMovable, false);
            item->setPos(QPointF(itemPos.x(), target.y()));
        }
    }
}

void CustomScene::tryLeavingStickyMode(QGraphicsItem* item, const QPointF& mousePos)
{
    if (verticalStickyMode)
    {
        item->moveBy(0, mousePos.y() - verticalStickPoint.y());
        verticalStickPoint.setY(mousePos.y());

        if (!closeEnough(mousePos.x(), verticalStickPoint.x(), stickyDistance))
        {
            verticalStickyMode = false;
            item->setFlag(QGraphicsItem::ItemIsMovable, true);
        }
    }
    if (horizontalStickyMode)
    {
        item->moveBy(mousePos.x() - horizontalStickPoint.x(), 0);
        horizontalStickPoint.setX(mousePos.x());

        if (!closeEnough(mousePos.y(), horizontalStickPoint.y(), stickyDistance))
        {
            horizontalStickyMode = false;
            item->setFlag(QGraphicsItem::ItemIsMovable, true);
        }
    }
}


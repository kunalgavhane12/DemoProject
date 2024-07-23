#ifndef CUSTOMVIEW_H
#define CUSTOMVIEW_H

#include <QGraphicsView>
#include <QKeyEvent>
#include <QDebug>

class CustomView: public QGraphicsView
{
    Q_OBJECT
public:
    CustomView(QGraphicsScene *scene, QWidget *parent = nullptr);
signals:
    void needsUndoBackUp();
protected:
    void keyPressEvent(QKeyEvent* event)override;
    void keyReleaseEvent(QKeyEvent* event)override;
    void mouseReleaseEvent(QMouseEvent* event)override;
};

#endif // CUSTOMVIEW_H

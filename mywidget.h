#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>
#include <QWheelEvent>

class MyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MyWidget(QWidget *parent = 0);

protected:
    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

private:
    QPoint mLastMousePosition;
    bool mMoving;
    
signals:

    
public slots:
    
};

#endif // MYWIDGET_H

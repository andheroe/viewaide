#ifndef MYLABEL_H
#define MYLABEL_H

#include <QLabel>
#include <QPainter>


class MyLabel : public QLabel
{
    Q_OBJECT
    public:
        MyLabel(QWidget *parent = 0);
    private:
        void paintEvent(QPaintEvent* event);
};

#endif // MYLABEL_H

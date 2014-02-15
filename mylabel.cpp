#include "mylabel.h"
#include <main_window.h>

MyLabel::MyLabel(QWidget *parent)
    : QLabel(parent)
{

}

void MyLabel::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);
    QPainter painter ( this );
    painter.setPen( QColor(0,0,0,150));
    int numb = Main_window::calibration_seconds;
    if ( numb >= 1 )
        painter.drawText( rect(), Qt::AlignCenter, QString::number(numb) );
    else
        painter.drawText( rect(), Qt::AlignCenter, "" );
}


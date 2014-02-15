#ifndef TABBAR_H
#define TABBAR_H
#include <QTabBar>
#include <QStylePainter>
#include <QStyleOptionTabV3>
#include <QPainter>
#include <QIcon>
#include <QString>

class TabBar : public QTabBar
{
public:

    explicit TabBar(QWidget* parent=0) : QTabBar(parent)
    {
        setIconSize(QSize(60, 60)); // 18 18
        setStyleSheet("QTabBar { font: 9pt }");
    }

protected:
    QSize tabSizeHint(int) const
    {
        return QSize(125, 100); // 125 100
    }

    void wheelEvent(QWheelEvent *)
    {
        if ( this->hasFocus() )
            return;
    }

    void paintEvent(QPaintEvent *)
    {
        QStylePainter p(this);
        for (int index = 0; index < count(); index++)
        {
            QStyleOptionTabV3 tab;
            initStyleOption(&tab, index);
            QIcon tempIcon = tab.icon;
            QString tempText = tab.text;
            tab.icon = QIcon();
            tab.text = QString();
            p.drawControl(QStyle::CE_TabBarTab, tab);
            QPainter painter;
            painter.begin(this);
            QRect tabrect = tabRect(index);
            tabrect.adjust(0, 8, 0, -8);
            painter.drawText(tabrect, Qt::AlignBottom | Qt::AlignHCenter, tempText);
            tempIcon.paint(&painter, tabrect, Qt::AlignTop | Qt::AlignHCenter);
            painter.end();
        }
    }
};

class TabWidget : public QTabWidget
{
public:
    explicit TabWidget(QWidget *parent = 0) : QTabWidget(parent)
    {
        setTabBar(new TabBar());
    }
};

#endif //TABBAR_H

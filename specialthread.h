#ifndef SPECIAL_THREAD_H
#define SPECIAL_THREAD_H

#include <QThread>
#include "main_window.h"

class SpecialThread : public QThread
{
private:
    Main_window* main_w;
public:
    SpecialThread( Main_window* m_w );
    void run();
};

#endif // SPECIAL_THREAD_H

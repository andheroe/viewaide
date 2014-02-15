#ifndef SPECIAL_THREAD_H
#define SPECIAL_THREAD_H

#include <QThread>
#include "main_window.h"

class Special_thread : public QThread
{
private:
    Main_window* main_w;
public:
    Special_thread( Main_window* m_w );
    void run();
};

#endif // SPECIAL_THREAD_H

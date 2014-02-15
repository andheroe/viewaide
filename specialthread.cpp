#include "specialthread.h"

SpecialThread::SpecialThread( Main_window* m_w )
{
    main_w = m_w;
}

void SpecialThread::run()
{
    main_w->slotMakePause();
}

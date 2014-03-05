#include "specialthread.h"

SpecialThread::SpecialThread( Main_window* m_w )
{
    main_w = m_w;
}

void SpecialThread::run()
{
    if ( !main_w->block_popup_msg)
        main_w->slotMakePause();
}

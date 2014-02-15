#include "special_thread.h"

Special_thread::Special_thread( Main_window* m_w )
{
    main_w = m_w;
}

void Special_thread::run()
{
    main_w->slotShowCircle();
}

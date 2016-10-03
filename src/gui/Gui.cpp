#include "Gui.hpp"

namespace gui
{

void sleep(unsigned long miliseconds)
{
#ifdef __WIN32
  Sleep(miliseconds);
#else
  timespec pause;
  pause.tv_sec = 0;
  pause.tv_nsec = miliseconds * 1000000;
  nanosleep(&pause, 0);
#endif
}

}

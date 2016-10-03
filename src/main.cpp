#include <allegro.h>
#ifdef __WIN32
  #include <winalleg.h>
#endif
#include "GuiManager.hpp"

int main()
{
#ifdef __WIN32
  timeBeginPeriod(1);
#endif

  gui::GuiManager::getInstance()->start();

#ifdef __WIN32
  timeEndPeriod(1);
#endif

  return 0;
}
END_OF_MAIN()


#include <allegro.h>
#ifdef __WIN32
  #include <winalleg.h>
#endif

#include "GuiManager.hpp"


int main()
{
#ifdef __WIN32
        timeBeginPeriod( 1 );
#endif

        gui::GuiManager::getInstance()->begin ();

#ifdef __WIN32
        timeEndPeriod( 1 );
#endif

        return EXIT_SUCCESS;
}
END_OF_MAIN()

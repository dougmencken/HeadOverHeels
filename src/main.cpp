
#include "Ism.hpp"
#include "GuiManager.hpp"


int main( int argc, char** argv )
{
        if ( argc > 0 ) isomot::setPathToGame( argv[ 0 ] );

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

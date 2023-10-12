
#include "screen.hpp"

#include "WrappersAllegro.hpp"

#include <iostream>

namespace variables
{
        unsigned int screenWidth = 640 ;

        unsigned int screenHeight = 480 ;

        unsigned int getScreenWidth() {  return screenWidth ;  }

        unsigned int getScreenHeight() {  return screenHeight ;  }

        void setScreenWidth( unsigned int width )
        {
                if ( width < 640 ) width = 640;
                screenWidth = width;
        }

        void setScreenHeight( unsigned int height )
        {
                if ( height < 480 ) height = 480;
                screenHeight = height;
        }
}

bool allegroWindowSizeToScreenSize ()
{
        bool switched = allegro::switchToWindowedVideo( variables::getScreenWidth(), variables::getScreenHeight() ) ;
        if ( ! switched ) {
                std::cout << "can’t change the size of game's window to "
                                << variables::getScreenWidth() << " x " << variables::getScreenHeight() << std::endl ;
        }

        allegro::Pict::theScreen().clearToColor( AllegroColor::makeColor( 0, 0, 0, 0xff ) );
        allegro::update ();

        return switched ;
}


#include "screen.hpp"

#include "WrappersAllegro.hpp"

#include <iostream>


namespace variables
{
        unsigned int screenWidth = constants::Default_Screen_Width ;

        unsigned int screenHeight = constants::Default_Screen_Height ;

        bool keepWidth = false ;

        bool keepHeight = false ;

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

        void keepThisWidth ( bool keep ) {  keepWidth = keep ;  }

        void keepThisHeight ( bool keep ) {  keepHeight = keep ;  }

        bool isWidthKept () {  return keepWidth ;  }

        bool isHeightKept () {  return keepHeight ;  }
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


#include "screen.hpp"

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

// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef screen_hpp_
#define screen_hpp_

namespace constants
{
        static const unsigned int  Smallest_Screen_Width = 640 ;

        static const unsigned int Smallest_Screen_Height = 480 ;

        static const unsigned int   Default_Screen_Width = Smallest_Screen_Width ;

        static const unsigned int  Default_Screen_Height = Smallest_Screen_Height ;
}

namespace variables
{

        unsigned int getScreenWidth () ;

        void setScreenWidth ( unsigned int w ) ;

        unsigned int getScreenHeight () ;

        void setScreenHeight ( unsigned int h ) ;

        void keepThisWidth ( bool keep ) ;

        void keepThisHeight ( bool keep ) ;

        bool isWidthKept () ;

        bool isHeightKept () ;

}

bool allegroWindowSizeToScreenSize () ;

#endif

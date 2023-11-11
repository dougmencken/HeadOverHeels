// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef GamePreferences_hpp_
#define GamePreferences_hpp_

#include <string>


class GamePreferences
{

public:

        /**
         * read the preferences from XML file
         */
        static bool readPreferences ( const std::string & fileName ) ;

        static bool writePreferences ( const std::string & fileName ) ;

        static bool allegroWindowSizeToScreenSize () ;

        static unsigned int getScreenWidth() {  return screenWidth ;  }

        static unsigned int getScreenHeight() {  return screenHeight ;  }

        static void setScreenWidth( unsigned int width )
        {
                if ( width < Smallest_Screen_Width )
                        width = Smallest_Screen_Width ;

                screenWidth = width;
        }

        static void setScreenHeight( unsigned int height )
        {
                if ( height < Smallest_Screen_Height )
                        height = Smallest_Screen_Height ;

                screenHeight = height;
        }

        static void keepThisWidth ( bool keep ) {  keepWidth = keep ;  }

        static void keepThisHeight ( bool keep ) {  keepHeight = keep ;  }

        static bool isWidthKept () {  return keepWidth ;  }

        static bool isHeightKept () {  return keepHeight ;  }

        /* constants */

        static const unsigned int  Smallest_Screen_Width = 640 ;
        static const unsigned int Smallest_Screen_Height = 480 ;

        static const unsigned int   Default_Screen_Width = Smallest_Screen_Width ;
        static const unsigned int  Default_Screen_Height = Smallest_Screen_Height ;

private:

        /* variables */

        static unsigned int screenWidth ;
        static unsigned int screenHeight ;

        static bool keepWidth ;
        static bool keepHeight ;

        // no any instance creation for now
        GamePreferences ( ) { }

} ;

#endif

// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Color_hpp_
#define Color_hpp_

#include "WrappersAllegro.hpp"

#include "Picture.hpp"


/**
 * Describes color as red, green and blue components,
 * and contains utilities to deal with colors
 */

class Color
{

public:

        Color( ) : red( 255 ), green( 0 ), blue( 255 ) { }

        Color( unsigned char r, unsigned char g, unsigned char b ) : red( r ), green( g ), blue( b ) { }

        Color( const Color& copy ) : red( copy.red ), green( copy.green ), blue( copy.blue ) { }

        unsigned char getRed () const {  return red ;  }

        unsigned char getGreen () const {  return green ;  }

        unsigned char getBlue () const {  return blue ;  }

        bool operator == ( const Color & c ) const {  return c.red == red && c.green == green && c.blue == blue ;  }

        bool operator != ( const Color & c ) const {  return c.red != red || c.green != green || c.blue != blue ;  }

        int toAllegroColor () const {  return allegro::makeColor( red, green, blue ) ;  }

        std::string toString () ;

        /*
         * True when it’s color with red=255 green=0 blue=255
         */
        static bool isKeyColor ( unsigned char red, unsigned char green, unsigned char blue )
        {
                return red == 255 && green == 0 && blue == 255 ;
        }

        static bool isKeyColor ( int allegroColor )
        {
                return  allegro::getRed( allegroColor ) == 255  &&
                        allegro::getGreen( allegroColor ) == 0  &&
                        allegro::getBlue( allegroColor ) == 255 ;
        }

        static const Color & blackColor () {  return *theBlack ;  }                     /* speccy color 0 */

        static const Color & blueColor () {  return *theBlue ;  }                       /* speccy color 1 */

        static const Color & redColor () {  return *theRed ;  }                         /* speccy color 2 */

        static const Color & magentaColor () {  return *theMagenta ;  }                 /* speccy color 3 */

        static const Color & greenColor () {  return *theGreen ;  }                     /* speccy color 4 */

        static const Color & cyanColor () {  return *theCyan ;  }                       /* speccy color 5 */

        static const Color & yellowColor () {  return *theYellow ;  }                   /* speccy color 6 */

        static const Color & whiteColor () {  return *theWhite ;  }                     /* speccy color 7 */

        static const Color & darkBlueColor () {  return *theDarkBlue ;  }

        static const Color & orangeColor () {  return *theOrange ;  }

        static const Color & gray50Color () {  return *the50Gray ;  }                   /* 50% gray */

        static const Color & colorOfTransparency () {  return *theTransparency ;  }     /* “ key ” color of transparency, pure magenta */

        static Picture * colorizePicture ( Picture * picture, const Color & color ) ;

        static Picture * multiplyWithColor ( Picture * picture, const Color & color ) ;

        static Picture * pictureToGrayscale ( Picture * picture ) ;

protected:

        unsigned char red ;

        unsigned char green ;

        unsigned char blue ;

private:

        static Picture * colorizePicture ( Picture * picture, unsigned char red, unsigned char green, unsigned char blue ) ;

        static Picture * multiplyWithColor ( Picture * picture, unsigned char red, unsigned char green, unsigned char blue ) ;

        static Color * theBlack ;

        static Color * theDarkBlue ;

        static Color * theBlue ;

        static Color * theRed ;

        static Color * theMagenta ;

        static Color * theGreen ;

        static Color * theCyan ;

        static Color * theYellow ;

        static Color * theWhite ;

        static Color * theOrange ;

        static Color * the50Gray ;

        static Color * theTransparency ;

} ;


#endif

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
 * Describes color as red, green, blue, and alpha components,
 * and contains utilities to deal with colors
 */

class Color
{

public:

        /* default constructor makes transparent color */
        Color( ) ;

        Color( unsigned char r, unsigned char g, unsigned char b, unsigned char a ) : red( r ), green( g ), blue( b ), alpha( a ) { }

        Color( const AllegroColor& color ) : red( color.getRed() ), green( color.getGreen() ), blue( color.getBlue() ), alpha( color.getAlpha() ) { }

        Color( const Color& copy ) : red( copy.red ), green( copy.green ), blue( copy.blue ), alpha( copy.alpha ) { }

        unsigned char getRed () const {  return red ;  }

        unsigned char getGreen () const {  return green ;  }

        unsigned char getBlue () const {  return blue ;  }

        unsigned char getAlpha () const {  return alpha ;  }

        bool operator == ( const Color & c ) const {  return c.red == red && c.green == green && c.blue == blue && c.alpha == alpha ;  }

        bool operator != ( const Color & c ) const {  return c.red != red || c.green != green || c.blue != blue || c.alpha != alpha ;  }

        AllegroColor toAllegroColor () const {  return AllegroColor::makeColor( red, green, blue, alpha ) ;  }

        std::string toString () const ;

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

        static void colorizePicture ( Picture * picture, const Color & color ) ;

        static void multiplyWithColor ( Picture * picture, const Color & color ) ;

        static void pictureToGrayscale ( Picture * picture ) ;

protected:

        unsigned char red ;

        unsigned char green ;

        unsigned char blue ;

        unsigned char alpha ;

private:

        static void multiplyWithColor ( Picture * picture, unsigned char red, unsigned char green, unsigned char blue ) ;

        static const Color * theBlack ;

        static const Color * theDarkBlue ;

        static const Color * theBlue ;

        static const Color * theRed ;

        static const Color * theMagenta ;

        static const Color * theGreen ;

        static const Color * theCyan ;

        static const Color * theYellow ;

        static const Color * theWhite ;

        static const Color * theOrange ;

        static const Color * the50Gray ;

} ;


#endif

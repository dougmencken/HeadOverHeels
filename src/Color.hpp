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

#include <allegro.h>


/**
 * Describes color as red, green and blue components,
 * and contains utilities to deal with colors
 */

class Color
{

protected:

        Color( unsigned char r, unsigned char g, unsigned char b ) ;

public:

        unsigned char getRed () const {  return red ;  }

        unsigned char getGreen () const {  return green ;  }

        unsigned char getBlue () const {  return blue ;  }

        bool operator == ( const Color * c ) {  return c->red == red && c->green == green && c->blue == blue ;  }

        bool operator != ( const Color * c ) {  return c->red != red || c->green != green || c->blue != blue ;  }

        int toAllegroColor () const {  return makecol( red, green, blue ) ;  }

        static Color * blackColor () {  return theBlack ;  }                    /* speccy color 0 */

        static Color * blueColor () {  return theBlue ;  }                      /* speccy color 1 */

        static Color * redColor () {  return theRed ;  }                        /* speccy color 2 */

        static Color * magentaColor () {  return theMagenta ;  }                /* speccy color 3 */

        static Color * greenColor () {  return theGreen ;  }                    /* speccy color 4 */

        static Color * cyanColor () {  return theCyan ;  }                      /* speccy color 5 */

        static Color * yellowColor () {  return theYellow ;  }                  /* speccy color 6 */

        static Color * whiteColor () {  return theWhite ;  }                    /* speccy color 7 */

        static Color * darkBlueColor () {  return theDarkBlue ;  }

        static Color * orangeColor () {  return theOrange ;  }

        static Color * gray50Color () {  return the50Gray ;  }                  /* 50% gray */

        static Color * colorOfTransparency () {  return theTransparency ;  }    /* “ key ” color of transparency, pure magenta */

        static BITMAP * colorizePicture ( BITMAP * picture, const Color * color ) ;

        static BITMAP * multiplyWithColor ( BITMAP * picture, const Color * color ) ;

        static BITMAP * pictureToGrayscale ( BITMAP * picture ) ;

private:

        unsigned char red ;

        unsigned char green ;

        unsigned char blue ;

        static BITMAP * colorizePicture ( BITMAP * picture, unsigned char red, unsigned char green, unsigned char blue ) ;

        static BITMAP * multiplyWithColor ( BITMAP * picture, unsigned char red, unsigned char green, unsigned char blue ) ;

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

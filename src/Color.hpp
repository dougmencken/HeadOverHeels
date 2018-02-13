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

        unsigned char getRed () {  return red ;  }

        unsigned char getGreen () {  return green ;  }

        unsigned char getBlue () {  return blue ;  }

        bool operator == ( const Color * c ) {  return c->red == red && c->green == green && c->blue == blue ;  }

        bool operator != ( const Color * c ) {  return c->red != red || c->green != green || c->blue != blue ;  }

        int toAllegroColor () {  return makecol( red, green, blue ) ;  }

        static Color * blackColor () {  return new Color( 0, 0, 0 ) ;  }                /* speccy color 0 */

        static Color * blueColor () {  return new Color( 50, 50, 255 ) ;  }             /* speccy color 1 */

        static Color * redColor () {  return new Color( 255, 50, 50 ) ;  }              /* speccy color 2 */

        static Color * magentaColor () {  return new Color( 255, 50, 255 ) ;  }         /* speccy color 3 */

        static Color * greenColor () {  return new Color( 50, 255, 50 ) ;  }            /* speccy color 4 */

        static Color * cyanColor () {  return new Color( 0, 228, 231 ) ;  }             /* speccy color 5 */

        static Color * yellowColor () {  return new Color( 255, 255, 50 ) ;  }          /* speccy color 6 */

        static Color * whiteColor () {  return new Color( 255, 255, 255 ) ;  }          /* speccy color 7 */

        static Color * orangeColor () {  return new Color( 239, 129, 0 ) ;  }

        static Color * gray50Color () {  return new Color( 127, 127, 127 ) ;  }         /* 50% gray */

        static Color * colorOfTransparency () {  return new Color( 255, 0, 255 ) ;  }   /* “ key ” color of transparency, pure magenta */

        static BITMAP * colorizePicture ( BITMAP * picture, Color * color ) ;

        static BITMAP * pictureToGrayscale ( BITMAP * picture ) ;

private:

        unsigned char red ;

        unsigned char green ;

        unsigned char blue ;

        static BITMAP * colorizePicture ( BITMAP * picture, unsigned char red, unsigned char green, unsigned char blue ) ;

} ;


#endif

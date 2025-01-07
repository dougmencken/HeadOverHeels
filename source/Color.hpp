// The free and open source remake of Head over Heels
//
// Copyright © 2025 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Color_hpp_
#define Color_hpp_

#include "WrappersAllegro.hpp"


/**
 * Describes color as red, green, blue, and alpha components,
 * and contains utilities to deal with colors
 */

class Color
{

public:

        Color( unsigned char r, unsigned char g, unsigned char b, unsigned char a ) : red( r ), green( g ), blue( b ), alpha( a ) { }

        Color( const AllegroColor& color ) : red( color.getRed() ), green( color.getGreen() ), blue( color.getBlue() ), alpha( color.getAlpha() ) { }

        Color( const Color& copy ) : red( copy.red ), green( copy.green ), blue( copy.blue ), alpha( copy.alpha ) { }

        virtual ~Color( ) { }

        virtual unsigned char   getRed () const {  return red ;  }
        virtual unsigned char getGreen () const {  return green ;  }
        virtual unsigned char  getBlue () const {  return blue ;  }
        virtual unsigned char getAlpha () const {  return alpha ;  }

        Color add ( const Color & c ) const ;
        Color operator + ( const Color & c ) const {  return add( c ) ;  }

        Color subtract ( const Color & c ) const ;
        Color operator - ( const Color & c ) const {  return subtract( c ) ;  }

        Color multiply ( const Color & c ) const ;

        Color withAlteredAlpha ( unsigned char newAlpha ) const ;

        bool equals ( const Color & c ) const {  return c.red == red && c.green == green && c.blue == blue && c.alpha == alpha ;  }

        bool operator == ( const Color & c ) const {  return   equals( c );  }
        bool operator != ( const Color & c ) const {  return ! equals( c );  }

        bool equalsRGBA ( const Color & c ) const {  return equals( c ) ;  }

        // ignoring alpha
        bool equalsRGB ( const Color & c ) const {  return c.red == red && c.green == green && c.blue == blue ;  }

        bool isFullyTransparent () const {  return toAllegroColor().isKeyColor() ;  }

        virtual AllegroColor toAllegroColor () const {  return AllegroColor::makeColor( red, green, blue, alpha ) ;  }

        virtual std::string toString () const ;

        static const Color & byName ( const std::string & color ) ;

        static const Color & blackColor () {  return theBlack ;  }

        static const Color & whiteColor () {  return theWhite ;  }

        static const Color & keyColor () {  return transparency ;  }

protected:

        unsigned char red ;

        unsigned char green ;

        unsigned char blue ;

        unsigned char alpha ;

private:

        static const Color transparency ;

        static const Color theWhite ;

        static const Color theBlack ;

        static const Color theBlue ;
        static const Color theRed ;
        static const Color theMagenta ;
        static const Color theGreen ;
        static const Color theCyan ;
        static const Color theYellow ;

        static const Color theDarkBlue ;

        static const Color theOrange ;

        static const Color theGray50 ;             // 50% gray
        static const Color theGray75white ;        // gray 75% white 25% black
        static const Color theGray25white ;        // gray 25% white 75% black

        static const Color theReducedWhite ;

        static const Color theReducedBlue ;
        static const Color theReducedRed ;
        static const Color theReducedMagenta ;
        static const Color theReducedGreen ;
        static const Color theReducedCyan ;
        static const Color theReducedYellow ;

        static const Color theLightBlue ;
        static const Color theLightRed ;
        static const Color theLightMagenta ;
        static const Color theLightGreen ;
        static const Color theLightCyan ;
        static const Color theLightYellow ;

} ;


#endif

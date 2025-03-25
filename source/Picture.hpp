// The free and open source remake of Head over Heels
//
// Copyright © 2025 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Picture_hpp_
#define Picture_hpp_

#include <string>

#include "pointers.hpp"

#include "WrappersAllegro.hpp"
#include "Color.hpp"


/**
 * Describes a picture
 */

class Picture
{

public:

        Picture( unsigned int width, unsigned int height ) ;

        Picture( unsigned int width, unsigned int height, const Color & color ) ;

        Picture( const std::string & path, const std::string & name ) /* throws NoSuchPictureException */ ;

        Picture( const allegro::Pict & pict )
                : apicture( allegro::Pict::asCloneOf( pict.ptr() ) )
        {}

        Picture( const Picture & pic ) // the copy constructor
                : apicture( allegro::Pict::asCloneOf( pic.getAllegroPict().ptr() ) )
        {}

        virtual ~Picture( ) {}

        unsigned int getWidth () const {  return this->apicture->getW() ;  }

        unsigned int getHeight () const {  return this->apicture->getH() ;  }

        Color getPixelAt ( int x, int y ) const {  return Color( this->apicture->getPixelAt( x, y ) ) ;  }

        void putPixelAt ( int x, int y, const Color & color ) const ;
        void drawPixelAt ( int x, int y, const Color & color ) const ;

        const allegro::Pict & getAllegroPict () const {  return *this->apicture ;  }

        void fillWithColor ( const Color & color ) ;

        void fillWithTransparencyChequerboard ( const unsigned int sizeOfSquare = 8 ) ;

        void colorizeWhite ( const Color & color ) {  changeWhiteToColor( color ) ;  }
        void changeWhiteToColor ( const Color & color ) {  replaceColor( Color::whiteColor(), color ) ;  }

        void colorizeBlack ( const Color & color ) {  changeBlackToColor( color ) ;  }
        void changeBlackToColor ( const Color & color ) {  replaceColor( Color::blackColor(), color ) ;  }

        void replaceColor ( const Color & from, const Color & to ) ;

        void replaceColorAnyAlpha ( const Color & from, const Color & to ) ;

        void toGrayscale () ;

        void expandOrCropTo ( unsigned int width, unsigned int height ) ;

        void flipHorizontal () ;
        void flipVertical () ;

        void rotate90 () ;
        void rotate270 () ;

        void rotate90clockwise () {  rotate270 () ;  }
        void rotate90counterclockwise () {  rotate90 () ;  }

        void multiplyWithColor ( const Color & multiplier ) ;

        void changeAlpha ( unsigned char newAlpha ) ;

        void invertColors () ;

        static Picture * summation ( const Picture & first, const Picture & second ) ;

        static Picture * difference ( const Picture & first, const Picture & second ) ;

        void saveAsPCX ( const std::string & path, const std::string & name ) ;

        void saveAsPNG ( const std::string & path, const std::string & name ) ;

        static std::vector < allegro::Pict * > loadAnimation ( const std::string & path, const std::string & name ) ;

private:

        multiptr < allegro::Pict > apicture ;

} ;

typedef multiptr < Picture > PicturePtr ;


#endif

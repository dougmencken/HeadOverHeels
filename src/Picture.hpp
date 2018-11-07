// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Picture_hpp_
#define Picture_hpp_

#include <string>

#include "pointers.hpp"

#include "WrappersAllegro.hpp"

class Color ;


/**
 * Describes a picture
 */

class Picture
{

public:

        Picture( unsigned int width, unsigned int height ) ;

        Picture( unsigned int width, unsigned int height, const Color & color ) ;

        Picture( const allegro::Pict& pict ) ;

        Picture( const Picture& pic ) ;  // copy constructor

        virtual ~Picture( ) ;

        const std::string& getName () const {  return name ;  }

        void setName( const std::string& newName ) ;

        unsigned int getWidth () const {  return picture->getW() ;  }

        unsigned int getHeight () const {  return picture->getH() ;  }

        AllegroColor getPixelAt ( int x, int y ) const {  return picture->getPixelAt( x, y ) ;  }

        void putPixelAt ( int x, int y, const Color& color ) const ;

        void drawPixelAt ( int x, int y, const Color& color ) const ;

        const allegro::Pict& getAllegroPict () const {  return *picture ;  }

        void fillWithColor ( const Color & color ) ;

        void colorize ( const Color & color ) ;

        void toGrayscale () ;

        void saveAsPCX ( const std::string& path ) ;

        void saveAsPNG ( const std::string& path ) ;

private:

        justptr < allegro::Pict > picture ;

        std::string name ;

} ;

typedef safeptr < Picture > PicturePtr ;


#endif

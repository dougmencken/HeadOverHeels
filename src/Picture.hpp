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

#include "WrappersAllegro.hpp"

class Color ;


/**
 * Describes a picture
 */

class Picture
{

public:

        Picture( unsigned int width, unsigned int height ) ;

        Picture( unsigned int width, unsigned int height, Color * color ) ;

        Picture( allegro::Pict * pict ) ;

        Picture( const allegro::Pict& pict ) ;

        Picture( const Picture& pic ) ;  // copy constructor

        virtual ~Picture( ) ;

        const std::string& getName () const {  return name ;  }

        void setName( const std::string& newName ) {  name = newName ;  }

        unsigned int getWidth () const {  return picture->w ;  }

        unsigned int getHeight () const {  return picture->h ;  }

        unsigned int getColorDepth () const {  return allegro::colorDepthOf( picture ) ;  }

        allegro::Pict * getAllegroPict () const {  return picture ;  }

        void fillWithColor ( Color * color ) ;

        void colorize ( Color * color ) ;

        Picture * makeGrayscaleCopy () ;

        Picture * makeColorizedCopy ( Color * color ) ;

private:

        allegro::Pict * picture ;

        std::string name ;

} ;


#endif

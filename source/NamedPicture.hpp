// The free and open source remake of Head over Heels
//
// Copyright © 2026 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef NamedPicture_hpp_
#define NamedPicture_hpp_

#include "Picture.hpp"

#include "util.hpp"

#ifdef DEBUG
#  define DEBUG_PICTURES        0
#  include <iostream>
#endif

/**
 * A picture with name
 */

class NamedPicture : public Picture
{

public:

        NamedPicture( unsigned int width, unsigned int height )
                : Picture( width, height )
                , name( "picture-" + util::makeRandomString( 12 ) )
        {}

        NamedPicture( unsigned int width, unsigned int height, const Color & color )
                : Picture( width, height, color )
                , name( "Picture." + util::makeRandomString( 12 ) )
        {}

        NamedPicture( const std::string & path, const std::string & fileName )
                : Picture( path, fileName )
                , name( fileName )
        {}

        NamedPicture( const allegro::Pict & pict )
                : Picture( pict )
                , name( "Picture." + util::makeRandomString( 12 ) )
        {}

        NamedPicture( const Picture & that )
                : Picture( that )
                , name( "copy-" + util::makeRandomString( 12 ) )
        {}

        // the copy constructor
        NamedPicture( const NamedPicture & that )
                : Picture( that )
                , name( "copy of " + that.name )
        {}

        virtual ~NamedPicture( )
        {
        # if defined( DEBUG_PICTURES )  &&  DEBUG_PICTURES
                std::cout << "bye bye " << getName() << std::endl ;
        # endif
        }

        const std::string & getName () const {  return this->name ;  }

        void setName( const std::string & newName )
        {
        # if defined( DEBUG_PICTURES )  &&  DEBUG_PICTURES
                std::cout << "rename picture \"" << getName() << "\" to \"" << newName << "\"" << std::endl ;
        # endif
                this->name = newName ;
        }

        void saveAsPCX ( const std::string & path ) {  Picture::saveAsPCX( path, getName() ) ;  }
        void saveAsPNG ( const std::string & path ) {  Picture::saveAsPNG( path, getName() ) ;  }

private:

        std::string name ;

} ;

typedef multiptr < NamedPicture > NamedPicturePtr ;

#endif

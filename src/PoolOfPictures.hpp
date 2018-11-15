// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef PoolOfPictures_hpp_
#define PoolOfPictures_hpp_

#include <map>
#include <string>

#include "Picture.hpp"


namespace iso
{

class PoolOfPictures
{

public:

        PoolOfPictures( ) { }

        virtual ~PoolOfPictures( ) {  clear() ;  }

        void clear () {  pictures.clear() ;  }

        PicturePtr getPicture ( const std::string & imageFile ) const
        {
                std::map< std::string, PicturePtr >::const_iterator pi = pictures.find( imageFile ) ;
                return ( pi != pictures.end () ) ? ( *pi ).second : PicturePtr () ;
        }

        PicturePtr getOrLoadAndGet ( const std::string & imageFile ) ;

        PicturePtr getOrLoadAndGetOrMakeAndGet ( const std::string & imageFile, unsigned int imageWidth, unsigned int imageHeight ) ;

private:

        std::map < std::string /* imageFile */, PicturePtr /* image */ > pictures ;

} ;

}

#endif

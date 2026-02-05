// The free and open source remake of Head over Heels
//
// Copyright © 2026 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef PoolOfPictures_hpp_
#define PoolOfPictures_hpp_

#include <map>
#include <string>

#include "NamedPicture.hpp"


/**
 * A storage of pictures that are read from files once and can be used later whenever needed
 */

class PoolOfPictures
{

public:

        PoolOfPictures( ) { }

        virtual ~PoolOfPictures( ) {  clear() ;  }

        void clear () {  pictures.clear() ;  }

        bool hasPicture ( const std::string & imageFile ) const ;

        NamedPicturePtr getPicture ( const std::string & imageFile ) const ;

        NamedPicturePtr getOrLoadAndGet ( const std::string & imageFile ) ;

        NamedPicturePtr getOrLoadAndGetOrMakeAndGet ( const std::string & imageFile, unsigned int imageWidth, unsigned int imageHeight ) ;

        NamedPicturePtr makePicture ( const std::string & imageFile, unsigned int imageWidth, unsigned int imageHeight ) ;

        void putPicture ( const std::string & imageFile, const NamedPicturePtr & picture ) ;

        static bool isPictureThere ( const std::string & imageFile ) ;

        static PoolOfPictures & getPoolOfPictures () {  return * thePoolOfPictures ;  }

private:

        std::map < std::string /* imageFile */, NamedPicturePtr /* image */ > pictures ;

        static PoolOfPictures * thePoolOfPictures ;

} ;

#endif

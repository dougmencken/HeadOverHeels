// The free and open source remake of Head over Heels
//
// Copyright © 2025 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef version_hpp_
#define version_hpp_

#include <string>

inline std::string gameVersion ()
{
# ifdef PACKAGE_VERSION
        std::string version( PACKAGE_VERSION );
# else
        std::string version( "none" );
# endif
# ifdef GIT_HEAD_HASH
        version += "-" + std::string( GIT_HEAD_HASH ) ;
# endif

        return version ;
}

#endif

// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef GamePreferences_hpp_
#define GamePreferences_hpp_

#include <string>

namespace iso
{

class GamePreferences
{

public:

        static bool readPreferences ( const std::string & fileName ) ;

        static bool writePreferences ( const std::string & fileName ) ;

private:

        // no any instance creation for now
        GamePreferences ( ) { }

} ;

}

#endif

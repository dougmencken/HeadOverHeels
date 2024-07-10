// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef KeyHandler_hpp_
#define KeyHandler_hpp_

#include <string>


namespace gui
{

class KeyHandler
{

public:

        /**
         * Subclasses may handle this key or pass it to some other handler
         */
        virtual void handleKey ( const std::string & key ) = 0 ;

} ;

}

#endif

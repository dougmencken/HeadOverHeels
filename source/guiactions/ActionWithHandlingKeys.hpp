// The free and open source remake of Head over Heels
//
// Copyright © 2025 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef ActionWithHandlingKeys_hpp_
#define ActionWithHandlingKeys_hpp_

#include "Action.hpp"
#include "KeyHandler.hpp"


namespace gui
{

class ActionWithHandlingKeys : public Action, public KeyHandler
{

public:

        ActionWithHandlingKeys( ) : Action(), KeyHandler() {}

        virtual ~ActionWithHandlingKeys( ) {}

        virtual void handleKey ( const std::string & key ) = 0 ;

protected:

        virtual void act () = 0 ;

} ;

}

#endif

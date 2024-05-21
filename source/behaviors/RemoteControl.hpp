// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef RemoteControl_hpp_
#define RemoteControl_hpp_

#include "Behavior.hpp"
#include "Timer.hpp"

class FreeItem ;


namespace behaviors
{

/**
 * Models both the controller and the controlled item
 */

class RemoteControl : public Behavior
{

public:

        RemoteControl( FreeItem & item, const std::string & behavior ) ;

        virtual ~RemoteControl( ) {}

        virtual bool update () ;

private:

        autouniqueptr < Timer > speedTimer ;

        autouniqueptr < Timer > fallTimer ;

};

}

#endif

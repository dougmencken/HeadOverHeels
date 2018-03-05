// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef RemoteControl_hpp_
#define RemoteControl_hpp_

#include "Behavior.hpp"
#include "Timer.hpp"


namespace isomot
{

class Item ;
class FreeItem ;

/**
 * A pair of controller & controlled item
 */

class RemoteControl : public Behavior
{

public:

        RemoteControl( Item * item, const std::string & behavior ) ;

        virtual ~RemoteControl( ) ;

        virtual bool update () ;

private:

       /**
        * Item to move by remote control
        */
        FreeItem * controlledItem ;

        Timer * speedTimer ;

        Timer * fallTimer ;

};

}

#endif

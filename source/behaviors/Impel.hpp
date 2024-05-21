// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Impel_hpp_
#define Impel_hpp_

#include "Behavior.hpp"
#include "Timer.hpp"

class FreeItem ;


namespace behaviors
{

/**
 * Moves an item in one direction when pushed by another item. Stops when it hits something
 */

class Impel : public Behavior
{

public:

        Impel( FreeItem & item, const std::string & behavior ) ;

        virtual ~Impel( ) {}

        virtual bool update () ;

private:

        // timer for the motion speed
        autouniqueptr < Timer > speedTimer ;

        // timer for the speed of falling
        autouniqueptr < Timer > fallTimer ;

};

}

#endif

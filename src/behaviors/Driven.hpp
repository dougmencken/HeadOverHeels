// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Driven_hpp_
#define Driven_hpp_

#include "Behavior.hpp"
#include "Timer.hpp"


namespace iso
{

/**
 * Move item in the direction set by item above it
 */

class Driven : public Behavior
{

public:

        Driven( const ItemPtr & item, const std::string & behavior ) ;

        virtual ~Driven( ) ;

        virtual bool update () ;

private:

        bool moving ;

       /**
        * Timer for speed of movement
        */
        autouniqueptr < Timer > speedTimer ;

       /**
        * Timer for speed of falling
        */
        autouniqueptr < Timer > fallTimer ;

};

}

#endif

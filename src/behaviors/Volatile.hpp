// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Volatile_hpp_
#define Volatile_hpp_

#include "Behavior.hpp"
#include "Timer.hpp"


namespace iso
{

/**
 * For items which lack autonomous movement and move only on touch by some other item
 */

class Volatile : public Behavior
{

public:

        Volatile( const ItemPtr & item, const std::string & behavior ) ;

        virtual ~Volatile( ) ;

        virtual bool update () ;

private:

       /**
        * True if item isn’t volatile yet by toggle of switch in room
        */
        bool solid ;

       /**
        * Timer for disappearance of volatile item
        */
        autouniqueptr < Timer > disappearanceTimer ;

};

}

#endif

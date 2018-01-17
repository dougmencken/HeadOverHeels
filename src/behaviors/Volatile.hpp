// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Volatile_hpp_
#define Volatile_hpp_

#include "Behavior.hpp"
#include "Timer.hpp"


namespace isomot
{

class Item ;
class ItemData ;

/**
 * For items which lack autonomous movement and move only on touch by some other item
 */

class Volatile : public Behavior
{

public:

        Volatile( Item * item, const std::string & behavior ) ;

        virtual ~Volatile( ) ;

        virtual bool update () ;

       /**
        * Data used when volatile disappears
        */
        void setMoreData ( void * data ) ;

protected:

       /**
        * Used when volatile disappears
        */
        ItemData * bubblesData ;

       /**
        * Timer for disappearance of volatile item
        */
        Timer * disappearanceTimer ;

private:

       /**
        * True if item isn’t volatile yet by toggle of switch in room
        */
        bool solid ;

};

}

#endif

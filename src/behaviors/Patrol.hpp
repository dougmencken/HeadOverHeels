// The free and open source remake of Head over Heels
//
// Copyright © 2022 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Patrol_hpp_
#define Patrol_hpp_

#include "Behavior.hpp"
#include "Timer.hpp"


namespace iso
{

/**
 * Moves item randomly. For "four primary directions" variant move north, south, east or west.
 * For variant "four secondary directions" move northeast, northwest, southeast and southwest.
 * And for "eight directions" variant move in all these eight directions
 */

class Patrol : public Behavior
{

public:

        Patrol( const ItemPtr & item, const std::string & behavior ) ;

        virtual ~Patrol( ) ;

        virtual bool update () ;

protected:

       /**
        * El elemento cambia aleatoriamente de dirección
        */
        void changeOrientation () ;

private:

       /**
        * Timer for item’s speed of movement
        */
        autouniqueptr < Timer > speedTimer ;

       /**
        * Timer for item’s speed of falling
        */
        autouniqueptr < Timer > fallTimer ;

       /**
        * Timer for change of item’s direction
        */
        autouniqueptr < Timer > changeTimer ;

};

}

#endif

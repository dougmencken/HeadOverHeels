// The free and open source remake of Head over Heels
//
// Copyright © 2022 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Hunter_hpp_
#define Hunter_hpp_

#include "Behavior.hpp"
#include "Timer.hpp"


namespace iso
{

/**
 * Hunts for player by moving wherever it is
 */

class Hunter : public Behavior
{

public:

        Hunter( const ItemPtr & item, const std::string & behavior ) ;

        virtual ~Hunter( ) ;

        virtual bool update () ;

private:

       /**
        * Calculate direction in which item moves to hunt active player
        */
        ActivityOfItem calculateDirection ( const ActivityOfItem & activity ) ;

       /**
        * Calculate direction in which item moves to hunt active player,
        * direction may be north, south, east, and west
        */
        ActivityOfItem calculateDirection4 ( const ActivityOfItem & activity ) ;

       /**
        * Calculate direction in which item moves to hunt active player,
        * direction may be any of eight possible ones
        */
        ActivityOfItem calculateDirection8 ( const ActivityOfItem & activity ) ;

       /**
        * Morph hidden imperial guard to full-bodied hunter
        * @return true if created or false if it still can’t be created
        */
        bool createFullBody () ;

private:

       /**
        * Timer for item’s speed of movement
        */
        autouniqueptr < Timer > speedTimer ;

};

}

#endif

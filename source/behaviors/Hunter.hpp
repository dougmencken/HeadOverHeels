// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
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
 * Hunts for a character by moving wherever it is
 */

class Hunter : public Behavior
{

public:

        Hunter( const ItemPtr & item, const std::string & behavior ) ;

        virtual ~Hunter( ) ;

        virtual bool update () ;

private:

        /**
         * Changes the direction in which the item moves to hunt the active character,
         * calls updateDirection4 or updateDirection8 depending on the hunter's behavior
         */
        ActivityOfItem updateDirection ( const ActivityOfItem & activity ) ;

        /**
         * Changes the direction in which the item moves to hunt the active character,
         * the updated direction may be north, south, east, and west
         */
        ActivityOfItem updateDirection4 ( const ActivityOfItem & activity ) ;

        /**
         * Changes the direction in which the item moves to hunt the active character,
         * the updated direction may be any of the eight possible ones
         */
        ActivityOfItem updateDirection8 ( const ActivityOfItem & activity ) ;

        /**
         * Morph the hidden imperial guard into the full-bodied hunter
         * @return true if was created or false if can’t be created
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
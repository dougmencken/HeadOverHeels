// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Jumping_hpp_
#define Jumping_hpp_

#include <vector>

#include "Item.hpp"
#include "FreeItem.hpp"
#include "Behavior.hpp"
#include "ActivityOfItem.hpp"


namespace activities
{

/**
 * Salto
 */

class Jumping
{

public:

        virtual ~Jumping() {}

        static Jumping & getInstance() ;

        /**
         * Item is jumping
         * @param jumpVector vector of pairs of values ( offset on X or Y, offset on Z )
         *                   for each cycle to define how the item jumps
         * @param jumpPhase the phase of jumping
         * @return true for jump or false when there’s a collision
         */
        bool jump ( behaviors::Behavior * behavior, ActivityOfItem * activity, unsigned int jumpPhase, const std::vector < std::pair< int /* xy */, int /* z */ > >& jumpVector ) ;

protected:

       /**
        * Raise the items stacked on a character
        * @param sender the character that begins lifting the heap above
        * @param item the first item of the heap
        * @param z how many units to rise the heap
        */
        void lift ( FreeItem & sender, Item & item, int z ) ;

private:

        Jumping() {}

        static Jumping * instance ;

};

}

#endif

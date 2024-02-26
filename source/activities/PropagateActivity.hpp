// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef PropagateActivity_hpp_
#define PropagateActivity_hpp_

#include "ActivityOfItem.hpp"
#include "Item.hpp"


namespace activities
{

class PropagateActivity
{

public:

        /**
         * Change activity of items that collide with the sender
         */
        static void toAdjacentItems ( Item & sender, const ActivityOfItem & activity ) ;

        /**
         * Change activity of items above the sender
         */
        static void toItemsAbove ( Item & sender, const ActivityOfItem & activity ) ;

};

}

#endif
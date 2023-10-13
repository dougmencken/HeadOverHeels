// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef KindOfActivity_hpp_
#define KindOfActivity_hpp_

#include <stack>

#include "ActivityOfItem.hpp"
#include "Item.hpp"


namespace iso
{

/**
 * Abstraction of item’s current activity
 */

class KindOfActivity
{

public:

        KindOfActivity( ) ;

        virtual ~KindOfActivity( ) ;

protected:

       /**
        * Change activity of items that collide with the sender
        */
        virtual void propagateActivityToAdjacentItems ( Item & sender, const ActivityOfItem & activity ) ;

       /**
        * Change activity of items above the sender
        */
        virtual void propagateActivityToItemsAbove ( Item & sender, const ActivityOfItem & activity ) ;

};

}

#endif

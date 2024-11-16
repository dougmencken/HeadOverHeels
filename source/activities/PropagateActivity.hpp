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

#include "DescribedItem.hpp"
#include "Activity.hpp"
#include "Motion2D.hpp"


namespace activities
{

class PropagateActivity
{

public:

        /**
         * Spread an activity to every item collided with the sender
         */
        static void spreadEasily( const AbstractItem & sender, const Activity & activity, const Motion2D & velocity /* = Motion2D::rest() */ ) ;

        /**
         * Spread an activity to all items that collide with the sender, the comprehensive version
         */
        static void toAdjacentItems ( DescribedItem & sender, const Activity & activity, const Motion2D & velocity ) ;

        /**
         * Spread an activity to items above the sender
         */
        static void toItemsAbove ( DescribedItem & sender, const Activity & activity, const Motion2D & velocity ) ;

};

}

#endif

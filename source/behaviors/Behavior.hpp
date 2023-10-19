// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Behavior_hpp_
#define Behavior_hpp_

#include <string>

#include "ActivityOfItem.hpp"
#include "Item.hpp"

using activities::ActivityOfItem ;

namespace behaviors
{

/**
 * Abstraction for the item's behavior. A game's item changes its activity in each cycle of update.
 * Different kinds of behavior define different ways of transition between these activities
 */

class Behavior
{

protected:

        Behavior( const ItemPtr & whichItem, const std::string & behaviorFromFile ) ;

public:

        virtual ~Behavior( ) ;

        /**
         * Updates the item's behavior in each cycle
         * @return true if the item is still alive after this update or false otherwise
         */
        virtual bool update () = 0 ;

        virtual void changeActivityOfItem ( const ActivityOfItem & activity, const ItemPtr & sender = ItemPtr () )
                {  this->activity = activity ;  this->sender = sender ;  }

        std::string getNameOfBehavior () const {  return nameOfBehavior ;  }

        ActivityOfItem getActivityOfItem () const {  return activity ;  }

        const ItemPtr & getItem () {  return item ;  }

protected:

        /**
         * Change activity of every item collided with the sender
         */
        void propagateActivity ( const Item & sender, const ActivityOfItem & activity ) ;

        std::string nameOfBehavior ;

        /**
         * Item with this behavior
         */
        ItemPtr item ;

        /**
         * Current variant of activity
         */
        ActivityOfItem activity ;

        /**
         * Another item which changes activity of item with this behavior
         */
        ItemPtr sender ;

} ;

}

#endif

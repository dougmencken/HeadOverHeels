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

#include "Ism.hpp"
#include "ActivityOfItem.hpp"
#include "Item.hpp"


namespace iso
{

/**
 * Abstraction for behavior of item. Item of game changes its activity in each cycle of update.
 * Different kinds of behavior define different ways of transition between activities
 */

class Behavior
{

protected:

        Behavior( const ItemPtr & whichItem, const std::string & behaviorFromFile ) ;

public:

        virtual ~Behavior( ) ;

        static Behavior * createBehaviorByName ( const ItemPtr & item, const std::string & behavior ) ;

        /**
         * Updates behavior of item in each cycle
         * @return true if item is still alive after this update or false otherwise
         */
        virtual bool update () = 0 ;

        virtual void changeActivityOfItem ( const ActivityOfItem & activity, const ItemPtr & sender = ItemPtr () )
                {  this->activity = activity ;  this->sender = sender ;  }

        virtual void changeActivityOfItem ( const ActivityOfItem & activity, Item & sender )
                {  changeActivityOfItem( activity, ItemPtr( &sender ) ) ;  }

protected:

        /**
         * Change activity of every item collided with sender
         */
        void propagateActivity ( const Item & sender, const ActivityOfItem & activity ) ;

protected:

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

public:

        std::string getNameOfBehavior () const {  return nameOfBehavior ;  }

        ActivityOfItem getActivityOfItem () const {  return activity ;  }

        const ItemPtr & getItem () {  return item ;  }

};

}

#endif

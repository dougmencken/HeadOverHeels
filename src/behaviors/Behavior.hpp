// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Behavior_hpp_
#define Behavior_hpp_

#include <string>
#include <stack>

#include "Ism.hpp"


namespace isomot
{

class Item ;
class KindOfActivity ;

/**
 * Abstraction for behavior of item. Item of game changes its activity in each cycle of update.
 * Different kinds of behavior define different ways of transition between activities
 */

class Behavior
{

protected:

        Behavior( Item * whichItem, const std::string & behavior ) ;

public:

        virtual ~Behavior( ) ;

        static Behavior* createBehaviorByName ( Item* item, const std::string& behavior, void* extraData ) ;

        /**
         * Updates behavior of item in each cycle
         * @return true if item is still alive after this update or false otherwise
         */
        virtual bool update () = 0 ;

        /**
         * @param activity Activity of item
         * @param sender Item which changes activity
         */
        virtual void changeActivityOfItem ( const ActivityOfItem & activity, Item * sender = 0 ) ;

protected:

        friend class KindOfActivity ;

        /**
         * Change kind of activity
         */
        void changeActivityTo ( KindOfActivity * activity ) {  whatToDo = activity ;  }

        /**
         * Change activity of every item collided with sender
         */
        void propagateActivity ( Item * sender, const ActivityOfItem & activity ) ;

protected:

        std::string nameOfBehavior ;

        /**
         * Item with this behavior
         */
        Item * item ;

        /**
         * Current kind of activity
         */
        KindOfActivity * whatToDo ;

        /**
         * Current variant of activity
         */
        ActivityOfItem activity ;

        /**
         * Another item which changes activity of item with this behavior
         */
        Item * sender ;

public:

        std::string getNameOfBehavior () const {  return nameOfBehavior ;  }

        ActivityOfItem getActivityOfItem () const {  return activity ;  }

        Item* getItem () {  return item ;  }

        /**
         * Add some more data for behavior
         */
        virtual void setMoreData ( void * data ) { }

};

}

#endif

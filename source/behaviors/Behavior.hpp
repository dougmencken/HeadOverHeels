// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
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
 * Abstraction for the item’s behavior. A game’s item changes its activity in each cycle of update.
 * Different kinds of behavior define different transitions between these activities
 */

class Behavior
{

protected:

        Behavior( const ItemPtr & itemThatBehaves, const std::string & behaviorName ) ;

public:

        virtual ~Behavior( ) ;

        /**
         * Updates the item’s behavior in each cycle
         * @return true if the item is still alive after this update or false otherwise
         */
        virtual bool update () = 0 ;

        virtual void setActivityOfItem ( const ActivityOfItem & newActivity )
        {
                this->activity = newActivity ;
                if ( this->affectedBy != nilPointer ) this->affectedBy = ItemPtr () ;
        }

        virtual void changeActivityOfItemDueTo ( const ActivityOfItem & newActivity, const ItemPtr & dueTo )
        {
                this->activity = newActivity ;
                this->affectedBy = dueTo ;
        }

        std::string getNameOfBehavior () const {  return this->nameOfBehavior ;  }

        ActivityOfItem getActivityOfItem () const {  return this->activity ;  }

        const ItemPtr & getItem () {  return this->item ;  }

protected:

        /**
         * Change activity of every item collided with the sender
         */
        void propagateActivity ( const Item & sender, const ActivityOfItem & activity ) ;

        std::string nameOfBehavior ;

        /**
         * The item that behaves
         */
        ItemPtr item ;

        /**
         * The current activity
         */
        ActivityOfItem activity ;

        /**
         * Another item that changed activity of this one
         */
        ItemPtr affectedBy ;

} ;

}

#endif

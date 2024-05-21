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

#include "Activity.hpp"
#include "Item.hpp"

using activities::Activity ;

namespace behaviors
{

/**
 * Abstraction for the item’s behavior. A game’s item changes its activity in each cycle of update.
 * Different kinds of behavior define different transitions between these activities
 */

/* abstract */ class Behavior
{

protected:

        Behavior( Item & item, const std::string & behaviorName )
                : nameOfBehavior( behaviorName )
                , itemThatBehaves( item )
                , currentActivity( activities::Activity::Waiting )
                , affectedBy( nilPointer )
                , howLongFalls( 0 )
        {}

public:

        virtual ~Behavior( ) {}

        /**
         * Updates the item’s behavior programmatically
         * @return true if the item can be updated thereafter (it didn’t disappear from the room)
         */
        virtual bool update () = 0 ;

        const std::string & getNameOfBehavior () const {  return this->nameOfBehavior ;  }

        /**
         * The current activity
         */
        const Activity & getCurrentActivity () const {  return this->currentActivity ;  }

        virtual void setCurrentActivity ( const Activity & newActivity )
        {
                changeActivityDueTo( newActivity, ItemPtr() );
        }

        virtual void changeActivityDueTo ( const Activity & newActivity, const ItemPtr & dueTo )
        {
                this->currentActivity = newActivity ;
                this->affectedBy = dueTo ;
        }

        /**
         * The item that behaves
         */
        Item & getItem () const {  return this->itemThatBehaves ;  }

        /**
         * Another item that changed activity of this one
         */
        const ItemPtr & getWhatAffectedThisBehavior () const {  return this->affectedBy ;  }

        unsigned int getHowLongFalls () const {  return this->howLongFalls ;  }

        void incrementHowLongFalls () {  ++ this->howLongFalls ;  }
        void resetHowLongFalls () {  this->howLongFalls = 0 ;  }

private:

        std::string nameOfBehavior ;

        Item & itemThatBehaves ;

        Activity currentActivity ;

        ItemPtr affectedBy ;

        // to accelerate a long fall
        unsigned int howLongFalls ;

} ;

}

#endif

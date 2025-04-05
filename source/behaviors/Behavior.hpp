// The free and open source remake of Head over Heels
//
// Copyright © 2025 Douglas Mencken dougmencken@gmail.com
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
#include "Motion3D.hpp"
#include "AbstractItem.hpp"
#include "FreeItem.hpp"

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

        Behavior( AbstractItem & item, const std::string & behaviorName )
                : nameOfBehavior( behaviorName )
                , itemThatBehaves( item )
                , currentActivity( activities::Activity::Waiting )
                , velocityVector( Motion2D::rest() )
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

        const Motion3D & getVelocityVector () const {  return this->velocityVector ;  }
        const Motion3D & get3DVelocityVector () const {  return getVelocityVector() ;  }

        Motion2D get2DVelocityVector () const {  return this->velocityVector.to2D() ;  }

        virtual void beWaiting ()
        {
                setCurrentActivity( activities::Activity::Waiting, Motion2D::rest() );
        }

        virtual void setCurrentActivity ( const Activity & newActivity, const std::string & way )
        {
                if ( newActivity == activities::Activity::Moving || newActivity == activities::Activity::Automoving
                                || newActivity == activities::Activity::Pushed || newActivity == activities::Activity::Dragged
                ) {
                        setCurrentActivity( newActivity, Motion2D( way ) );
                } else
                        setCurrentActivity( newActivity, Motion2D::rest() );
        }

        virtual void setCurrentActivity ( const Activity & newActivity, const Motion2D & velocity )
        {
                changeActivityDueTo( newActivity, velocity, AbstractItemPtr() );
        }

        virtual void changeActivityDueTo ( const Activity & newActivity, const AbstractItemPtr & dueTo )
        {
                changeActivityDueTo( newActivity, Motion2D::rest(), dueTo );
        }

        virtual void changeActivityDueTo ( const Activity & newActivity, const Motion2D & vector, const AbstractItemPtr & dueTo )
        {
                this->currentActivity = newActivity ;
                this->velocityVector = vector ;
                this->affectedBy = dueTo ;
        }

        /**
         * The item that behaves
         */
        AbstractItem & getItem () const {  return this->itemThatBehaves ;  }

        /**
         * Another item that changed activity of this one
         */
        const AbstractItemPtr & getWhatAffectedThisBehavior () const {  return this->affectedBy ;  }

        unsigned int getHowLongFalls () const {  return this->howLongFalls ;  }

        void incrementHowLongFalls () {  ++ this->howLongFalls ;  }
        void resetHowLongFalls () {  this->howLongFalls = 0 ;  }

private:

        std::string nameOfBehavior ;

        AbstractItem & itemThatBehaves ;

        Activity currentActivity ;

        Motion3D velocityVector ;

        AbstractItemPtr affectedBy ;

        // to accelerate a long fall
        unsigned int howLongFalls ;

} ;

}

#endif

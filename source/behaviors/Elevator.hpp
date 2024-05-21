// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Elevator_hpp_
#define Elevator_hpp_

#include "Behavior.hpp"
#include "Timer.hpp"

class FreeItem ;

namespace activities
{

class ActivityOfElevator : public Activity
{

public: /* constants */

        static const unsigned int GoingDown             = 200 ;
        static const unsigned int ReachedBottom         = 202 ;
        static const unsigned int GoingUp               = 210 ;
        static const unsigned int ReachedTop            = 211 ;

} ;

}

namespace behaviors
{

/**
 * Moves an item above it up and down
 */

class Elevator : public Behavior
{

public:

        Elevator( FreeItem & item, const std::string & behavior ) ;

        virtual ~Elevator( ) {}

        virtual bool update () ;

        int getTop () const {  return top ;  }

        int getBottom () const {  return bottom ;  }

        bool getAscent () const {  return ascent ;  }

        void setTop ( int top ) {  this->top = top ;  }

        void setBottom ( int bottom ) {  this->bottom = bottom ;  }

        void setAscent ( bool ascent ) {  this->ascent = ascent ;  }

private:

        /**
         * the maximum height at which elevator ascends
         */
        int top ;

        /**
         * the minimum height at which elevator descends
         */
        int bottom ;

        /**
         * is the first movement of elevator ascending if true or descending if false
         */
        bool ascent ;

        /**
         * used to retain the previous activity
         */
        Activity lastActivity ;

        // timer for the motion speed
        autouniqueptr < Timer > speedTimer ;

        // timer for waiting at the highest and lowest points
        autouniqueptr < Timer > waitingTimer ;

};

}

#endif

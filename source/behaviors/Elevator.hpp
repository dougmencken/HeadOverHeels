// The free and open source remake of Head over Heels
//
// Copyright © 2026 Douglas Mencken dougmencken@gmail.com
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

public :

        ActivityOfElevator( unsigned int whatDoing ) : Activity( whatDoing ) {}

public: /* constants */

        static const unsigned int GoingDown             = 200 ;
        static const unsigned int ReachedBottom         = 202 ;
        static const unsigned int GoingUp               = 220 ;
        static const unsigned int ReachedTop            = 222 ;

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

        int getTop () const {  return this->top ;  }

        int getBottom () const {  return this->bottom ;  }

        bool isAscending () const {  return this->ascending ;  }

        void setTop ( int highest ) {  this->top = highest ;  }

        void setBottom ( int lowest ) {  this->bottom = lowest ;  }

        void setAscending ( bool ascend ) {  this->ascending = ascend ;  }

        void setActivityOfElevator ( const activities::ActivityOfElevator & newActivity )
                {  Behavior::setCurrentActivity( newActivity, Motion2D::rest() );  }

#ifdef __Cxx11__ /* when complier supports c++11 */
        static constexpr double Delay_Before_Reversing = 0.333 ; // in seconds
#else
        #define Delay_Before_Reversing          0.333
#endif

private:

        /**
         * the maximum height to which this elevator ascends
         */
        int top ;

        /**
         * the minimum height to which this elevator descends
         */
        int bottom ;

        /**
         * is the first movement of the elevator ascending if true or descending if false
         */
        bool ascending ;

        /**
         * used to retain the previous activity
         */
        activities::ActivityOfElevator lastActivity ;

        // timer for the motion speed
        autouniqueptr < Timer > speedTimer ;

        // timer for waiting at the highest and lowest points
        autouniqueptr < Timer > waitingTimer ;

};

}

#endif

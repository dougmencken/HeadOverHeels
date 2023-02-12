// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Elevator_hpp_
#define Elevator_hpp_

#include "Behavior.hpp"
#include "Timer.hpp"


namespace iso
{

/**
 * Moves item above it up and down
 */

class Elevator : public Behavior
{

public:

        Elevator( const ItemPtr & item, const std::string & behavior ) ;

        virtual ~Elevator( ) ;

        virtual bool update () ;

        int getTop () const {  return top ;  }

        int getBottom () const {  return bottom ;  }

        bool getAscent () const {  return ascent ;  }

        void setTop ( int top ) {  this->top = top ;  }

        void setBottom ( int bottom ) {  this->bottom = bottom ;  }

        void setAscent ( bool ascent ) {  this->ascent = ascent ;  }

private:

       /**
        * Maximum height at which elevator ascends
        */
        int top ;

       /**
        * Minimum height at which elevator descends
        */
        int bottom ;

       /**
        * Is first movement of elevator ascending if true or descending if false
        */
        bool ascent ;

        ActivityOfItem lastActivity ;

       /**
        * Timer for speed of movement
        */
        autouniqueptr < Timer > speedTimer ;

       /**
        * Timer for delays on change of direction
        */
        autouniqueptr < Timer > stopTimer ;

};

}

#endif

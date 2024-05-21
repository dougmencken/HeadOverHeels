// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Patrol_hpp_
#define Patrol_hpp_

#include "Behavior.hpp"
#include "Timer.hpp"

class FreeItem ;


namespace behaviors
{

/**
 * Moves item randomly
 *
 * For “in four primary directions” behavior move north, south, east or west.
 * For behavior “in four secondary directions” move northeast, northwest, southeast and southwest.
 * And for “in eight directions” move in all these eight directions
 */

class Patrol : public Behavior
{

public:

        Patrol( FreeItem & item, const std::string & behavior ) ;

        virtual ~Patrol( ) {}

        virtual bool update () ;

private:

        void randomlyChangeOrientation () ;

        // timer for the motion speed
        autouniqueptr < Timer > speedTimer ;

        // timer for the speed of falling
        autouniqueptr < Timer > fallTimer ;

        // timer for the next change of orientation
        autouniqueptr < Timer > patrolTimer ;

};

}

#endif

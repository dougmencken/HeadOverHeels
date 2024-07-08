// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef CreateGameOverSlide_hpp_
#define CreateGameOverSlide_hpp_

#include "Action.hpp"


namespace gui
{

/**
 * Create the summary screen to show the player’s range, score, the number of visited rooms
 * and the number of planets liberated
 */

class CreateGameOverSlide : public gui::Action
{

public:

        /**
         * @param rooms how many visited rooms
         * @param planets how many liberated planets
         */
        CreateGameOverSlide( unsigned int rooms, unsigned short planets )
                : Action( )
                , visitedRooms( rooms )
                , liberatedPlanets( planets ) {}

        virtual ~CreateGameOverSlide( ) {}

protected:

        virtual void act () ;

private:

        unsigned int visitedRooms ;

        unsigned short liberatedPlanets ;

};

}

#endif

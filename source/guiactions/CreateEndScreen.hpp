// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef CreateEndScreen_hpp_
#define CreateEndScreen_hpp_

#include "Action.hpp"


namespace gui
{

/**
 * Create the summary screen to show the player’s range, score, the number of visited rooms
 * and the number of planets liberated
 */

class CreateEndScreen : public gui::Action
{

public:

        /**
         * @param rooms how many visited rooms
         * @param planets how many liberated planets
         */
        CreateEndScreen( unsigned int rooms, unsigned short planets ) ;

        virtual std::string getNameOfAction () const {  return "CreateEndScreen" ;  }

protected:

        /**
         * show the summary screen
         */
        virtual void doAction () ;

private:

        unsigned int visitedRooms ;

        unsigned short liberatedPlanets ;

};

}

#endif

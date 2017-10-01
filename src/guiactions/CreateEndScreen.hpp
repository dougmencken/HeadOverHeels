// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef CreateEndScreen_hpp_
#define CreateEndScreen_hpp_

#include <string>
#include <allegro.h>
#include "Action.hpp"


namespace gui
{

/**
 * Create summary screen to show player’s range, score, number of visited rooms
 * and number of liberated planets
 */

class CreateEndScreen : public gui::Action
{

public:

        /**
         * Constructor
         * @param picture Image where to draw
         * @param rooms Count of visited rooms
         * @param planets Count of liberated planets
         */
        CreateEndScreen( BITMAP* picture, unsigned short rooms, unsigned short planets ) ;

        /**
         * Show summary screen
         */
        void doIt () ;

        std::string getNameOfAction () const {  return "CreateEndScreen" ;  }

private:

        BITMAP* where ;

        /**
         * Count of visited rooms
         */
        unsigned short rooms ;

        /**
         * Count of liberated planets
         */
        unsigned short planets ;

};

}

#endif

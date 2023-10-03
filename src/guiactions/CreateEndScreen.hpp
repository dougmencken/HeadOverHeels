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
 * Create summary screen to show player’s range, score, number of visited rooms
 * and number of liberated planets
 */

class CreateEndScreen : public gui::Action
{

public:

        /**
         * @param rooms Count of visited rooms
         * @param planets Count of liberated planets
         */
        CreateEndScreen( unsigned int rooms, unsigned short planets ) ;

        virtual std::string getNameOfAction () const {  return "CreateEndScreen" ;  }

protected:

        /**
         * Show summary screen
         */
        virtual void doAction () ;

private:

        /**
         * Count of visited rooms
         */
        unsigned int rooms ;

        /**
         * Count of liberated planets
         */
        unsigned short planets ;

};

}

#endif

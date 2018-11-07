// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Camera_hpp_
#define Camera_hpp_

#include "Ism.hpp"
#include "Way.hpp"
#include "PlayerItem.hpp"


namespace iso
{

class Room;

/**
 * Camera centers the room. If it’s simple or small room, it just calculates optimal coordinates.
 * If it is a big room, it also centers active character on screen and calculates coordinates
 * to draw part of such room on screen
 */

class Camera
{

public:

        /**
         * @param Room for this camera
         */
        Camera( Room * room ) ;

        virtual ~Camera( ) ;

        /* Center room on screen and set reference point. Once this point has been assigned,
         * use "centerOn" to center room, since "turnOn" will no longer have an effect
         * @param player active player
         * @param entry Way of entry to room
         */
        void turnOn ( const PlayerItem & player, const Way& wayOfEntry ) ;

        /**
         * Centers the room on the screen
         */
        bool centerOn ( const PlayerItem & player ) ;

private:

        /**
         * La sala objetivo de la cámara
         */
        Room * room ;

        /**
         * Reference point from which to move camera
         */
        std::pair < int, int > reference ;

        std::pair < int, int > delta ;

public:

        int getDeltaX () const {  return delta.first;  }

        int getDeltaY () const {  return delta.second;  }

        void setDeltaX ( int delta ) {  this->delta.first = delta;  }

        void setDeltaY ( int delta ) {  this->delta.second = delta;  }

};

}

#endif

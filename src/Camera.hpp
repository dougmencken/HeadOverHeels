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
#include "Item.hpp"


namespace iso
{

class Room ;

/**
 * Camera gives offset for image of room
 */

class Camera
{

public:

        /**
         * @param Room for this camera
         */
        Camera( Room * room ) ;

        virtual ~Camera( ) { }

        void centerRoom () ;

        /*
         * @return whether camera has moved
         */
        bool centerOnItem ( const Item & item ) ;

private:

        /**
         * La sala objetivo de la cámara
         */
        Room * room ;

        std::pair < int, int > delta ;

        std::string centeredOnItem ;

public:

        int getDeltaX () const {  return delta.first;  }

        int getDeltaY () const {  return delta.second;  }

        void setDeltaX ( int delta ) {  this->delta.first = delta;  }

        void setDeltaY ( int delta ) {  this->delta.second = delta;  }

};

}

#endif

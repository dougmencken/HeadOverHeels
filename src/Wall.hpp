// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Wall_hpp_
#define Wall_hpp_

#include <WrappersAllegro.hpp>

#include "Ism.hpp"
#include "Drawable.hpp"
#include "Mediated.hpp"


namespace isomot
{

/**
 * Segment of room’s wall
 */

class Wall : public Drawable, public Mediated
{

public:

        Wall( bool trueXfalseY, int index, allegro::Pict* image ) ;

        virtual ~Wall( ) ;

        /**
         * Calculate offset for wall’s graphics
         */
        void calculateOffset () ;

        void draw ( allegro::Pict * where ) ;

private:

        bool onX;

        /**
         * Position of this segment on wall, the smaller the closer to point of origin
         */
        int position ;

        std::pair < int, int > offset ;

        /**
         * Graphics of wall’s segment
         */
        allegro::Pict * image ;

public:

        bool isOnX () const {  return onX ;  }

        bool isOnY () const {  return ! onX ;  }

        int getPosition () {  return position ;  }

};

}

#endif

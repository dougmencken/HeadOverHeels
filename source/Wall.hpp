// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Wall_hpp_
#define Wall_hpp_

#include <WrappersAllegro.hpp>

#include "Picture.hpp"
#include "Drawable.hpp"
#include "Mediated.hpp"


/**
 * The segment of the room’s wall
 */

class Wall : public Drawable, public Mediated
{

public:

        Wall( bool trueXfalseY, int index, Picture* image ) ;

        virtual ~Wall( ) ;

        /**
         * Calculate offset for wall’s graphics
         */
        void calculateOffset () ;

        virtual void draw () ;

        virtual bool operator < ( const Wall& segment ) const
                {  return getPosition() < segment.getPosition() ;  }

        static bool comparePointersToWall ( const Wall * first, const Wall * second )
                {  return ( first != nilPointer && second != nilPointer ) ? *first < *second : false ;  }

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
        Picture * image ;

public:

        bool isOnX () const {  return onX ;  }

        bool isOnY () const {  return ! onX ;  }

        int getPosition () const {  return position ;  }

        Picture * getImage () const {  return image ;  }

} ;

typedef multiptr < Wall > WallPtr ;

#endif

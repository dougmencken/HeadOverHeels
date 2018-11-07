// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef FloorTile_hpp_
#define FloorTile_hpp_

#include <WrappersAllegro.hpp>

#include "Ism.hpp"
#include "Drawable.hpp"
#include "Mediated.hpp"
#include "Shady.hpp"
#include "Picture.hpp"


namespace iso
{

/**
 * Floor tile of room, set of such items forms room’s ground
 */

class FloorTile : public Drawable, public Mediated, public Shady
{

public:

       /**
        * @param cellX X coordinate on grid
        * @param cellY Y coordinate on grid
        * @param image Graphics of tile
        */
        FloorTile( int cellX, int cellY, Picture * image ) ;

        virtual ~FloorTile( ) ;

        /**
         * Calculate offset for tile’s graphics
         */
        void calculateOffset () ;

        void draw ( const allegro::Pict& where ) ;

private:

        std::string uniqueName;

        /**
         * X and Y coordinates on grid
         */
        std::pair < int, int > coordinates ;

        /**
         * Offset ( X, Y ) for tile’s graphics
         */
        std::pair < int, int > offset ;

        /**
         * Picture of tile
         */
        Picture * rawImage ;

        /**
         * Picture of shaded tile
         */
        Picture * shadyImage ;

public:

        /**
         * Room’s column to which this tile belongs
         */
        int getColumn () const ;

        std::string getUniqueName () const {  return uniqueName ;  }

        int getCellX () const {  return coordinates.first ;  }

        int getCellY () const {  return coordinates.second ;  }

        int getOffsetX () const {  return offset.first ;  }

        int getOffsetY () const {  return offset.second ;  }

        Picture * getRawImage () const {  return rawImage ;  }

        Picture * getShadyImage () const {  return shadyImage ;  }

        void setShadyImage( Picture * newShady ) ;

        void freshShadyImage () ;

};

typedef safeptr < FloorTile > FloorTilePtr ;

}

#endif

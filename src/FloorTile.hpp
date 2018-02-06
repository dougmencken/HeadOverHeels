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

#include <allegro.h>

#include "Ism.hpp"
#include "Drawable.hpp"
#include "Mediated.hpp"


namespace isomot
{

/**
 * Floor tile of room, set of such items forms room’s ground
 */

class FloorTile : public Drawable, public Mediated
{

public:

       /**
        * Constructor
        * @param column Room’s column to which this tile belongs
        * @param x X coordinate on grid
        * @param y Y coordinate on grid
        * @param image Graphics of tile
        */
        FloorTile( int column, int x, int y, BITMAP* image ) ;

        virtual ~FloorTile( ) ;

        /**
         * Calculate offset for tile’s graphics
         */
        void calculateOffset () ;

        /**
         * Dibuja la loseta
         */
        void draw ( BITMAP* where ) ;

        void requestShadow () ;

        /**
         * Sombrea la imagen de la loseta con la sombra de otro elemento
         * @param x Coordenada X de pantalla donde está situada la base del elemento que sombrea
         * @param y Coordenada Y de pantalla donde está situada la base del elemento que sombrea
         * @param shadow La sombra que se proyecta sobre la loseta
         * @param shadingScale Grado de opacidad de las sombras desde 0, sin sombras, hasta 256,
         *                     sombras totalmente opacas
         * @param transparency Grado de transparencia del elemento que sombrea a la loseta
         */
        void castShadowImage ( int x, int y, BITMAP* shadow, short shadingScale, unsigned char transparency = 0 ) ;

private:

        /**
         * Room’s column to which this tile belongs
         */
        int column ;

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
        BITMAP * rawImage ;

        /**
         * Picture of shaded tile
         */
        BITMAP * shadyImage ;

        /**
         * Estado de sombreado de la loseta
         */
        WhichShade shady ;

public:

        int getColumn () const {  return column ;  }

        int getX () const {  return coordinates.first ;  }

        int getY () const {  return coordinates.second ;  }

        int getOffsetX () const {  return offset.first ;  }

        int getOffsetY () const {  return offset.second ;  }

        BITMAP* getRawImage () const {  return rawImage ;  }

        /**
         * Establece el estado del proceso de sombreado
         */
        void setWhichShade ( const WhichShade& shade ) {  this->shady = shade ;  }

        /**
         * Estado del proceso de sombreado
         */
        WhichShade whichShade () const {  return shady ;  }

};

}

#endif

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

#include <utility>
#include <cmath>
#include <allegro.h>
#include "Ism.hpp"
#include "Drawable.hpp"
#include "Mediated.hpp"


namespace isomot
{

/**
 * Una loseta de una sala. Un conjunto de estos elementos forma el suelo
 */

class FloorTile : public Drawable, public Mediated
{

public:

       /**
        * Constructor. Crea una loseta nula
        */
        FloorTile( ) ;

       /**
        * Constructor
        * @param Columna de la sala a la que pertenece la loseta
        * @param x Coordenada X de la loseta en la rejilla
        * @param y Coordenada Y de la loseta en la rejilla
        * @param image Representación gráfica de la loseta
        */
        FloorTile( int column, int x, int y, BITMAP* image ) ;

        virtual ~FloorTile( ) ;

        /**
         * Calcula el desplazamiento de la imagen de la loseta en función de los atributos actuales
         */
        void calculateOffset () ;

        /**
         * Dibuja la loseta
         */
        void draw ( BITMAP* where ) ;

        /**
         * Solicita el sombreado de la loseta
         * @param xCell Coordenada X de la loseta en la rejilla de la sala
         * @param yCell Coordenada Y de la loseta en la rejilla de la sala
         */
        void requestCastShadow () ;

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
         * Columna de la sala a la que pertenece la loseta
         */
        int column ;

        /**
         * Coordenadas X e Y, respectivamente, de la loseta en la rejilla
         */
        std::pair < int, int > coordinates ;

        /**
         * Desplazamiento de la loseta en los ejes X e Y, respectivamente
         */
        std::pair < int, int > offset ;

        /**
         * Gráfico de la loseta
         */
        BITMAP * image ;

        /**
         * Gráfico de la loseta sombreada
         */
        BITMAP * shadyImage ;

        /**
         * Estado de sombreado de la loseta
         */
        WhichShade shady ;

public:

        /**
         * Columna de la sala a la que pertenece la loseta
         * @param Un valor entre cero y el producto del número de losetas de la sala en cada eje
         */
        int getColumn () const {  return column ;  }

        /**
         * Coordenada X de la loseta en la rejilla
         * @param Un número positivo
         */
        int getX () const {  return coordinates.first ;  }

        /**
         * Coordenada Y de la loseta en la rejilla
         * @param Un número positivo
         */
        int getY () const {  return coordinates.second ;  }

        /**
         * Establece el desplazamiento espacial de la loseta
         * @param x Desplazamiento de la loseta en el eje X
         * @param y Desplazamiento de la loseta en el eje Y
         */
        void setOffset ( int x, int y ) {  offset.first = x;  offset.second = y;  }

        /**
         * Gráfico de la loseta
         * @return Devuelve una imagen de Allegro
         */
        BITMAP* getImage () {  return image ;  }

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

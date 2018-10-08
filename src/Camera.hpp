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


namespace isomot
{

class Room;
class PlayerItem;

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

        /**
        * Centra la sala en pantalla y establece el punto de referencia. Una vez se ha
        * asignado dicho punto debe usarse la operación "center" para centrar la sala,
        * pues "turnOn" ya no tendrá efecto
        * @param player El jugador activo
        * @param entry Vía de entrada a la sala
        */
        void turnOn ( PlayerItem * player, const Way& wayOfEntry ) ;

        /**
         * Centra la sala en pantalla
         * @param player El jugador activo
         * @return true si se ha producido variación respecto al desplazamiento previo,
         * o false en caso contrario
         */
        bool centerOn ( PlayerItem * player ) ;

private:

        /**
         * La sala objetivo de la cámara
         */
        Room * room ;

        /**
         * Punto de referencia a partir del cual se realizará el desplazamiento de la cámara
         * Coincide con las coordenadas X e Y del jugador activo
         */
        std::pair < int, int > reference ;

        /**
         * Desplazamiento de pantalla a realizar en el eje X e Y, respectivamente, para centrar la sala
         */
        std::pair < int, int > delta ;

public:

        /**
         * Desplazamiento de pantalla a realizar en el eje X para centrar la sala
         * @return Un número positivo indica que la sala se desplazará a la derecha, mientras que un
         * número negativo desplazará la sala a la izquierda
         */
        int getDeltaX () const {  return delta.first;  }

        /**
         * Desplazamiento de pantalla a realizar en el eje Y para centrar la sala
         * @return Un número positivo indica que la sala se desplazará hacia abajo, mientras que un
         * número negativo desplazará la sala hacia arriba
         */
        int getDeltaY () const {  return delta.second;  }

        void setDeltaX ( int delta ) {  this->delta.first = delta;  }

        void setDeltaY ( int delta ) {  this->delta.second = delta;  }

};

}

#endif

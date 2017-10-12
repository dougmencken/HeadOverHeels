// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Camera_hpp_
#define Camera_hpp_

#include <utility>
#include "Ism.hpp"
#include "Way.hpp"


namespace isomot
{

class Room;
class PlayerItem;

/**
 * Centra la sala. Si es una sala simple o pequeña simplemente calculará las
 * coordenadas de pantalla óptimas para situar el punto origen del espacio
 * Si es una sala múltiple, además centrará al personaje activo en pantalla y
 * calculará las coordenadas donde debe situarse el bitmap en el que se dibuja
 * la sala respecto de la pantalla
 */
class Camera
{

public:

        /**
         * Constructor
         * @param La sala objetivo de la cámara
         */
        Camera( Room* room ) ;

        virtual ~Camera( ) ;

        /**
        * Centra la sala en pantalla y establece el punto de referencia. Una vez se ha
        * asignado dicho punto debe usarse la operación "center" para centrar la sala,
        * pues "turnOn" ya no tendrá efecto
        * @param player El jugador activo
        * @param entry Vía de entrada a la sala
        */
        void turnOn ( PlayerItem* player, const Way& wayOfEntry ) ;

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

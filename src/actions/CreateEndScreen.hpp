// The free and open source remake of Head over Heels
//
// Copyright © 2016 Douglas Mencken dougmencken @ gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef CreateEndScreen_hpp_
#define CreateEndScreen_hpp_

#include <string>
#include <allegro.h>
#include "Action.hpp"

namespace gui
{

/**
 * Crea la pantalla resumen de la partida. Muestra el rango, la puntuación, el
 * número de salas visitadas y el número de planetas liberados
 */
class CreateEndScreen : public gui::Action
{

public:

        /**
         * Constructor
         * @param picture Imagen donde se dibujará la interfaz gráfica
         * @param rooms Número de salas visitadas
         * @param planets Número de planetas liberados
         */
        CreateEndScreen( BITMAP* picture, unsigned short rooms, unsigned short planets ) ;

        /**
         * Show the summary screen
         */
        void doIt () ;

private:

        /**
         * Imagen donde se dibujará la interfaz gráfica
         */
        BITMAP* where ;

        /**
         * Número de salas visitadas
         */
        unsigned short rooms ;

        /**
         * Número de planetas liberados
         */
        unsigned short planets ;

};

}

#endif

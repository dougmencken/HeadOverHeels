// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef LoadGame_hpp_
#define LoadGame_hpp_

#include <string>
#include <allegro.h>
#include "Action.hpp"

namespace gui
{

/**
 * Carga una partida del disco y pone en marcha el juego
 */
class LoadGame : public Action
{

public:

        /**
         * Constructor
         * @param picture Imagen donde se dibujará la interfaz gráfica
         * @param slot Número del archivo a cargar
         */
        LoadGame( BITMAP* picture, int slot ) ;


        /**
         * Load the game and begin it
         */
        void doIt () ;

private:

        /**
         * Imagen donde se dibujará la interfaz gráfica
         */
        BITMAP* where ;

        /**
         * Número del archivo a cargar. Los archivos tienen un nombre de la forma savegameN.xml,
         * donde N es este número
         */
        int slot ;

};

}

#endif

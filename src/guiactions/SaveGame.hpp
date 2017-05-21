// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef SaveGame_hpp_
#define SaveGame_hpp_

#include <string>
#include <allegro.h>
#include "Action.hpp"


namespace gui
{

/**
 * Graba la partida en curso y regresa al juego
 */

class SaveGame : public Action
{

public:

        /**
         * Constructor
         * @param picture Imagen donde se dibujará la interfaz gráfica
         * @param slot Número del archivo a grabar
         */
        SaveGame( BITMAP* picture, int slot ) ;

        /**
         * Save the game and back to play
         */
        void doIt () ;

        std::string getNameOfAction ()  {  return "SaveGame" ;  }

private:

        /**
         * Imagen donde se dibujará la interfaz gráfica
         */
        BITMAP* where ;

        /**
         * Número del archivo a grabar. Los archivos tienen un nombre de la forma savegameN.xml,
         * donde N es este número
         */
        int slot ;

};

}

#endif

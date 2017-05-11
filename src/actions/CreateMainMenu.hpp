// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef CreateMainMenu_hpp_
#define CreateMainMenu_hpp_

#include <string>
#include <allegro.h>
#include "Action.hpp"


namespace gui
{

class Screen ;

/**
 * Crea el menú principal del juego
 */

class CreateMainMenu : public Action
{

public:

        /**
         * Constructor
         * @param picture Imagen donde se dibujará la interfaz gráfica
         */
        CreateMainMenu( BITMAP* picture ) ;

        /**
         * Show the main menu of the game
         */
        void doIt () ;

        std::string getNameOfAction ()  {  return "CreateMainMenu" ;  }

private:

        /**
         * Imagen donde se dibujará la interfaz gráfica
         */
        BITMAP* where ;

};

}

#endif

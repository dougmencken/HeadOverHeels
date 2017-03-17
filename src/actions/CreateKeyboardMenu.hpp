// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef CreateKeyboardMenu_hpp_
#define CreateKeyboardMenu_hpp_

#include <string>
#include <allegro.h>
#include "Action.hpp"

namespace gui
{

/**
 * Crea el menú de configuración del teclado y el joystick
 */
class CreateKeyboardMenu : public Action
{

public:

        /**
         * Constructor
         * @param picture Imagen donde se dibujará la interfaz gráfica
         */
        CreateKeyboardMenu( BITMAP* picture ) ;

        /**
         * Show the keys menu
         */
        void doIt () ;

        const char * getNameOfAction ()  {  return "CreateKeyboardMenu" ;  }

private:

        /**
         * Imagen donde se dibujará la interfaz gráfica
         */
        BITMAP* where ;

};

}

#endif

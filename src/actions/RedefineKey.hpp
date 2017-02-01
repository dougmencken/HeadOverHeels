// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef RedefineKey_hpp_
#define RedefineKey_hpp_

#include <string>
#include <allegro.h>
#ifdef __WIN32
  #include <winalleg.h>
#endif
#include "Action.hpp"

namespace gui
{

// Declaraciones adelantadas
class Menu;

/**
 * Cambia una tecla para el control del juego por otra indicada por el usuario
 */
class RedefineKey : public Action
{

public:

        /**
        * Constructor
        * @param menu Menú de opciones donde se encuentra el texto de la tecla a cambiar
        * @param keyText Descripción de la tecla a cambiar
        * @param assignedKey Código de la tecla asignada actualmente
        */
        RedefineKey( Menu* menu, std::string keyText, int assignedKey ) ;

        /**
        * Change the key
        */
        void doIt () ;

private:

        /**
        * Menú de opciones donde se encuentra el texto de la tecla a cambiar
        */
        Menu* menu ;

        /**
        * Descripción de la tecla a cambiar
        */
        std::string keyText ;

        /**
        * Código de la tecla asignada actualmente
        */
        int assignedKey ;

};

}

#endif

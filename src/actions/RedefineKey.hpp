// Head over Heels (A remake by helmántika.org)
//
// © Copyright 2008 Jorge Rodríguez Santos <jorge@helmantika.org>
// © Copyright 1987 Ocean Software Ltd. (Original game)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef REDEFINEKEY_HPP_
#define REDEFINEKEY_HPP_

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

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

#ifndef ADJUSTMUSIC_HPP_
#define ADJUSTMUSIC_HPP_

#include "AdjustValue.hpp"

namespace gui
{

// Declaraciones adelantadas
class Menu;

/**
 * Ajusta el volumen de la música haciendo uso de las teclas ARRIBA y ABAJO
 */
class AdjustMusic : public AdjustValue
{

public:

        /**
         * Constructor
         * @param menu Menú de opciones donde se cambia el volumen de la música
         * @param text Descripción de la opción que permite cambiar el volumen
         */
        AdjustMusic( Menu* menu, const std::string& text ) ;

        /**
         * Change the volume of music
         */
        void doIt () ;

};

}

#endif

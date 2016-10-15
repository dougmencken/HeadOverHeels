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

#ifndef CREATEPLANETSSCREEN_HPP_
#define CREATEPLANETSSCREEN_HPP_

#include <allegro.h>
#include "Action.hpp"

namespace gui
{

/**
 * Crea la pantalla de los planetas
 */
class CreatePlanetsScreen : public Action
{

public:

        /**
         * Constructor
         * @param picture Imagen donde se dibujará la interfaz gráfica
         * @param gameInProgress Indica si hay una partida en curso o comienza una nueva
         */
        CreatePlanetsScreen( BITMAP* picture, bool gameInProgress );

        /**
         * Crea la pantalla de los planetas
         */
        void doIt () ;

        /**
         * Indica la liberación de Blacktooth
         */
        void blacktoothIsFree() {  this->blacktooth = true ;  }

        /**
         * Indica la liberación de Egyptus
         */
        void egyptusIsFree() {  this->egyptus = true ;  }

        /**
         * Indica la liberación de Penitentiary
         */
        void penitentiaryIsFree() {  this->penitentiary = true ;  }

        /**
         * Indica la liberación de Byblos
         */
        void byblosIsFree() {  this->byblos = true ;  }

        /**
         * Indica la liberación de Safari
         */
        void safariIsFree() {  this->safari = true ;  }

private:

        /**
         * Imagen donde se dibujará la interfaz gráfica
         */
        BITMAP* where ;

        /**
         * Indica si hay una partida en curso o comienza una nueva
         */
        bool gameInProgress ;

        /**
         * Indica si Blacktooth ha sido liberado
         */
        bool blacktooth ;

        /**
         * Indica si Egyptus ha sido liberado
         */
        bool egyptus ;

        /**
         * Indica si Penitentiary ha sido liberado
         */
        bool penitentiary ;

        /**
         * Indica si Byblos ha sido liberado
         */
        bool byblos ;

        /**
         * Indica si Safari ha sido liberado
         */
        bool safari ;

};

}

#endif

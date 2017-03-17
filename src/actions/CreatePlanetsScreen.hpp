// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef CreatePlanetsScreen_hpp_
#define CreatePlanetsScreen_hpp_

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

        const char * getNameOfAction ()  {  return "CreatePlanetsScreen" ;  }

        /**
         * Indica la liberación de Blacktooth
         */
        void blacktoothIsFree() {  blacktoothFree = true ;  }

        /**
         * Indica la liberación de Egyptus
         */
        void egyptusIsFree() {  egyptusFree = true ;  }

        /**
         * Indica la liberación de Penitentiary
         */
        void penitentiaryIsFree() {  penitentiaryFree = true ;  }

        /**
         * Indica la liberación de Byblos
         */
        void byblosIsFree() {  byblosFree = true ;  }

        /**
         * Indica la liberación de Safari
         */
        void safariIsFree() {  safariFree = true ;  }

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
        bool blacktoothFree ;

        /**
         * Indica si Egyptus ha sido liberado
         */
        bool egyptusFree ;

        /**
         * Indica si Penitentiary ha sido liberado
         */
        bool penitentiaryFree ;

        /**
         * Indica si Byblos ha sido liberado
         */
        bool byblosFree ;

        /**
         * Indica si Safari ha sido liberado
         */
        bool safariFree ;

};

}

#endif

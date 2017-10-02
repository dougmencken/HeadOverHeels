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
         * @param where Imagen donde se dibujará la interfaz gráfica
         * @param gameInProgress Indica si hay una partida en curso o comienza una nueva
         */
        CreatePlanetsScreen( BITMAP* where, bool gameInProgress );

        virtual ~CreatePlanetsScreen( ) ;

        /**
         * Crea la pantalla de los planetas
         */
        void doIt () ;

        std::string getNameOfAction () const {  return "CreatePlanetsScreen" ;  }

        void liberateBlacktooth() {  blacktoothFree = true ;  }

        void liberateEgyptus() {  egyptusFree = true ;  }

        void liberatePenitentiary() {  penitentiaryFree = true ;  }

        void liberateByblos() {  byblosFree = true ;  }

        void liberateSafari() {  safariFree = true ;  }

private:

        /**
         * Where graphical interface draws
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

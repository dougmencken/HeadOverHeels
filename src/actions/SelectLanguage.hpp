// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef SelectLanguage_hpp_
#define SelectLanguage_hpp_

#include <string>
#include <allegro.h>
#include "Action.hpp"


namespace gui
{

/**
 * Establece el idioma del juego
 */

class SelectLanguage: public Action
{

public:

        /**
         * Constructor
         * @param picture Imagen donde se dibujará la interfaz gráfica
         * @param language Código ISO del idioma seleccionado
         */
        SelectLanguage( BITMAP* picture, const std::string& language ) ;

        /**
         * Set the language of the game
         */
        void doIt () ;

        std::string getNameOfAction ()  {  return "SelectLanguage" ;  }

private:

        /**
         * Imagen donde se dibujará la interfaz gráfica
         */
        BITMAP* where ;

        /**
         * Código ISO del idioma seleccionado
         */
        std::string language ;

};

}

#endif

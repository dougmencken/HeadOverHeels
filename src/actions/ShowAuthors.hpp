// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef ShowAuthors_hpp_
#define ShowAuthors_hpp_

#include <allegro.h>
#include "Action.hpp"


namespace gui
{

class Screen;
class Label;
class LanguageLine;

/**
 * Muestra los autores de la aplicación y todos sus colaboradores
 */

class ShowAuthors : public Action
{

public:

        /**
         * Constructor
         * @param picture Imagen donde se dibujará la interfaz gráfica
         */
        ShowAuthors( BITMAP* picture ) ;

        void doIt () ;

        const char * getNameOfAction ()  {  return "ShowAuthors" ;  }

private:

        /**
         * Imagen donde se dibujará la interfaz gráfica
         */
        BITMAP* where ;

};

}

#endif

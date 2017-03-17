// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef CreateAudioMenu_hpp_
#define CreateAudioMenu_hpp_

#include "Action.hpp"

#include <allegro.h>


namespace gui
{

/**
 * Crea el menú de para el ajuste de los efectos sonidos y la música
 */

class CreateAudioMenu : public Action
{

public:

        /**
         * Constructor
         * @param picture Imagen donde se dibujará la interfaz gráfica
         */
        CreateAudioMenu( BITMAP* picture ) ;

        /**
         * Show the audio menu
         */
        void doIt () ;

        const char * getNameOfAction ()  {  return "CreateAudioMenu" ;  }

private:

        /**
         * Imagen donde se dibujará la interfaz gráfica
         */
        BITMAP* where ;

};

}

#endif

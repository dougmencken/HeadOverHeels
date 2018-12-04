// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef CreateCongratulationsScreen_hpp_
#define CreateCongratulationsScreen_hpp_

#include "Action.hpp"


namespace gui
{

/**
 * Presenta el texto final de felicitación cuando se termina con éxito el juego
 */

class CreateCongratulationsScreen : public Action
{

public:

        /**
         * @param rooms Número de salas visitadas
         * @param planets Número de planetas liberados
         */
        CreateCongratulationsScreen( unsigned short rooms, unsigned short planets ) ;

        virtual ~CreateCongratulationsScreen( ) ;

        virtual std::string getNameOfAction () const {  return "CreateCongratulationsScreen" ;  }

protected:

        /**
         * Crea la pantalla con el texto
         */
        virtual void doAction () ;

private:

        /**
         * Número de salas visitadas
         */
        unsigned short rooms ;

        /**
         * Número de planetas liberados
         */
        unsigned short planets ;

};

}

#endif

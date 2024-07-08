// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef ShowCongratulations_hpp_
#define ShowCongratulations_hpp_

#include "Action.hpp"


namespace gui
{

/**
 * Presenta el texto final de felicitación cuando el juego se completa con éxito
 */

class ShowCongratulations : public Action
{

public:

        /**
         * @param rooms Número de salas visitadas
         * @param planets Número de planetas liberados
         */
        ShowCongratulations( unsigned short rooms, unsigned short planets )
                : Action( )
                , rooms( rooms )
                , planets( planets ) {}

        virtual ~ShowCongratulations( ) {}

protected:

        virtual void act () ;

private:

        unsigned short rooms ;

        unsigned short planets ;

};

}

#endif

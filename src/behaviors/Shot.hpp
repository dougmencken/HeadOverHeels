// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Shot_hpp_
#define Shot_hpp_

#include "Behavior.hpp"
#include "HPC.hpp"

namespace isomot
{

class PlayerItem ;

class Shot : public Behavior
{

public:

       /**
        * Constructor
        * @param item Elemento que tiene este comportamiento
        * @param id Identificador de comportamiento
        */
        Shot( Item* item, const BehaviorId& id ) ;

        virtual ~Shot( ) ;

       /**
        * Actualiza el comportamiento del elemento en cada ciclo
        * @return false si la actualización implica la destrucción del elemento o true en caso contrario
        */
        virtual bool update () ;

private:

       /**
        * El jugador que ha disparado
        */
        PlayerItem* playerItem ;

       /**
        * Cronómetro que controla la velocidad de movimiento del elemento
        */
        HPC* speedTimer ;

public:

       /**
        * Establece el jugador que ha disparado
        * @param playerItem Un jugador
        */
        void setPlayerItem ( PlayerItem* playerItem ) ;

};

}

#endif

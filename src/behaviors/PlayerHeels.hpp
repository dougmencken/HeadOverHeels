// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef PlayerHeels_hpp_
#define PlayerHeels_hpp_

#include "UserControlled.hpp"

namespace isomot
{

class Item;
class ItemData;

/**
 * Comportamiento del jugador Head. Dado que Head es uno de los elementos controlados por el usuario
 * su comportamiento viene definido tanto por la operación "update" como por "behave"
 * Head puede caminar, saltar, planear y disparar, como acciones más significativas
 */

class PlayerHeels : public UserControlled
{

public:

       /**
        * Constructor
        * @param item Elemento que tiene este comportamiento
        * @param id Identificador del comportamiento
        */
        PlayerHeels( Item * item, const BehaviorId & id ) ;

        virtual ~PlayerHeels( ) ;

       /**
        * Actualiza el comportamiento del elemento en cada ciclo
        * @return false si la actualización implica la destrucción del elemento o true en caso contrario
        */
        virtual bool update () ;

       /**
        * Actualiza el estado del jugador en función de las órdenes dadas por el usuario
        */
        virtual void behave () ;

};

}

#endif

// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef PlayerHeadAndHeels_hpp_
#define PlayerHeadAndHeels_hpp_

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

class PlayerHeadAndHeels : public UserControlled
{

public:

       /**
        * Constructor
        * @param item Elemento que tiene este comportamiento
        * @param id Identificador del comportamiento
        */
        PlayerHeadAndHeels( Item * item, const BehaviorId & id ) ;

        virtual ~PlayerHeadAndHeels( ) ;

       /**
        * Actualiza el comportamiento del elemento en cada ciclo
        * @return false si la actualización implica la destrucción del elemento o true en caso contrario
        */
        virtual bool update () ;

       /**
        * Actualiza el estado del jugador en función de las órdenes dadas por el usuario
        */
        virtual void behave () ;

protected:

       /**
        * Pone en espera al jugador. Implica la presentación del primer fotograma de la
        * secuencia de animación según la orientación del jugador y ponerlo a parpadear si pasan
        * entre 5 y 9 segundos
        * @param playerItem El jugador que va a cambiar su comportamiento
        */
        virtual void wait( PlayerItem * playerItem ) ;

       /**
        * Hace parpadear al jugador. El parpadeo se interrumpirá ante cualquier orden del usuario
        * @param playerItem El jugador que va a cambiar su comportamiento
        */
        virtual void blink( PlayerItem * playerItem ) ;

protected:

       /**
        * Fotogramas de parpadeo del jugador. Uno por cada punto cardinal, aunque aquellos fotogramas en los que
        * el personaje está de espaldas no son diferentes a los habituales
        */
        int blinkFrames[ 4 ] ;

};

}

#endif

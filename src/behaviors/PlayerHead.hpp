// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef PlayerHead_hpp_
#define PlayerHead_hpp_

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

class PlayerHead : public UserControlled
{

public:

        PlayerHead( Item* item, const std::string& behavior ) ;

        virtual ~PlayerHead( ) ;

        virtual bool update () ;

        virtual void behave () ;

protected:

       /**
        * Pone en espera al jugador. Implica la presentación del primer fotograma de la
        * secuencia de animación según la orientación del jugador y ponerlo a parpadear si pasan
        * entre 5 y 9 segundos
        * @param playerItem El jugador que va a cambiar su comportamiento
        */
        virtual void wait ( PlayerItem* playerItem ) ;

       /**
        * Hace parpadear al jugador. El parpadeo se interrumpirá ante cualquier orden del usuario
        * @param playerItem El jugador que va a cambiar su comportamiento
        */
        virtual void blink ( PlayerItem* playerItem ) ;

protected:

        int blinkFrames[ howManyBlinkFrames ] ;

};

}

#endif

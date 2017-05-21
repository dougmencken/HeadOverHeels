// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef KindOfActivity_hpp_
#define KindOfActivity_hpp_

#include <vector>

#include "Ism.hpp"


namespace isomot
{

class Behavior ;
class Item ;

/**
 * Abstraction of item’s current activity
 */

class KindOfActivity
{

public:

        KindOfActivity( ) ;

        virtual ~KindOfActivity( ) ;

       /**
        * Estado que mueve a un elemento
        * @param behavior Comportamiento que emplea el estado de movimiento
        * @param activity Un subestado que concreta la dirección de movimiento
        * @param canFall Indica si el elemento puede caer. Si se dan las condiciones para la caída
        *                se producirá el cambio de estado correspondiente
        * @return true si se produjo el movimiento o el cambio de estado; o, false si hubo colisión
        */
        virtual bool move ( Behavior * behavior, ActivityOfItem * activity, bool canFall ) ;

       /**
        * Estado que desplaza a un elemento
        * @param behavior Comportamiento que emplea el estado de desplazamiento
        * @param activity Un subestado que concreta la dirección de desplazamiento
        * @param canFall Indica si el elemento puede caer. Si se dan las condiciones para la caída
        *                se producirá el cambio de estado correspondiente
        * @return true si se produjo el desplazamiento o el cambio de estado; o, false si hubo colisión
        */
        virtual bool displace ( Behavior * behavior, ActivityOfItem * activity, bool canFall ) ;

       /**
        * Estado que hace caer a un elemento
        * @param behavior Comportamiento que emplea el estado de caída
        * @return true si se produjo la caída o false si hubo colisión
        */
        virtual bool fall ( Behavior * behavior ) ;

       /**
        * Estado que hace saltar a un elemento
        * @param behavior Comportamiento que emplea el estado de salto
        * @param jumpMatrix Matriz que define el salto del jugador. Cada par de valores define el desplazamiento
        *                   en el eje X o Y y el desplazamiento en el eje Z por cada ciclo
        * @param jumpIndex Índice que indica la fase del salto que se está ejecutando
        * @return true si se produjo el salto o false si hubo colisión
        */
        virtual bool jump ( Behavior * behavior, ActivityOfItem * activity, const std::vector< JumpMotion >& jumpMatrix, int * jumpIndex ) ;

protected:

        void changeKindOfActivity ( Behavior * behavior, KindOfActivity * newKind ) ;

       /**
        * Change activity of items that collide with the sender
        */
        virtual void propagateActivityToAdjacentItems ( Item * sender, const ActivityOfItem & activity ) ;

       /**
        * Change activity of items above the sender
        */
        virtual void propagateActivityToTopItems ( Item * sender, const ActivityOfItem & activity ) ;

};

}

#endif

// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef MoveState_hpp_
#define MoveState_hpp_

#include "Ism.hpp"
#include "BehaviorState.hpp"


namespace isomot
{

class Behavior ;
class FreeItem ;

/**
 * Movimiento de un elemento
 */

class MoveState : public BehaviorState
{

protected:

        MoveState( ) ;

public:

        virtual ~MoveState( ) ;

        /**
         * Único objeto de esta clase para toda la aplicación
         * @return Un puntero a la clase madre para hacer uso del polimorfismo de clase
         */
        static BehaviorState * getInstance () ;

        /**
         * Estado que mueve a un elemento
         * @param behavior Comportamiento usuario del estado de movimiento
         * @param substate Un subestado que concreta la dirección de movimiento
         * @param canFall Indica si el elemento puede caer. Si se dan las condiciones para la caída
         *                se producirá el cambio de estado correspondiente
         * @return true si se produjo el movimiento o el cambio de estado; o, false si hubo colisión
         */
        virtual bool move ( Behavior * behavior, StateId * substate, bool canFall ) ;

protected:

        /**
         * Operación recursiva que levanta todos los elementos apilados sobre un elemento
         * @param freeItem Primer elemento de la pila
         * @param z Unidades que ascenderá la pila de elementos
         */
        void ascent ( FreeItem * freeItem, int z ) ;

        /**
         * Operación recursiva que hace descender todos los elementos apilados sobre un elemento
         * @param freeItem Primer elemento de la pila
         * @param z Unidades que descenderá la pila de elementos
         */
        void descend ( FreeItem * freeItem, int z ) ;

private:

        /**
         * Único objeto de esta clase para toda la aplicación
         */
        static BehaviorState * instance ;

};

}

#endif

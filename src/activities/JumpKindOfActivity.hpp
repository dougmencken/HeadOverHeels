// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef JumpKindOfActivity_hpp_
#define JumpKindOfActivity_hpp_

#include <vector>
#include "KindOfActivity.hpp"
#include "Ism.hpp"


namespace isomot
{

class Behavior;
class FreeItem;

/**
 * Salto de un elemento
 */

class JumpKindOfActivity : public KindOfActivity
{

protected:

        JumpKindOfActivity();

public:

        virtual ~JumpKindOfActivity();

       /**
        * Único objeto de esta clase para toda la aplicación
        * @return Un puntero a la clase madre para hacer uso del polimorfismo de clase
        */
        static KindOfActivity * getInstance() ;

       /**
        * Estado que hace saltar a un elemento
        * @param behavior Comportamiento que necesita el estado de salto
        * @param jumpMatrix Matriz que define el salto del jugador. Cada par de valores define el desplazamiento
        *                   en el eje X o Y y el desplazamiento en el eje Z por cada ciclo
        * @param jumpIndex Índice que indica la fase del salto que se está ejecutando
        * @return true si se produjo el salto o false si hubo colisión
        */
        virtual bool jump ( Behavior * behavior, ActivityOfItem * activity, const std::vector < JumpMotion >& jumpMatrix, int * jumpIndex ) ;

protected:

       /**
        * Operación recursiva que levanta todos los elementos apilados sobre el jugador
        * @param sender Jugador que desencadena el levantamiento de la pila
        * @param freeItem Primer elemento de la pila
        * @param z Unidades que ascenderá la pila de elementos
        */
        void lift ( FreeItem * sender, FreeItem * freeItem, int z ) ;

private:

       /**
        * Único objeto de esta clase para toda la aplicación
        */
        static KindOfActivity * instance ;

};

}

#endif

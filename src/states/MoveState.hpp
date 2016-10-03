// Head over Heels (A remake by helmántika.org)
//
// © Copyright 2008 Jorge Rodríguez Santos <jorge@helmantika.org>
// © Copyright 1987 Ocean Software Ltd. (Original game)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef MOVESTATE_HPP_
#define MOVESTATE_HPP_

#include "Ism.hpp"
#include "BehaviorState.hpp"

namespace isomot
{

// Declaraciones adelantadas
class Behavior;
class FreeItem;

/**
 * Movimiento de un elemento
 */
class MoveState : public BehaviorState
{
protected:

        MoveState();

public:

        virtual ~MoveState();

        /**
         * Único objeto de esta clase para toda la aplicación
         * @return Un puntero a la clase madre para hacer uso del polimorfismo de clase
         */
        static BehaviorState* getInstance();

        /**
         * Estado que mueve a un elemento
         * @param behavior Comportamiento usuario del estado de movimiento
         * @param substate Un subestado que concreta la dirección de movimiento
         * @param canFall Indica si el elemento puede caer. Si se dan las condiciones para la caída
         *                se producirá el cambio de estado correspondiente
         * @return true si se produjo el movimiento o el cambio de estado; o, false si hubo colisión
         */
        virtual bool move(Behavior* behavior, StateId* substate, bool canFall);

protected:

        /**
         * Operación recursiva que levanta todos los elementos apilados sobre un elemento
         * @param freeItem Primer elemento de la pila
         * @param z Unidades que ascenderá la pila de elementos
         */
        void ascent(FreeItem* freeItem, int z);

        /**
         * Operación recursiva que hace descender todos los elementos apilados sobre un elemento
         * @param freeItem Primer elemento de la pila
         * @param z Unidades que descenderá la pila de elementos
         */
        void descend(FreeItem* freeItem, int z);

private:

        /**
         * Único objeto de esta clase para toda la aplicación
         */
        static BehaviorState* instance;
};

}

#endif //MOVESTATE_HPP_

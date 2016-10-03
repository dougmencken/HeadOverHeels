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

#ifndef BEHAVIORSTATE_HPP_
#define BEHAVIORSTATE_HPP_

#include <vector>
#include "Ism.hpp"

namespace isomot
{

// Declaraciones adelantadas
class Behavior;
class Item;

/**
 * Base para la codificación del estado de un elemento con comportamiento
 */
class BehaviorState
{
public:

        BehaviorState();

        virtual ~BehaviorState();

        /**
         * Estado que mueve a un elemento
         * @param behavior Comportamiento que emplea el estado de movimiento
         * @param substate Un subestado que concreta la dirección de movimiento
         * @param canFall Indica si el elemento puede caer. Si se dan las condiciones para la caída
         *                se producirá el cambio de estado correspondiente
         * @return true si se produjo el movimiento o el cambio de estado; o, false si hubo colisión
         */
        virtual bool move(Behavior* behavior, StateId* substate, bool canFall);

        /**
         * Estado que desplaza a un elemento
         * @param behavior Comportamiento que emplea el estado de desplazamiento
         * @param substate Un subestado que concreta la dirección de desplazamiento
         * @param canFall Indica si el elemento puede caer. Si se dan las condiciones para la caída
         *                se producirá el cambio de estado correspondiente
         * @return true si se produjo el desplazamiento o el cambio de estado; o, false si hubo colisión
         */
        virtual bool displace(Behavior* behavior, StateId* substate, bool canFall);

        /**
         * Estado que hace caer a un elemento
         * @param behavior Comportamiento que emplea el estado de caída
         * @return true si se produjo la caída o false si hubo colisión
         */
        virtual bool fall(Behavior* behavior);

  /**
   * Estado que hace saltar a un elemento
   * @param behavior Comportamiento que emplea el estado de salto
   * @param jumpMatrix Matriz que define el salto del jugador. Cada par de valores define el desplazamiento
   *                   en el eje X o Y y el desplazamiento en el eje Z por cada ciclo
   * @param jumpIndex Índice que indica la fase del salto que se está ejecutando
   * @return true si se produjo el salto o false si hubo colisión
   */
  virtual bool jump(Behavior* behavior, StateId* substate, const std::vector<JumpMotion>& jumpMatrix, int* jumpIndex);

protected:

  /**
   * Cambio de estado
   * @param behavior El comportamiento que va a cambiar de estado
   * @param state El nuevo estado
   */
  void changeState(Behavior* behavior, BehaviorState* state);

  /**
   * Cambia el estado de todos los elementos que hayan colisionado con el emisor
   * @param sender Elemento que propaga el estado
   * @param stateId Identificador del estado
   */
  virtual void propagateStateAdjacentItems(Item* sender, const StateId& stateId);

  /**
   * Cambia el estado de los elementos que haya encima del emisor
   * @param sender Elemento que propaga el estado
   * @param stateId Identificador del estado
   */
  virtual void propagateStateTopItems(Item* sender, const StateId& stateId);
};

}

#endif //BEHAVIORSTATE_HPP_

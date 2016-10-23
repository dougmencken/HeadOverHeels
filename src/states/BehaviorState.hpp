// The free and open source remake of Head over Heels
//
// Copyright © 2016 Douglas Mencken dougmencken @ gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef BehaviorState_hpp_
#define BehaviorState_hpp_

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

#endif

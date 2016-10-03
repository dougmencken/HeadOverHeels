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

#ifndef JUMPSTATE_HPP_
#define JUMPSTATE_HPP_

#include <vector>
#include "BehaviorState.hpp"
#include "Ism.hpp"

namespace isomot
{

// Declaraciones adelantadas
class Behavior;
class FreeItem;

/**
 * Salto de un elemento
 */
class JumpState : public BehaviorState
{
protected:

	JumpState();

public:

	virtual ~JumpState();

  /**
   * Único objeto de esta clase para toda la aplicación
   * @return Un puntero a la clase madre para hacer uso del polimorfismo de clase
   */
  static BehaviorState* getInstance();

  /**
   * Estado que hace saltar a un elemento
   * @param behavior Comportamiento que necesita el estado de salto
   * @param jumpMatrix Matriz que define el salto del jugador. Cada par de valores define el desplazamiento
   *                   en el eje X o Y y el desplazamiento en el eje Z por cada ciclo
   * @param jumpIndex Índice que indica la fase del salto que se está ejecutando
   * @return true si se produjo el salto o false si hubo colisión
   */
  virtual bool jump(Behavior* behavior, StateId* substate, const std::vector<JumpMotion>& jumpMatrix, int* jumpIndex);

protected:

  /**
   * Operación recursiva que levanta todos los elementos apilados sobre el jugador
   * @param sender Jugador que desencadena el levantamiento de la pila
   * @param freeItem Primer elemento de la pila
   * @param z Unidades que ascenderá la pila de elementos
   */
  void lift(FreeItem* sender, FreeItem* freeItem, int z);

private:

  /**
   * Único objeto de esta clase para toda la aplicación
   */
  static BehaviorState* instance;
};

}

#endif //JUMPSTATE_HPP_

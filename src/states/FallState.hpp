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

#ifndef FALLSTATE_HPP_
#define FALLSTATE_HPP_

#include <stack>
#include "Ism.hpp"
#include "BehaviorState.hpp"

namespace isomot
{

// Declaraciones adelantadas
class Behavior;
class FreeItem;

/**
 * Caída de un elemento
 */
class FallState : public BehaviorState
{
protected:

	FallState();

public:

	virtual ~FallState();

  /**
   * Único objeto de esta clase para toda la aplicación
   * @return Un puntero a la clase madre para hacer uso del polimorfismo de clase
   */
	static BehaviorState* getInstance();

  /**
   * Estado que hace caer a un elemento
   * @param behavior Comportamiento usuario del estado de caída
   * @return true si se produjo la caída o false si hubo colisión
   */
	virtual bool fall(Behavior* behavior);

private:

  void assignAnchor(FreeItem* freeItem, std::stack<int> bottomItems);

private:

  /**
   * Único objeto de esta clase para toda la aplicación
   */
  static BehaviorState* instance;
};

}

#endif //FALLSTATE_HPP_

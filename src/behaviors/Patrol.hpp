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

#ifndef PATROL_HPP_
#define PATROL_HPP_

#include "Behavior.hpp"
#include "HPC.hpp"

namespace isomot
{

// Declaraciones adelantadas
class Item;

/**
 * Mueve aleatoriamente al elemento dependiendo del tipo de comportamiento. En el
 * caso del patrullero-4c se mueve en las direcciones norte, sur, este u oeste; en
 * el caso del patrullero-4d lo hace en las direcciones noreste, noroeste, sudeste
 * y sudoeste; y, en el caso del patrullero-8 lo hace en todas las direcciones
 */
class Patrol : public Behavior
{
public:

  /**
   * Constructor
   * @param item Elemento que tiene este comportamiento
   * @param id Identificador de comportamiento: Patrol4cBehavior, Patrol4dBehavior
   * o Patrol8Behavior
   */
	Patrol(Item* item, const BehaviorId& id);

	virtual ~Patrol();

  /**
   * Actualiza el comportamiento del elemento en cada ciclo
   * @return false si la actualización implica la destrucción del elemento o true en caso contrario
   */
  virtual bool update();

protected:

  /**
   * El elemento cambia aleatoriamente de dirección
   */
  void changeDirection();

private:

  /**
   * Cronómetro que controla la velocidad de movimiento del elemento
   */
  HPC* speedTimer;

  /**
   * Cronómetro que controla la velocidad de caída del elemento
   */
  HPC* fallenTimer;

  /**
   * Cronómetro que controla el cambio de dirección del elemento
   */
  HPC* changeTimer;
};

}

#endif //PATROL_HPP_

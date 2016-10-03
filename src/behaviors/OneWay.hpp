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

#ifndef ONEWAY_HPP_
#define ONEWAY_HPP_

#include "Behavior.hpp"
#include "HPC.hpp"

namespace isomot
{

// Declaraciones adelantadas
class Item;

/**
 * Mueve al elemento en un único sentido. Cuando éste choca con algo gira 180º y prosigue la
 * marcha en sentido contrario
 */
class OneWay : public Behavior
{
public:

  /**
   * Constructor
   * @param item Elemento que tiene este comportamiento
   * @param id Identificador del comportamiento
   * @param isFlying Indica si el elemento vuela
   */
	OneWay(Item* item, const BehaviorId& id, bool isFlying);

	virtual ~OneWay();

  /**
   * Actualiza el comportamiento del elemento en cada ciclo
   * @return false si la actualización implica la destrucción del elemento o true en caso contrario
   */
  virtual bool update();

protected:

  /**
   * Acciones a realizar cuando el elemento está en un estado ajeno a los estados propios de su
   * comportamiento
   */
  void start();

  /**
   * El elemento da media vuelta
   */
  void turnRound();

private:

  /**
   * Indica si el elemento vuela
   */
  bool isFlying;

  /**
   * Cronómetro que controla la velocidad de movimiento del elemento
   */
  HPC* speedTimer;

  /**
   * Cronómetro que controla la velocidad de caída del elemento
   */
  HPC* fallenTimer;
};

}

#endif //ONEWAY_HPP_

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

#ifndef MOBILE_HPP_
#define MOBILE_HPP_

#include "Behavior.hpp"
#include "HPC.hpp"

namespace isomot
{

// Declaraciones adelantadas
class Item;

/**
 * Elemento que puede desplazarse sólo al ser empujado por otros, ya que
 * carece de movimiento autónomo
 */
class Mobile : public Behavior
{
public:

  /**
   * Constructor
   * @param item Elemento que tiene este comportamiento
   * @param id Identificador de comportamiento
   */
  Mobile(Item* item, const BehaviorId& id);

  virtual ~Mobile();

  /**
   * Actualiza el comportamiento del elemento en cada ciclo
   * @return false si la actualización implica la destrucción del elemento o true en caso contrario
   */
  virtual bool update();

private:

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

#endif //MOBILE_HPP_

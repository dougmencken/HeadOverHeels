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

#ifndef VOLATILE_HPP_
#define VOLATILE_HPP_

#include "Behavior.hpp"
#include "HPC.hpp"
#include "Ism.hpp"

namespace isomot
{

// Declaraciones adelantadas
class Item;
class ItemData;

/**
 * Elemento que puede desplazarse sólo al ser empujado por otros, ya que
 * carece de movimiento autónomo
 */
class Volatile : public Behavior
{
public:

  /**
   * Constructor
   * @param item Elemento que tiene este comportamiento
   * @param id Identificador de comportamiento
   */
  Volatile(Item* item, const BehaviorId& id);

  virtual ~Volatile();

  /**
   * Actualiza el comportamiento del elemento en cada ciclo
   * @return false si la actualización implica la destrucción del elemento o true en caso contrario
   */
  virtual bool update();

protected:

  /**
   * Datos del elemento empleado como disparo
   */
  ItemData* bubblesData;

private:

  /**
   * Indica si el elemento deja de ser volátil por haber activado un interruptor en la sala
   */
  bool solid;

  /**
   * Cronómetro que controla el tiempo de existencia del elemento volátil
   */
  HPC* destroyTimer;

public: // Operaciones de consulta y actualización

  /**
   * Asigna los datos del elemento usado como transición en la destrucción del volátil
   */
  void setExtraData(void* data);
};

}

#endif //VOLATILE_HPP_


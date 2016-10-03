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

#ifndef HUNTER_HPP_
#define HUNTER_HPP_

#include "Behavior.hpp"
#include "HPC.hpp"

namespace isomot
{

// Declaraciones adelantadas
class Item;
class ItemData;

/**
 * Intenta dar caza al jugador moviéndose allá donde esté
 */
class Hunter : public Behavior
{
public:

  /**
   * Constructor
   * @param item Elemento que tiene este comportamiento
   * @param id Identificador del comportamiento
   */
  Hunter(Item* item, const BehaviorId& id);

  virtual ~Hunter();

  /**
   * Actualiza el comportamiento del elemento en cada ciclo
   * @return false si la actualización implica la destrucción del elemento o true en caso contrario
   */
  virtual bool update();

private:

  /**
   * Calcula el sentido en el que debe avanzar el elemento para poder cazar al jugador activo
   * @return El estado que indica la dirección calculada
   */
  StateId calculateDirection(const StateId& stateId);

  /**
   * Calcula el sentido en el que debe avanzar el elemento para poder cazar al jugador activo
   * La dirección está restringida a los puntos cardinales básicos
   * @return El estado que indica la dirección calculada
   */
  StateId calculateDirection4(const StateId& stateId);

  /**
   * Calcula el sentido en el que debe avanzar el elemento para poder cazar al jugador activo
   * La dirección puede ser cualquier de las ocho direcciones posibles
   * @return El estado que indica la dirección calculada
   */
  StateId calculateDirection8(const StateId& stateId);

  /**
   * Crea para los cazadores ocultos el guarda imperial completo
   * @return true si se creó o false si todavía no se puede crear
   */
  bool createFullBody();

private:

  /**
   * Cronómetro que controla la velocidad de movimiento del elemento
   */
  HPC* speedTimer;

  /**
   * Datos del guarda imperial
   */
  ItemData* guardData;

public: // Operaciones de consulta y actualización

  /**
   * Asigna los datos del elemento usado como disparo
   */
  void setExtraData(void* data);

};

}

#endif //HUNTER_HPP_

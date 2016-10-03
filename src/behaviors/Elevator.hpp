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

#ifndef ELEVATOR_HPP_
#define ELEVATOR_HPP_

#include "Behavior.hpp"
#include "HPC.hpp"

namespace isomot
{

// Declaraciones adelantadas
class Item;

/**
 * Mueve a los elementos situados encima hacia arriba o hacia abajo
 */
class Elevator : public Behavior
{
public:

  /**
   * Constructor
   * @param item Elemento que tiene este comportamiento
   * @param id Identificador del comportamiento
   */
  Elevator(Item* item, const BehaviorId& id);

  virtual ~Elevator();

  /**
   * Actualiza el comportamiento del elemento en cada ciclo
   * @return false si la actualización implica la destrucción del elemento o true en caso contrario
   */
  virtual bool update();

private:

  /**
   * Altura máxima a la que asciende el ascensor
   */
  int top;

  /**
   * Altura mínima a la que desciende el ascensor
   */
  int bottom;

  /**
   * Indica si el primer movimiento que realiza el ascensor es ascendente (true) o descendente (false)
   */
  bool ascent;

  /**
   * Estado válido. Los ascensores no se pueden desplazar, por esta razón se utiliza este atributo
   * para retornar a un estado válido cuando algún elemento cambie el estado del ascensor debido a
   * una colisión
   */
  StateId validState;

  /**
   * Cronómetro que controla la velocidad de movimiento del elemento
   */
  HPC* speedTimer;

  /**
   * Cronómetro que controla los tiempos de parada del ascensor. Suceden cuando cambia de dirección
   */
  HPC* stopTimer;

public: // Operaciones de consulta y actualización

  /**
   * Asigna los datos de altura máxima y mínima, así como el movimiento inicial del ascensor
   */
  void setExtraData(void* data);

};

}

#endif //ELEVATOR_HPP_

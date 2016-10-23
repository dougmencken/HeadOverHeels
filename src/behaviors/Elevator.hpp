// The free and open source remake of Head over Heels
//
// Copyright © 2016 Douglas Mencken dougmencken @ gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Elevator_hpp_
#define Elevator_hpp_

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

#endif

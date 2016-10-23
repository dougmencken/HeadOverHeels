// The free and open source remake of Head over Heels
//
// Copyright © 2016 Douglas Mencken dougmencken @ gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Hunter_hpp_
#define Hunter_hpp_

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

#endif

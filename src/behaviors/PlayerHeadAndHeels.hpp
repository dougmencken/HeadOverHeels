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

#ifndef PLAYERHEADANDHEELS_HPP_
#define PLAYERHEADANDHEELS_HPP_

#include "UserControlled.hpp"

namespace isomot
{

// Declaraciones adelantadas
class Item;
class ItemData;

/**
 * Comportamiento del jugador Head. Dado que Head es uno de los elementos controlados por el usuario
 * su comportamiento viene definido tanto por la operación <update> como por <execute>
 * Head puede caminar, saltar, planear y disparar, como acciones más significativas
 */
class PlayerHeadAndHeels : public UserControlled
{
public:

  /**
   * Constructor
   * @param item Elemento que tiene este comportamiento
   * @param id Identificador del comportamiento
   */
  PlayerHeadAndHeels(Item* item, const BehaviorId& id);

  virtual ~PlayerHeadAndHeels();

  /**
   * Actualiza el comportamiento del elemento en cada ciclo
   * @return false si la actualización implica la destrucción del elemento o true en caso contrario
   */
  virtual bool update();

  /**
   * Actualiza el estado del jugador en función de las órdenes dadas por el usuario
   */
  virtual void execute();

protected:

  /**
   * Pone en espera al jugador. Implica la presentación del primer fotograma de la
   * secuencia de animación según la orientación del jugador y ponerlo a parpadear si pasan
   * entre 5 y 9 segundos
   * @param playerItem El jugador que va a cambiar su comportamiento
   */
  virtual void wait(PlayerItem* playerItem);

  /**
   * Hace parpadear al jugador. El parpadeo se interrumpirá ante cualquier orden del usuario
   * @param playerItem El jugador que va a cambiar su comportamiento
   */
  virtual void blink(PlayerItem* playerItem);

protected:

  /**
   * Fotogramas de parpadeo del jugador. Uno por cada punto cardinal, aunque aquellos fotogramas en los que
   * el personaje está de espaldas no son diferentes a los habituales
   */
  int blinkFrames[4];
};

}

#endif /*PLAYERHEADANDHEELS_HPP_*/

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

#ifndef ISOMOT_HPP_
#define ISOMOT_HPP_

#include <allegro.h>
#ifdef __WIN32
  #include <winalleg.h>
#endif
#include "csxml/SaveGameXML.hpp"

namespace isomot
{

// Declaraciones adelantadas
class ItemDataManager;
class MapManager;

/**
 * El motor isométrico
 */
class Isomot
{
public:

  Isomot();

  virtual ~Isomot();

  /**
   * Crea las salas iniciales y pone en marcha el motor isométrico
   */
  void start();

  /**
   * Crea dos salas a partir de los datos contenidos en el archivo XML donde se guarda una partida
   * y pone en marcha el motor isométrico
   */
  void start(const sgxml::players::player_sequence& playerSequence);

  /**
   * Detiene el motor isométrico
   */
  void stop();

  /**
   * Pone en marcha el motor isométrico después de una parada
   */
  void restart();

  /**
   * Prepara el motor para iniciar una nueva partida
   */
  void reset();

  /**
   * Actualiza el motor isométrico. La actualización se realiza en dos subprocesos diferenciados:
   * Por un lado se actualizan todas las maquinas de estados de los elementos del juegos y por otro
   * lado se dibuja la sala activa
   * @return La imagen donde se ha dibujado la vista isométrica
   */
  BITMAP* update();

private:

  /**
   * Gestiona la sala final del juego. Es una sala muy especial porque el usuario no tiene el
   * control de ningún jugador
   */
  void updateEndRoom();

private:

  /**
   * Indica si el motor está gestionando la sala final del juego
   */
  bool isEndRoom;

  /**
   * Imagen donde se dibuja la vista isométrica
   */
  BITMAP* view;

  /**
   * Gestor de datos de los elementos del juego
   */
  ItemDataManager* itemDataManager;

  /**
   * Gestor del mapa
   */
  MapManager* mapManager;

public: // Operaciones de consulta y actualización

  /**
   * Gestor de datos de los elementos del juego
   * @return Un puntero al gestor
   */
  ItemDataManager* getItemDataManager() const;

  /**
   * Gestor del mapa
   * @return Un puntero al gestor
   */
  MapManager* getMapManager() const;
};

}

#endif //ISOMOT_HPP_

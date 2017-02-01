// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef GameFileManager_hpp_
#define GameFileManager_hpp_

#include <string>
#include "Ism.hpp"
#include "csxml/SaveGameXML.hpp"

namespace isomot
{

// Declaraciones adelantadas
class GameManager;
class Isomot;
class MapManager;

/**
 * Gestiona la grabación y recuperación de una partida
 */
class GameFileManager
{
public:

  /**
   * Constructor
   * @param gameManager El gestor del juego
   * @param isomot El motor isométrico del juego
   */
	GameFileManager(const GameManager* gameManager, const Isomot* isomot);

	~GameFileManager();

  /**
   * Guarda los datos necesarios para grabar la partida
   * @param roomId Identificador de la sala donde se ha cogido el pez de la reencarnación
   * @param label Etiqueta del jugador que ha cogido el pez
   * @param x Coordenada X donde está situado el pez de la reencarnación
   * @param y Coordenada Y donde está situado el pez de la reencarnación
   * @param z Coordenada Z donde está situado el pez de la reencarnación
   * @param direction Dirección a la que mira el jugador en el momento de coger el pez
   */
	void assignFishData(const std::string& roomId, short label, int x, int y, int z, const Direction& direction);

  /**
   * Carga una partida de un archivo XML
   * @param fileName Nombre del archivo
   */
  void loadGame(const std::string& fileName);

  /**
   * Guarda una partida en un archivo XML
   * @param fileName Nombre del archivo
   * @pre El jugador ha tenido que comerse el pez, es decir, la partida se graba en disco sólo
   * si se han asignado los datos necesarios a través de assignFishData
   */
  void saveGame(const std::string& fileName);

private:

  /**
   * Establece el estado de los jugadores (vidas, bocina, bolso y rosquillas)
   * @param playerSequence Datos obtenidos de los jugadores de la partida cargada
   */
  void assignPlayerStatus(const sgxml::players::player_sequence& playerSequence);

private:

  /**
   * Identificador de la sala donde se ha cogido el pez de la reencarnación
   */
  std::string roomId;

  /**
   * Etiqueta del jugador que ha cogido el pez
   */
  short label;

  /**
   * Coordenada X donde está situado el pez de la reencarnación
   */
  int x;

  /**
   * Coordenada Y donde está situado el pez de la reencarnación
   */
  int y;

  /**
   * Coordenada Z donde está situado el pez de la reencarnación
   */
  int z;

  /**
   * Dirección a la que mira el jugador en el momento de coger el pez
   */
  Direction direction;

  /**
   * El gestor del juego
   */
  GameManager* gameManager;

  /**
   * El motor isométrico del juego
   */
  Isomot* isomot;

};

}

#endif

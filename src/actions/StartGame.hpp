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

#ifndef STARTGAME_H_
#define STARTGAME_H_

#include <allegro.h>
#include "Action.hpp"

namespace gui
{

/**
 * Inicia una nueva partida
 * */
class StartGame : public Action
{
public:

  /**
   * Constructor
   * @param destination Imagen donde se dibujará la interfaz gráfica
   * @param gameInProgress Indica si hay una partida en curso o comienza una nueva
   */
  StartGame(BITMAP* destination, bool gameInProgress);

	/**
   * Inicia una nueva partida, es decir, pone en marcha el gestor del juego
   */
  void execute();

private:

  /**
   * Imagen donde se dibujará la interfaz gráfica
   */
  BITMAP* destination;

  /**
   * Indica si hay una partida en curso o comienza una nueva
   */
  bool gameInProgress;
};

}

#endif /*STARTGAME_H_*/

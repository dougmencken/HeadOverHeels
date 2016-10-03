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

#ifndef SAVEGAME_HPP_
#define SAVEGAME_HPP_

#include <string>
#include <allegro.h>
#include "Action.hpp"

namespace gui
{

/**
 * Graba la partida en curso y regresa al juego
 */
class SaveGame : public Action
{
public:

  /**
   * Constructor
   * @param destination Imagen donde se dibujará la interfaz gráfica
   * @param slot Número del archivo a grabar
   */
	SaveGame(BITMAP* destination, int slot);

  /**
   * Ejecuta el contenido de la acción, es decir, graba la partida y devuelve el control al juego
   */
  void execute();

private:

  /**
   * Imagen donde se dibujará la interfaz gráfica
   */
  BITMAP* destination;

  /**
   * Número del archivo a grabar. Los archivos tienen un nombre de la forma savegameN.xml,
   * donde N es este número
   */
  int slot;
};

}

#endif /*SAVEGAME_HPP_*/

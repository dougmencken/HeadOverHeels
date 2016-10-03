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

#ifndef CREATECONGRATULATIONSSCREEN_HPP_
#define CREATECONGRATULATIONSSCREEN_HPP_

#include <allegro.h>
#include "Action.hpp"

namespace gui
{

/**
 * Presenta el texto final de felicitación cuando se termina con éxito el juego
 */
class CreateCongratulationsScreen : public Action
{
public:

  /**
   * Constructor
   * @param destination Imagen donde se dibujará la interfaz gráfica
   * @param rooms Número de salas visitadas
   * @param planets Número de planetas liberados
   */
	CreateCongratulationsScreen(BITMAP* destination, unsigned short rooms, unsigned short planets);

	/**
   * Crea la pantalla con el texto
   */
  void execute();

private:

  /**
   * Imagen donde se dibujará la interfaz gráfica
   */
  BITMAP* destination;

  /**
   * Número de salas visitadas
   */
  unsigned short rooms;

  /**
   * Número de planetas liberados
   */
  unsigned short planets;
};

}

#endif /*CREATECONGRATULATIONSSCREEN_HPP_*/

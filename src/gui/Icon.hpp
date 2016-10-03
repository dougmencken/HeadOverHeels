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

#ifndef ICON_HPP_
#define ICON_HPP_

#include <allegro.h>
#include "Widget.hpp"

namespace gui
{

/**
 * Una imagen estática presente en la pantalla
 */
class Icon : public Widget
{
public:

  /**
   * Constructor
   * @param x Coordenada X de pantalla donde situar el elemento
   * @param x Coordenada X de pantalla donde situar el elemento
   * @param image La imagen a presentar
   */
	Icon(unsigned int x, unsigned int y, BITMAP* image);

	virtual ~Icon();

  /**
   * Dibuja el elemento
   * @param destination Imagen donde será dibujado
   */
  void draw(BITMAP* destination);

private:

  /**
   * La imagen del icono
   */
  BITMAP* image;
};

}

#endif /*ICON_HPP_*/

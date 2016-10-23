// The free and open source remake of Head over Heels
//
// Copyright © 2016 Douglas Mencken dougmencken @ gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

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

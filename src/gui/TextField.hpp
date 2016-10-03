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

#ifndef TEXTFIELD_HPP_
#define TEXTFIELD_HPP_

#include <list>
#include <algorithm>
#include <functional>
#include <string>
#include <memory>
#include <allegro.h>
#include "Widget.hpp"

namespace gui
{

// Declaraciones adelantadas
class Label;

enum Alignment
{
  LeftAlignment,
  CenterAlignment,
  RightAlignment
};

/**
 * Almacena texto en varias líneas que puede alinearse a la izquierda, centro y derecha
 */
class TextField : public Widget
{
public:

  /**
   * Constructor
   * @param x Coordenada X de pantalla donde situar el elemento
   * @param x Coordenada X de pantalla donde situar el elemento
   * @param width Anchura del campo de texto
   * @param height Altura del campo de texto
   * @param alignment Alineación horizontal del texto: izquierda, centro o derecha
   */
  TextField(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const Alignment& alignment);

  virtual ~TextField();

  /**
   * Dibuja el elemento
   */
  virtual void draw(BITMAP* destination);

	/**
	 * Añade una nueva línea al campo de texto
	 */
	void addLine(const std::string& text, const std::string& font, const std::string& color);

  /**
   * Cambia la posición del elemento
   * @param x Coordenada X de pantalla donde situar el elemento
   * @param y Coordenada Y de pantalla donde situar el elemento
   */
	void changePosition(unsigned int x, unsigned int y);

private:

  /**
   * Anchura del campo de texto
   */
  unsigned int width;

  /**
   * Altura del campo de texto
   */
  unsigned int height;

  /**
   * Alineación horizontal del texto: izquierda, centro o derecha
   */
  Alignment alignment;

  /**
   * Distancia de interlineado respecto a la última línea añadida
   */
  unsigned int delta;

  /**
   * El campo de texto se compone de etiquetas
   */
  std::list<Label*> lines;
};

}

#endif /*TEXTFIELD_HPP_*/

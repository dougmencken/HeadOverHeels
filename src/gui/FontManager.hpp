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

#ifndef FONTMANAGER_HPP_
#define FONTMANAGER_HPP_

#include <list>
#include <string>
#include <algorithm>
#include <functional>
#include <allegro.h>

namespace gui
{

// Declaraciones adelantadas
class Font;

/**
 * Almacena las fuentes tipográficas usadas en la aplicación
 */
class FontManager
{
private:

	FontManager();

public:

	virtual ~FontManager();

  /**
   * Único objeto de esta clase para toda la aplicación
   * @return Un puntero a la clase madre para hacer uso del polimorfismo de clase
   */
  static FontManager* getInstance();

  /**
   * Crea una fuente tipográfica en función de los parámetros especificados
   * @param fontName Nombre de la fuente tipográfica empleado como identificador
   * @param fontFile Nombre del archivo de imagen que contienen la fuente tipográfica
   * @param color Color de los caracteres. El color se puede cambiar sólo si los caracteres son blancos
   * @param doubleSize Si vale true se doblará la altura de los caracteres
   */
  void createFont(const std::string& fontName, const std::string& fontFile, int color, bool doubleSize = false);

  /**
   * Busca una fuente tipográfica previamente creada
   * @param fontName Nombre de la fuente caligráfica
   * @param color Color de los caracteres de la fuente caligráfica
   * @return La fuente ó 0 si no existe
   */
  Font* findFont(const std::string& font, const std::string& color);

private:

  /**
   * Único objeto de esta clase para toda la aplicación
   */
  static FontManager* instance;

  /**
   * Conjunto de fuentes disponibles
   */
  std::list<Font*> fonts;
};

/**
 * Objeto-función usado como predicado en la búsqueda de una fuente tipográfica por su identificador
 */
class EqualFont : public std::binary_function<Font*, std::string, bool>
{
public:
  bool operator()(const Font* font, const std::string& fontName) const;
};

}

#endif /*FONTMANAGER_HPP_*/

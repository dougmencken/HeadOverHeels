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

#ifndef SELECTLANGUAGE_HPP_
#define SELECTLANGUAGE_HPP_

#include <string>
#include <allegro.h>
#include "Action.hpp"

namespace gui
{

/**
 * Establece el idioma del juego
 */
class SelectLanguage: public Action
{
public:

  /**
   * Constructor
   * @param destination Imagen donde se dibujará la interfaz gráfica
   * @param language Código ISO del idioma seleccionado
   */
  SelectLanguage(BITMAP* destination, const std::string& language);

  /**
   * Ejecuta el contenido de la acción, es decir, establece el idioma del juego
   */
  void execute();

private:

  /**
   * Imagen donde se dibujará la interfaz gráfica
   */
  BITMAP* destination;

  /**
   * Código ISO del idioma seleccionado
   */
  std::string language;
};

}

#endif /* SELECTLANGUAGE_HPP_ */

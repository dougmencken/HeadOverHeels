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

#ifndef TRANSPARENCYMANAGER_HPP_
#define TRANSPARENCYMANAGER_HPP_

#include <vector>

namespace isomot
{

/**
 * Gestor de la transparencia de los elementos libres
 */
class TransparencyManager
{
public:

  /**
   * Inicializa la tabla de transparencias
   */
  TransparencyManager();

  virtual ~TransparencyManager();

  /**
   * Añade un nuevo elemento a la tabla
   * @param percent Porcentaje de transparencia del elemento
   */
  void add(const unsigned char percent);

  /**
   * Elimina un elemento de la tabla
   * @param percent Porcentaje de transparencia del elemento
   */
  void remove(const unsigned char percent);

private:

	/**
	 * Tabla de transparencias. Una estructura ordenada de 101 elementos donde el índice indica el
	 * porcentaje de transparencia (de 0 a 100) y su valor el número de elementos que tienen dicho
	 * porcentaje en la sala
	 */
	std::vector<unsigned int> table;

public: // Operaciones de consulta y actualización

  /**
   * Número de elementos de la tabla con un determinado porcentaje de transparencia
   * @param Porcentaje de transparencia consultado
   * @return Número de elementos con ese porcentaje de transparencia
   */
  unsigned int getTransparencyItems(unsigned int percent) const { return (percent >= 0 && percent <= 100 ? this->table[percent] : 0); }
};

}

#endif // TRANSPARENCYMANAGER_HPP_

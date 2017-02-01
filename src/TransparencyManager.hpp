// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef TransparencyManager_hpp_
#define TransparencyManager_hpp_

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

#endif

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

#ifndef ITEMDATAMANAGER_HPP_
#define ITEMDATAMANAGER_HPP_

#include <string>
#include <list>
#include <algorithm>
#include <iostream>
#include <functional>
#include <allegro.h>
#include "ItemData.hpp"
#include "csxml/ItemsXML.hpp"

namespace isomot
{

/**
 * Gestor de datos de los elementos del juego. Extrae la información a partir de la lectura de un
 * archivo XML, creando una lista de datos que incluye también los gráficos y las sombras de cada
 * elemento
 */
class ItemDataManager
{
public:

	/**
	 * Constructor
	 * @param fileName Nombre del archivo XML que contiene los datos de los elementos
	 */
	ItemDataManager(const std::string& fileName);

	virtual ~ItemDataManager();

	/**
	 * Crea la lista de datos a partir de la información extraída por el gestor XML del archivo de
	 * definición de elementos
	 */
	void load();

	/**
	 * Busca un elemento en la lista
	 * @param label El identificador del elemento a encontrar
	 * @return Un registro con los datos del elemento ó 0 si la búsqueda fracasó
	 */
	ItemData* find(const short label);

protected:

  /**
   * Extracción de los fotogramas de un elemento a partir de una imagen
   * @param itemData Un elemento con los campos bitmap, frameWidth y frameHeight no nulos
   * @return El elemento con el campo motion almacenando los fotogramas ó 0 si se produjo un error
   */
  ItemData* createBitmapFrames(ItemData* itemData);

  /**
   * Extracción de los fotogramas de las sombras de un elemento a partir de una imagen
   * @param itemData Un elemento con los campos shadow, shadowWidth y shadowHeight no nulos
   * @return El elemento con el campo motion almacenando los fotogramas ó 0 si se produjo un error
   */
  ItemData* createShadowFrames(ItemData* itemData);

private:

  /**
   * Dimensiones de las partes de una puerta
   */
  class DoorMeasures
  {
  public:

    /**
     * Constructor. Inicializa todos los campos del registro de datos
     */
    DoorMeasures();

    /**
     * Anchura espacial del dintel en el eje X
     */
    int lintelWidthX;

    /**
     * Anchura espacial del dintel en el eje Y
     */
    int lintelWidthY;

    /**
     * Altura espacial del dintel
     */
    int lintelHeight;

    /**
     * Anchura espacial de la jamba izquierda en el eje X
     */
    int leftJambWidthX;

    /**
     * Anchura espacial de la jamba izquierda en el eje Y
     */
    int leftJambWidthY;

    /**
     * Altura espacial de la jamba izquierda
     */
    int leftJambHeight;

    /**
     * Anchura espacial de la jamba derecha en el eje X
     */
    int rightJambWidthX;

    /**
     * Anchura espacial de la jamba derecha en el eje Y
     */
    int rightJambWidthY;

    /**
     * Altura espacial de la jamba derecha
     */
    int rightJambHeight;
  };

  /**
   * Extrae el dintel de una puerta
   * @param door Un gráfico con una puerta al completo
   * @param dm Medidas espaciales de las partes de la puerta
   * @param type Punto cardinal al que está orientado el vano de la puerta
   * @return Un gráfico con el dintel de la puerta ó 0 si se produjo algún error
   */
  BITMAP* cutOutLintel(BITMAP* door, const DoorMeasures& dm, const ixml::door::value type);

  /**
   * Extrae la jamba izquierda de una puerta
   * @param door Un gráfico con una puerta al completo
   * @param dm Medidas espaciales de las partes de la puerta
   * @param type Punto cardinal al que está orientado el vano de la puerta
   * @return Un gráfico con la jamba izquierda de la puerta ó 0 si se produjo algún error
   */
  BITMAP* cutOutLeftJamb(BITMAP* door, const DoorMeasures& dm, const ixml::door::value type);

  /**
   * Extrae la jamba derecha de una puerta
   * @param door Un gráfico con una puerta al completo
   * @param dm Medidas espaciales de las partes de la puerta
   * @param type Punto cardinal al que está orientado el vano de la puerta
   * @return Un gráfico con la jamba derecha de la puerta ó 0 si se produjo algún error
   */
  BITMAP* cutOutRightJamb(BITMAP* door, const DoorMeasures& dm, const ixml::door::value type);

  /**
   * Predicado unario de std::for_each para la destrucción de un elemento
   * @param itemData Un elemento del contenedor
   */
  static void destroyItemData(ItemData& itemData);

protected:

  /**
   * Nombre del archivo XML que contiene los datos de los elementos
   */
  std::string fileName;

  /**
   * Datos de todos los elementos del juego
   */
  std::list<ItemData> itemData;
};

/**
 * Objeto-función usado como predicado en la búsqueda de los datos de un elemento del juego
 */
struct EqualItemData : public std::binary_function<ItemData, short, bool>
{
  bool operator()(const ItemData& itemData, short label) const;
};

}

#endif //ITEMDATAMANAGER_HPP_

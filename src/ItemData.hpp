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

#ifndef ITEMDATA_HPP_
#define ITEMDATA_HPP_

#include <vector>
#include <string>
#include <allegro.h>

namespace isomot
{

/**
 * Registro que almacena la definición de un elemento del juego leído de un archivo del disco
 */
class ItemData
{
public:

  /**
   * Constructor. Inicializa todos los campos del registro de datos
   */
  ItemData();

  virtual ~ItemData();

public:

  /**
   * Etiqueta unívoca del elemento, es decir, no puede haber dos elementos de distinto tipo con la
   * misma etiqueta. Las etiquetas son constantes y no cadenas de caracteres
   */
  short label;

  /**
   * Anchura espacial del elemento en el eje X
   */
  int widthX;

  /**
   * Anchura espacial del elemento en el eje Y
   */
  int widthY;

  /**
   * Altura espacial del elemento
   */
  int height;

  /**
   * Indica cuántos fotogramas diferentes tiene el elemento para cada una de las direcciones
   * posibles: norte, sur, este y oeste. Los valores posibles son 1, 2 y 4
   */
  unsigned char directionFrames;

  /**
   * Indica si el elemento es mortal, es decir, si al tocarlo el jugador perderá una vida
   */
  bool mortal;

  /**
   * El peso del elemento en segundos. Puede ser 0 para indicar que aunque quede suspendido en el
   * aire no caerá o un valor distinto para indicar lo contrario. Cuanto mayor sea este valor mayor
   * será la velocidad de caída
   */
  double weight;

  /**
   * Tiempo en segundos que será mostrado cada fotograma de la secuencia de animación
   */
  double framesDelay;

  /**
   * Tiempo en segundos necesario para que el elemento se mueva
   */
  double speed;

  /**
   * Nombre del archivo que contiene los fotogramas del elemento
   */
  std::string bitmap;

  /**
   * Anchura en píxeles de cada fotograma
   */
  int frameWidth;

  /**
   * Altura en píxeles de cada fotograma
   */
  int frameHeight;

  /**
   * Nombre del archivo que contiene las sombras del elemento
   */
  std::string shadow;

  /**
   * Anchura en píxeles de cada sombra
   */
  int shadowWidth;

  /**
   * Altura en píxeles de cada sombra
   */
  int shadowHeight;

  /**
   * Fotogramas extra del elemento, es decir, aquellos que no tiene relación con el movimiento como,
   * por ejemplo, los fotogramas de salto
   */
  int extraFrames;

  /**
   * Indica los fotogramas que forman la secuencia de animación
   */
  std::vector<int> frames;

  /**
   * Conjunto de fotogramas del elemento
   */
  std::vector<BITMAP*> motion;

  /**
   * Conjunto de sombras del elemento
   */
  std::vector<BITMAP*> shadows;
};

}

#endif //ITEMDATA_HPP_

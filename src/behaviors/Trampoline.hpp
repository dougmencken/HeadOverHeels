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

#ifndef TRAMPOLINE_HPP_
#define TRAMPOLINE_HPP_

#include "Behavior.hpp"
#include "HPC.hpp"

namespace isomot
{

// Declaraciones adelantadas
class Item;

/**
 * Mueve al elemento en la dirección marcada por el elemento que tiene encima
 */
class Trampoline : public Behavior
{
public:

  /**
   * Constructor
   * @param item Elemento que tiene este comportamiento
   * @param id Identificador del comportamiento
   */
  Trampoline(Item* item, const BehaviorId& id);

  virtual ~Trampoline();

  /**
   * Actualiza el comportamiento del elemento en cada ciclo
   * @return false si la actualización implica la destrucción del elemento o true en caso contrario
   */
  virtual bool update();

private:

  /**
   * Indica si el trampolín está plegado debido al peso de un elemento
   */
  bool folded;

  /**
   * Indica si el trampolín está rebotando
   */
  bool rebounding;

  /**
   * Fotograma del trampolín donde se representa en su estado normal.  Se espera que sea el fotograma 0
   */
  int regularFrame;

  /**
   * Fotograma del trampolín donde se representa plegado. Se espera que sea el fotograma 1
   */
  int foldedFrame;

  /**
   * Cronómetro que controla la velocidad de movimiento del elemento
   */
  HPC* speedTimer;

  /**
   * Cronómetro que controla la velocidad de caída del elemento
   */
  HPC* fallenTimer;

  /**
   * Cronometro que controla el tiempo que el trampolín está rebotando
   */
  HPC* reboundTimer;
};

}

#endif //TRAMPOLINE_HPP_

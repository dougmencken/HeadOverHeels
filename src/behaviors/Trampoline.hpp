// The free and open source remake of Head over Heels
//
// Copyright © 2016 Douglas Mencken dougmencken @ gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Trampoline_hpp_
#define Trampoline_hpp_

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

#endif

// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Patrol_hpp_
#define Patrol_hpp_

#include "Behavior.hpp"
#include "HPC.hpp"


namespace isomot
{

class Item;

/**
 * Mueve aleatoriamente al elemento dependiendo del tipo de comportamiento. En el
 * caso del patrullero-4c se mueve en las direcciones norte, sur, este u oeste; en
 * el caso del patrullero-4d lo hace en las direcciones noreste, noroeste, sudeste
 * y sudoeste; y, en el caso del patrullero-8 lo hace en todas las direcciones
 */

class Patrol : public Behavior
{

public:

       /**
        * Constructor
        * @param item Elemento que tiene este comportamiento
        * @param id Identificador de comportamiento: Patrol4cBehavior, Patrol4dBehavior
        * o Patrol8Behavior
        */
        Patrol( Item * item, const BehaviorId & id ) ;

        virtual ~Patrol( ) ;

       /**
        * Actualiza el comportamiento del elemento en cada ciclo
        * @return false si la actualización implica la destrucción del elemento o true en caso contrario
        */
        virtual bool update () ;

protected:

       /**
        * El elemento cambia aleatoriamente de dirección
        */
        void changeDirection () ;

private:

       /**
        * Cronómetro que controla la velocidad de movimiento del elemento
        */
        HPC * speedTimer ;

       /**
        * Cronómetro que controla la velocidad de caída del elemento
        */
        HPC * fallTimer ;

       /**
        * Cronómetro que controla el cambio de dirección del elemento
        */
        HPC * changeTimer ;

};

}

#endif

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

class Item ;

/**
 * Moves item randomly. For "four primary directions" variant move north, south, east or west.
 * For variant "four secondary directions" move northeast, northwest, southeast and southwest.
 * And for "eight directions" variant move in all these eight directions
 */

class Patrol : public Behavior
{

public:

        Patrol( Item * item, const std::string & behavior ) ;

        virtual ~Patrol( ) ;

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

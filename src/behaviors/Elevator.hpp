// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Elevator_hpp_
#define Elevator_hpp_

#include "Behavior.hpp"
#include "Timer.hpp"


namespace isomot
{

class Item ;

/**
 * Moves item above it up and down
 */

class Elevator : public Behavior
{

public:

        Elevator( Item * item, const std::string & behavior ) ;

        virtual ~Elevator( ) ;

        virtual bool update () ;

private:

       /**
        * Maximum height at which elevator ascends
        */
        int top ;

       /**
        * Minimum height at which elevator descends
        */
        int bottom ;

       /**
        * Is first movement of elevator ascending if true or descending if false
        */
        bool ascent ;

        ActivityOfItem lastActivity ;

       /**
        * Cronómetro que controla la velocidad de movimiento del elemento
        */
        Timer * speedTimer ;

       /**
        * Cronómetro que controla los tiempos de parada del ascensor. Suceden cuando cambia de dirección
        */
        Timer * stopTimer ;

public:

       /**
        * Asigna los datos de altura máxima y mínima, así como el movimiento inicial del ascensor
        */
        void setMoreData ( void * data ) ;

};

}

#endif

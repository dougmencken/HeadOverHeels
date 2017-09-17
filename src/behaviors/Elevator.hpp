// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Elevator_hpp_
#define Elevator_hpp_

#include "Behavior.hpp"
#include "HPC.hpp"


namespace isomot
{

class Item ;

/**
 * Mueve a los elementos situados encima hacia arriba o hacia abajo
 */

class Elevator : public Behavior
{

public:

        Elevator( Item * item, const std::string & behavior ) ;

        virtual ~Elevator( ) ;

        virtual bool update () ;

private:

       /**
        * Altura máxima a la que asciende el ascensor
        */
        int top ;

       /**
        * Altura mínima a la que desciende el ascensor
        */
        int bottom ;

       /**
        * Indica si el primer movimiento que realiza el ascensor es ascendente (true) o descendente (false)
        */
        bool ascent ;

        ActivityOfItem lastActivity ;

       /**
        * Cronómetro que controla la velocidad de movimiento del elemento
        */
        HPC * speedTimer ;

       /**
        * Cronómetro que controla los tiempos de parada del ascensor. Suceden cuando cambia de dirección
        */
        HPC * stopTimer ;

public:

       /**
        * Asigna los datos de altura máxima y mínima, así como el movimiento inicial del ascensor
        */
        void setMoreData ( void * data ) ;

};

}

#endif

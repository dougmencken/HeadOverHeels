// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef OneWay_hpp_
#define OneWay_hpp_

#include "Behavior.hpp"
#include "HPC.hpp"


namespace isomot
{

// Declaraciones adelantadas
class Item;

/**
 * Mueve al elemento en un único sentido. Cuando éste choca con algo gira 180º y prosigue la
 * marcha en sentido contrario
 */

class OneWay : public Behavior
{

public:

       /**
        * Constructor
        * @param item Elemento que tiene este comportamiento
        * @param id Identificador del comportamiento
        * @param isFlying Indica si el elemento vuela
        */
        OneWay( Item * item, const BehaviorId & id, bool isFlying ) ;

        virtual ~OneWay( ) ;

       /**
        * Actualiza el comportamiento del elemento en cada ciclo
        * @return false si la actualización implica la destrucción del elemento o true en caso contrario
        */
        virtual bool update () ;

protected:

       /**
        * Acciones a realizar cuando el elemento está en un estado ajeno a los estados propios de su
        * comportamiento
        */
        void start () ;

       /**
        * El elemento da media vuelta
        */
        void turnRound () ;

private:

       /**
        * Indica si el elemento vuela
        */
        bool isFlying ;

       /**
        * Cronómetro que controla la velocidad de movimiento del elemento
        */
        HPC * speedTimer ;

       /**
        * Cronómetro que controla la velocidad de caída del elemento
        */
        HPC * fallTimer ;

};

}

#endif

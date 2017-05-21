// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef ConveyorBelt_hpp_
#define ConveyorBelt_hpp_

#include "Behavior.hpp"
#include "HPC.hpp"


namespace isomot
{

class Item ;


class ConveyorBelt : public Behavior
{

public:

       /**
        * Constructor
        * @param item Elemento que tiene este comportamiento
        * @param id Identificador del comportamiento
        */
        ConveyorBelt( Item * item, const BehaviorOfItem & behavior ) ;

        virtual ~ConveyorBelt( ) ;

       /**
        * Actualiza el comportamiento del elemento en cada ciclo
        * @return false si la actualización implica la destrucción del elemento o true en caso contrario
        */
        virtual bool update () ;

private:

       /**
        * Indica si la cinta está en marcha o detenida
        */
        bool active ;

       /**
        * Cronómetro que controla la velocidad de arrastre de los elementos situados encima
        */
        HPC * speedTimer ;

};

}

#endif

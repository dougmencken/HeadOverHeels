// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef RemoteControl_hpp_
#define RemoteControl_hpp_

#include "Behavior.hpp"
#include "HPC.hpp"


namespace isomot
{

class Item;
class FreeItem;

/**
 * Representa a un par de elementos controlador/controlado. Cuando un elemento choca con un elemento
 * controlador, éste recoge el sentido de la colisión y mueve al elemento controlado en dicho sentido
 */

class RemoteControl : public Behavior
{

public:

       /**
        * Constructor
        * @param item Elemento que tiene este comportamiento
        * @param id Identificador del comportamiento
        */
        RemoteControl( Item * item, const BehaviorId & id ) ;

        virtual ~RemoteControl( ) ;

       /**
        * Actualiza el comportamiento del elemento en cada ciclo
        * @return false si la actualización implica la destrucción del elemento o true en caso contrario
        */
        virtual bool update () ;

private:

       /**
        * El elemento controlado por el mando a distancia
        */
        FreeItem * controlledItem ;

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

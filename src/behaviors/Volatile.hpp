// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Volatile_hpp_
#define Volatile_hpp_

#include "Behavior.hpp"
#include "HPC.hpp"
#include "Ism.hpp"


namespace isomot
{

class Item ;
class ItemData ;

/**
 * For items which lack autonomous movement and move only on touch by some other item
 */

class Volatile : public Behavior
{

public:

       /**
        * @param item Elemento que tiene este comportamiento
        * @param id Identificador de comportamiento
        */
        Volatile( Item * item, const BehaviorOfItem & id ) ;

        virtual ~Volatile( ) ;

       /**
        * Actualiza el comportamiento del elemento en cada ciclo
        * @return false si la actualización implica la destrucción del elemento o true en caso contrario
        */
        virtual bool update () ;

       /**
        * Asigna los datos del elemento usado como transición en la destrucción del volátil
        */
        void setMoreData ( void * data ) ;

protected:

       /**
        * Datos del elemento empleado como disparo
        */
        ItemData * bubblesData ;

private:

       /**
        * Indica si el elemento deja de ser volátil por haber activado un interruptor en la sala
        */
        bool solid ;

       /**
        * Cronómetro que controla el tiempo de existencia del elemento volátil
        */
        HPC * destroyTimer ;

};

}

#endif

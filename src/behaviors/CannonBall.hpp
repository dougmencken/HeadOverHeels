// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef CannonBall_hpp_
#define CannonBall_hpp_

#include "Behavior.hpp"
#include "HPC.hpp"


namespace isomot
{

class Item ;
class ItemData ;

/**
 * La bola de cañón de la pantalla final. Se mueve hacia el norte y cuando colisiona desaparece
 */

class CannonBall : public Behavior
{

public:

       /**
        * Constructor
        * @param item Elemento que tiene este comportamiento
        * @param behavior Identificador de comportamiento
        */
        CannonBall( Item * item, const BehaviorOfItem & behavior ) ;

        virtual ~CannonBall( );

       /**
        * Actualiza el comportamiento del elemento en cada ciclo
        * @return false si la actualización implica la destrucción del elemento o true en caso contrario
        */
        virtual bool update () ;

protected:

       /**
        * Datos del elemento empleado como disparo
        */
        ItemData * bubblesData ;

private:

       /**
        * Cronómetro que controla la velocidad de movimiento del elemento
        */
        HPC * speedTimer ;

public:

       /**
        * Asigna los datos del elemento usado para representar la explosión de la bola
        */
        void setMoreData ( void * data ) ;

};

}

#endif

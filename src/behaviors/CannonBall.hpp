// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef CannonBall_hpp_
#define CannonBall_hpp_

#include "Behavior.hpp"
#include "Timer.hpp"


namespace isomot
{

class Item ;
class ItemData ;

/**
 * The cannonball of the final screen. It moves northwards and disappears when it collides
 */

class CannonBall : public Behavior
{

public:

        CannonBall( Item * item, const std::string & behavior ) ;

        virtual ~CannonBall( );

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
        Timer * speedTimer ;

public:

       /**
        * Asigna los datos del elemento usado para representar la explosión de la bola
        */
        void setMoreData ( void * data ) ;

};

}

#endif

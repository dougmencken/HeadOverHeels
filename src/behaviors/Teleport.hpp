// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Teleport_hpp_
#define Teleport_hpp_

#include "Behavior.hpp"
#include "HPC.hpp"
#include "Ism.hpp"


namespace isomot
{

class Item ;

/**
 * Elemento que se activa al detectar un elemento encima. Permite el teletransporte entre salas
 */

class Teleport : public Behavior
{

public:

        Teleport( Item * item, const std::string & behavior ) ;

        virtual ~Teleport( ) ;

        virtual bool update () ;

private:

       /**
        * Indica si el telepuerto está activo
        */
        bool activated ;

};

}

#endif

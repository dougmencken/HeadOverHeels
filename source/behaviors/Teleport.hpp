// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Teleport_hpp_
#define Teleport_hpp_

#include "Behavior.hpp"

class GridItem ;


namespace behaviors
{

/**
 * Teleports between rooms, activates when some item is above it
 */

class Teleport : public Behavior
{

public:

        Teleport( GridItem & item, const std::string & behavior ) ;

        virtual ~Teleport( ) {}

        virtual bool update () ;

private:

        bool activated ;

};

}

#endif

// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Sink_hpp_
#define Sink_hpp_

#include "Behavior.hpp"
#include "Timer.hpp"

class GridItem ;


namespace behaviors
{

/**
 * Sink when another item is on top of this one
 */

class Sink : public Behavior
{

public:

        Sink( GridItem & item, const std::string & behavior ) ;

        virtual ~Sink( ) {}

        virtual bool update () ;

private:

        autouniqueptr < Timer > fallTimer ;

};

}

#endif

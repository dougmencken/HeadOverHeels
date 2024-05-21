// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Doughnut_hpp_
#define Doughnut_hpp_

#include "Behavior.hpp"
#include "Timer.hpp"

class FreeItem ;


namespace behaviors
{

class Doughnut : public Behavior
{

public:

        Doughnut( FreeItem & item, const std::string & behavior ) ;

        virtual ~Doughnut( ) {}

        virtual bool update () ;

private:

        // timer for the speed of motion
        autouniqueptr < Timer > speedTimer ;

} ;

}

#endif

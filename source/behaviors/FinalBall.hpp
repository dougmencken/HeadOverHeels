// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef FinalBall_hpp_
#define FinalBall_hpp_

#include "Behavior.hpp"
#include "Timer.hpp"

class FreeItem ;


namespace behaviors
{

/**
 * The ball for the final scene. It moves northwards and disappears when collides
 */

class FinalBall : public Behavior
{

public:

        FinalBall( FreeItem & item, const std::string & behavior ) ;

        virtual ~FinalBall( ) { }

        virtual bool update () ;

private:

        autouniqueptr < Timer > speedTimer ;

};

}

#endif

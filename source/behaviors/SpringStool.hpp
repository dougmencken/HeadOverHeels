// The free and open source remake of Head over Heels
//
// Copyright © 2025 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef SpringStool_hpp_
#define SpringStool_hpp_

#include "Behavior.hpp"
#include "Timer.hpp"

class FreeItem ;


namespace behaviors
{

class SpringStool : public Behavior
{

public:

        SpringStool( FreeItem & item, const std::string & behavior ) ;

        virtual ~SpringStool( ) {}

        virtual bool update () ;

private:

        // is the spring folded due to the weight of something above
        bool folded ;

        // does the spring bounce
        bool rebounding ;

        // frame for an undeformed spring stool, usually 0th
        int unstrainedFrame ;

        // frame for a folded spring stool, usually 1st
        int foldedFrame ;

        // timer for the motion speed
        autouniqueptr < Timer > speedTimer ;

        // timer for the speed of falling
        autouniqueptr < Timer > fallTimer ;

        // chronometer for bouncing
        autouniqueptr < Timer > reboundTimer ;

};

}

#endif

// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Trampoline_hpp_
#define Trampoline_hpp_

#include "Behavior.hpp"
#include "Timer.hpp"


namespace behaviors
{

class Trampoline : public Behavior
{

public:

        Trampoline( const ItemPtr & item, const std::string & behavior ) ;

        virtual ~Trampoline( ) ;

        virtual bool update () ;

private:

       /**
        * Is trampoline folded due to weight of item on top of it
        */
        bool folded ;

       /**
        * Does trampoline bounce
        */
        bool rebounding ;

       /**
        * Frame of undeformed trampoline, usually 0th frame
        */
        int plainFrame ;

       /**
        * Frame of folded trampoline, usually 1st frame
        */
        int foldedFrame ;

       /**
        * Timer for speed of movement
        */
        autouniqueptr < Timer > speedTimer ;

       /**
        * Timer for speed of falling
        */
        autouniqueptr < Timer > fallTimer ;

       /**
        * Chronometer for bouncing
        */
        autouniqueptr < Timer > reboundTimer ;

};

}

#endif

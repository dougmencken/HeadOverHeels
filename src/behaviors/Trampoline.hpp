// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Trampoline_hpp_
#define Trampoline_hpp_

#include "Behavior.hpp"
#include "Timer.hpp"


namespace isomot
{

class Item ;


class Trampoline : public Behavior
{

public:

        Trampoline( Item * item, const std::string & behavior ) ;

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
        Timer* speedTimer ;

       /**
        * Timer for speed of falling
        */
        Timer* fallTimer ;

       /**
        * Chronometer for time of bouncing
        */
        Timer* reboundTimer ;

};

}

#endif

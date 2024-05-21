// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef ThereAndBack_hpp_
#define ThereAndBack_hpp_

#include "Behavior.hpp"
#include "Timer.hpp"

class FreeItem ;


namespace behaviors
{

/**
 * Move an item in one direction. When it hits something, rotate 180°
 * and continue to move in the opposite way
 */

class ThereAndBack : public Behavior
{

public:

        ThereAndBack( FreeItem & item, const std::string & behavior, bool flying ) ;

        virtual ~ThereAndBack( ) {}

        virtual bool update () ;

protected:

        void moveIt () ;

        void turnBack () ;

private:

        bool isFlying ;

        autouniqueptr < Timer > speedTimer ;

        autouniqueptr < Timer > fallTimer ;

};

}

#endif

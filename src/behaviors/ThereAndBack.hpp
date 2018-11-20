// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef ThereAndBack_hpp_
#define ThereAndBack_hpp_

#include "Behavior.hpp"
#include "Timer.hpp"


namespace iso
{

/**
 * Moves item in one direction. When it collides with something,
 * it rotates 180° and continues to move in opposite way
 */

class ThereAndBack : public Behavior
{

public:

        ThereAndBack( const ItemPtr & item, const std::string & behavior, bool flying ) ;

        virtual ~ThereAndBack( ) ;

        virtual bool update () ;

protected:

        void letsMove () ;

        void turnRound () ;

private:

        bool isFlying ;

        autouniqueptr < Timer > speedTimer ;

        autouniqueptr < Timer > fallTimer ;

};

}

#endif

// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef FinalBall_hpp_
#define FinalBall_hpp_

#include "Behavior.hpp"
#include "Timer.hpp"


namespace iso
{

class Item ;
class ItemData ;

/**
 * The ball of the final screen. It moves northwards and disappears when it collides
 */

class FinalBall : public Behavior
{

public:

        FinalBall( const ItemPtr & item, const std::string & behavior ) ;

        virtual ~FinalBall( );

        virtual bool update () ;

private:

       /**
        * Timer for speed of movement
        */
        autouniqueptr < Timer > speedTimer ;

};

}

#endif

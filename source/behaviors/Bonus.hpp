// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Bonus_hpp_
#define Bonus_hpp_

#include "Behavior.hpp"
#include "Timer.hpp"

class FreeItem ;
class AvatarItem ;


namespace behaviors
{

/**
 * The behavior of a bonus bunny, horn, pack of donuts, handbag, crown or reincarnation fish
 */

class Bonus : public Behavior
{

public:

        Bonus( FreeItem & item, const std::string & behavior ) ;

        virtual ~Bonus () { }

        virtual bool update () ;

protected:

        /**
         * whether can take this item or not
         */
        bool mayTake ( const Item & taker ) ;

        void takeIt ( AvatarItem & whoTakes ) ;

private:

        // timer for the disappearance of bonus
        autouniqueptr < Timer > disappearanceTimer ;

        // timer for the speed of motion
        autouniqueptr < Timer > speedTimer ;

        // timer for the speed of falling
        autouniqueptr < Timer > fallTimer ;

};

}

#endif

// The free and open source remake of Head over Heels
//
// Copyright © 2025 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Volatile_hpp_
#define Volatile_hpp_

#include "Behavior.hpp"
#include "Timer.hpp"


namespace behaviors
{

/**
 * The behavior of item that disappears for some reason, like
 * over time or when another item touches it
 */

class Volatile : public Behavior
{

public :

        Volatile( DescribedItem & item, const std::string & behavior ) ;

        virtual ~Volatile( ) {}

        virtual bool update () ;

        // string constants found in room description files

        static const std::string on_contact ; // "vanishing on contact"
        static const std::string when_above ; // "vanishing when some free dude is above"
        static const std::string when_above_slower ; // "slowly vanishing when some free dude is above"
        static const std::string as_Head_appears ; // "vanishing as soon as Head appears"
        static const std::string after_a_while ; // "vanishing after a while"

private :

        /**
         * is true when this item isn’t currently volatile, such as when a switch is toggled
         */
        bool solid ;

        autouniqueptr < Timer > disappearanceTimer ;

};

}

#endif

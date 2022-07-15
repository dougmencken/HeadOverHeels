// The free and open source remake of Head over Heels
//
// Copyright © 2022 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Impel_hpp_
#define Impel_hpp_

#include "Behavior.hpp"
#include "Timer.hpp"


namespace iso
{

/**
 * Move item in one direction as result of displacement of another item. Stop when something is hit
 */

class Impel : public Behavior
{

public:

        Impel( const ItemPtr & item, const std::string & behavior ) ;

        virtual ~Impel( ) ;

        virtual bool update () ;

private:

       /**
        * Timer for speed of movement
        */
        autouniqueptr < Timer > speedTimer ;

       /**
        * Timer for speed of falling
        */
        autouniqueptr < Timer > fallTimer ;

};

}

#endif

// The free and open source remake of Head over Heels
//
// Copyright © 2022 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Sink_hpp_
#define Sink_hpp_

#include "Behavior.hpp"
#include "Timer.hpp"


namespace iso
{

/**
 * Sink when another item is on top of this one
 */

class Sink : public Behavior
{

public:

        Sink( const ItemPtr & item, const std::string & behavior ) ;

        virtual ~Sink( ) ;

        virtual bool update () ;

private:

       /**
        * Timer for speed of falling
        */
        autouniqueptr < Timer > fallTimer ;

};

}

#endif

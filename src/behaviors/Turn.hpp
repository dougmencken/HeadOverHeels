// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Turn_hpp_
#define Turn_hpp_

#include "Behavior.hpp"
#include "Timer.hpp"


namespace iso
{

/**
 * Moves item in one direction. When it collides with something,
 * turn 90° left or right and continue to move
 */

class Turn : public Behavior
{

public:

        Turn( const ItemPtr & item, const std::string & behavior ) ;

        virtual ~Turn( ) ;

        virtual bool update () ;

protected:

        void begin () ;

       /**
        * When item turns around
        */
        void turn () ;

private:

        autouniqueptr < Timer > speedTimer ;

        autouniqueptr < Timer > fallTimer ;

};

}

#endif

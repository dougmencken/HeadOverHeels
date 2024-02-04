// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Detector_hpp_
#define Detector_hpp_

#include "Behavior.hpp"
#include "Timer.hpp"


namespace behaviors
{

/**
 * Detect the active character and move to hunt it
 */

class Detector : public Behavior
{

public:

        Detector( const ItemPtr & item, const std::string & behavior ) ;

        virtual ~Detector( ) ;

        virtual bool update () ;

private:

       /**
        * Timer for the movement speed
        */
        autouniqueptr < Timer > speedTimer ;

       /**
        * Timer for the speed of falling
        */
        autouniqueptr < Timer > fallTimer ;

};

}

#endif

// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Detector_hpp_
#define Detector_hpp_

#include "Behavior.hpp"
#include "Timer.hpp"


namespace isomot
{

class Item ;

/**
 * Detect player and move to hunt it
 */

class Detector : public Behavior
{

public:

        Detector( Item * item, const std::string & behavior ) ;

        virtual ~Detector( ) ;

        virtual bool update () ;

private:

       /**
        * Timer for speed of movement
        */
        Timer * speedTimer ;

       /**
        * Timer for speed of falling
        */
        Timer * fallTimer ;

};

}

#endif

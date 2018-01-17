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
 * Detecta a un jugador y avanza en la dirección adecuada para darle caza
 */

class Detector : public Behavior
{

public:

        Detector( Item * item, const std::string & behavior ) ;

        virtual ~Detector( ) ;

        virtual bool update () ;

private:

       /**
        * Cronómetro que controla la velocidad de movimiento del elemento
        */
        Timer * speedTimer ;

       /**
        * Cronómetro que controla la velocidad de caída del elemento
        */
        Timer * fallTimer ;

};

}

#endif

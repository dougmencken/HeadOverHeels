// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
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


namespace isomot
{

class Item ;

/**
 * Move item in one direction as result of displacement of another item. Stop when something is hit
 */

class Impel : public Behavior
{

public:

        Impel( Item * item, const std::string & behavior ) ;

        virtual ~Impel( ) ;

        virtual bool update () ;

private:

       /**
        * Timer that defines speed of movement
        */
        Timer * speedTimer ;

       /**
        * Timer that defines speed of fall
        */
        Timer * fallTimer ;

};

}

#endif

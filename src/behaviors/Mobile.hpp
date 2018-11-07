// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Mobile_hpp_
#define Mobile_hpp_

#include "Behavior.hpp"
#include "Timer.hpp"


namespace iso
{

class Item ;

/**
 * Item moves only when pushed by other item, and lacks autonomous movement
 */

class Mobile : public Behavior
{

public:

        Mobile( const ItemPtr & item, const std::string & behavior ) ;

        virtual ~Mobile( ) ;

        virtual bool update () ;

private:

        autouniqueptr < Timer > speedTimer ;

        autouniqueptr < Timer > fallTimer ;

};

}

#endif

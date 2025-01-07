// The free and open source remake of Head over Heels
//
// Copyright © 2025 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef CreatorOfBehaviors_hpp_
#define CreatorOfBehaviors_hpp_

#include <string>

#include "Behavior.hpp"
#include "AbstractItem.hpp"


namespace behaviors
{

class CreatorOfBehaviors
{

private:

        CreatorOfBehaviors () {}

public:

        static autouniqueptr< Behavior > createBehaviorByName ( AbstractItem & item, const std::string & behavior ) ;

} ;

}

#endif

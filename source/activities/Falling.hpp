// The free and open source remake of Head over Heels
//
// Copyright © 2025 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Falling_hpp_
#define Falling_hpp_

#include <string>
#include <vector>

#include "Behavior.hpp"
#include "Mediator.hpp"


namespace activities
{

class Falling
{

public:

        virtual ~Falling( ) {}

        /**
         * Item is falling
         * @return true if the item falls, or false when there’s a collision
         */
        bool fall ( behaviors::Behavior & behavior ) ;

        static Falling & getInstance () ;

private:

        /**
         * When the item falls on some other one, that one becomes the item’s anchor
         */
        void assignAnchor ( const std::string & uniqueNameOfItem, Mediator * mediator, const std::vector < std::string > & itemsBelow ) ;

private:

        Falling( ) {}

        static Falling * instance ;

};

}

#endif

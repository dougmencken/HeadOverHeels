// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Displacing_hpp_
#define Displacing_hpp_

#include "ActivityOfItem.hpp"
#include "Behavior.hpp"


namespace activities
{

class Displacing
{

public:

        virtual ~Displacing( ) {}

        static Displacing & getInstance() ;

        /**
         * Item is being displaced
         * @param behavior the behavior of item
         * @param activity subactivity to get way of movement
         * @param canFall whether an item falls, false if it flies
         * @return true for displacing or changing activity, false for a collision
         */
        bool displace ( behaviors::Behavior * behavior, ActivityOfItem * activity, bool canFall ) ;

private:

        Displacing( ) {}

        static Displacing * instance ;

};

}

#endif

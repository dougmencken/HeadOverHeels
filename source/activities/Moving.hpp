// The free and open source remake of Head over Heels
//
// Copyright © 2025 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Moving_hpp_
#define Moving_hpp_

#include "Behavior.hpp"

class FreeItem ;


namespace activities
{

class Moving
{

public:

        virtual ~Moving( ) {}

        static Moving & getInstance () ;

        /**
         * Item is moving
         * @param behavior the behavior of item
         * @param itFalls whether the item falls, false for a flying one
         * @return true for a move or changed activity, false for a collision
         */
        bool move ( behaviors::Behavior & behavior, bool itFalls ) ;

protected:

        /**
         * Lift the item and items stacked above it, recursively
         * @param freeItem the item
         * @param z units to ascend
         */
        void ascend ( FreeItem & freeItem, int dz ) ;

        /**
         * Lower the item and items below it, recursively
         * @param freeItem the item
         * @param z units to descend
         */
        void descend ( FreeItem & freeItem, int dz ) ;

private:

        Moving( ) {}

        static Moving * instance ;

};

}

#endif

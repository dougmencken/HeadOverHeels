// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Moving_hpp_
#define Moving_hpp_

#include "KindOfActivity.hpp"
#include "FreeItem.hpp"


namespace activities
{

class Moving : public KindOfActivity
{

protected:

        Moving( ) : KindOfActivity( ) {}

public:

        virtual ~Moving( ) {}

        static Moving & getInstance () ;

       /**
        * Move the item
        * @param behavior the behavior of item
        * @param activity the current activity
        * @param itFalls whether the item falls, false for a flying one
        * @return true for a move or activity change, false for a collision
        */
        virtual bool move ( behaviors::Behavior * behavior, ActivityOfItem * activity, bool itFalls ) ;

protected:

        /**
         * Recursively lift the items stacked on that item
         * @param freeItem That item
         * @param z Units to ascend
         */
        void ascent ( FreeItem & freeItem, int z ) ;

        /**
         * Recursively lower the items stacked on that item
         * @param freeItem That item
         * @param z Units to descend
         */
        void descend ( FreeItem & freeItem, int z ) ;

private:

        static Moving * instance ;

};

}

#endif

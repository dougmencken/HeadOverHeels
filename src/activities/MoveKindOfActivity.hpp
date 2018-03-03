// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef MoveKindOfActivity_hpp_
#define MoveKindOfActivity_hpp_

#include "Ism.hpp"
#include "KindOfActivity.hpp"


namespace isomot
{

class Behavior ;
class FreeItem ;


class MoveKindOfActivity : public KindOfActivity
{

protected:

        MoveKindOfActivity( ) ;

public:

        virtual ~MoveKindOfActivity( ) ;

        static KindOfActivity * getInstance () ;

        /**
         * Move item
         * @param behavior Behavior of item
         * @param activity Way of movement
         * @param canFall true if item falls, false if it flies
         * @return when item displaced or changed activity, false when there’s collision
         */
        virtual bool move ( Behavior * behavior, ActivityOfItem * activity, bool canFall ) ;

protected:

        /**
         * Recursively lift items stacked on this item
         * @param freeItem Currently first item
         * @param z Units to ascend
         */
        void ascent ( FreeItem * freeItem, int z ) ;

        /**
         * Recursively lower items stacked on this item
         * @param freeItem Currently first item
         * @param z Units to descend
         */
        void descend ( FreeItem * freeItem, int z ) ;

private:

        static KindOfActivity * instance ;

};

}

#endif

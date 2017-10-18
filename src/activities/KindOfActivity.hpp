// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef KindOfActivity_hpp_
#define KindOfActivity_hpp_

#include <vector>

#include "Ism.hpp"


namespace isomot
{

class Behavior ;
class Item ;

/**
 * Abstraction of item’s current activity
 */

class KindOfActivity
{

public:

        KindOfActivity( ) ;

        virtual ~KindOfActivity( ) ;

       /**
        * Item moves
        * @param activity specifies direction of movement
        * @param canFall whether item may fall
        * @return true for move or change of activity, false for collision
        */
        virtual bool move ( Behavior * behavior, ActivityOfItem * activity, bool canFall )
        {
                return true ;
        }

       /**
        * Item is being displaced
        * @param activity subactivity which specifies direction of movement
        * @param canFall whether item may fall
        * @return true for displace or change of activity, false for collision
        */
        virtual bool displace ( Behavior * behavior, ActivityOfItem * activity, bool canFall )
        {
                return true ;
        }

       /**
        * Item falls
        * @return true if fall or false when there’s collision
        */
        virtual bool fall ( Behavior * behavior )
        {
                return true ;
        }

       /**
        * Item jumps
        * @param jumpVector vector of pair of values ( offset on X or Y and offset on Z )
        *                   for each cycle to define item’s jump
        * @param jumpPhase phase of jump
        * @return true if jump or false when there’s collision
        */
        virtual bool jump ( Behavior * behavior, ActivityOfItem * activity, const std::vector< JumpMotion >& jumpVector, int jumpPhase )
        {
                return true ;
        }

protected:

       /**
        * Change activity of items that collide with the sender
        */
        virtual void propagateActivityToAdjacentItems ( Item * sender, const ActivityOfItem & activity ) ;

       /**
        * Change activity of items above the sender
        */
        virtual void propagateActivityToItemsAbove ( Item * sender, const ActivityOfItem & activity ) ;

};

}

#endif

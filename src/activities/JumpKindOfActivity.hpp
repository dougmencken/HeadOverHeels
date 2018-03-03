// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef JumpKindOfActivity_hpp_
#define JumpKindOfActivity_hpp_

#include <vector>
#include "KindOfActivity.hpp"
#include "Ism.hpp"


namespace isomot
{

class Behavior;
class FreeItem;

/**
 * Salto de un elemento
 */

class JumpKindOfActivity : public KindOfActivity
{

protected:

        JumpKindOfActivity();

public:

        virtual ~JumpKindOfActivity();

        static KindOfActivity * getInstance() ;

       /**
        * Item jumps
        * @param jumpVector vector of pair of values ( offset on X or Y and offset on Z )
        *                   for each cycle to define item’s jump
        * @param jumpPhase phase of jump
        * @return true if jump or false when there’s collision
        */
        virtual bool jump ( Behavior * behavior, ActivityOfItem * activity, const std::vector < JumpMotion >& jumpVector, int jumpPhase ) ;

protected:

       /**
        * Raise items stacked on player
        * @param sender player that triggers lifting of pile
        * @param freeItem first item of pile
        * @param z units to ascend pile of items
        */
        void lift ( FreeItem * sender, FreeItem * freeItem, int z ) ;

private:

        static KindOfActivity * instance ;

};

}

#endif

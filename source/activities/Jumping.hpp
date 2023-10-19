// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Jumping_hpp_
#define Jumping_hpp_

#include <vector>

#include "KindOfActivity.hpp"
#include "FreeItem.hpp"


namespace activities
{

/**
 * Salto
 */

class Jumping : public KindOfActivity
{

protected:

        Jumping() : KindOfActivity( ) {}

public:

        virtual ~Jumping() {}

        static Jumping & getInstance() ;

       /**
        * Item jumps
        * @param jumpVector vector of pair of values ( offset on X or Y, offset on Z )
        *                   for each cycle to define item’s jump
        * @param jumpPhase phase of jump
        * @return true if jump or false when there’s collision
        */
        virtual bool jump ( behaviors::Behavior * behavior, ActivityOfItem * activity, unsigned int jumpPhase, const std::vector < std::pair< int /* xy */, int /* z */ > >& jumpVector ) ;

protected:

       /**
        * Raise the items stacked on a character
        * @param sender the character that begins lifting the heap
        * @param item the first item of the heap
        * @param z how many units to rise the heap
        */
        void lift ( FreeItem & sender, Item & item, int z ) ;

private:

        static Jumping * instance ;

};

}

#endif

// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Special_hpp_
#define Special_hpp_

#include "Behavior.hpp"
#include "HPC.hpp"
#include "Ism.hpp"


namespace isomot
{

class Item ;
class ItemData ;
class PlayerItem ;

/**
 * Behavior of magic bonus item
 */

class Special : public Behavior
{

public:

        Special( Item* item, const std::string& behavior ) ;

        virtual ~Special () ;

        virtual bool update () ;

protected:

        /**
         * Checks if magic item can be taken, it depends on item and on character who takes it
         */
        bool mayTake ( Item* sender ) ;

        void takeMagicItem ( PlayerItem* sender ) ;

protected:

        ItemData * bubblesData ;

private:

        /**
         * Timer for disappearance of special item
         */
        HPC * disappearanceTimer ;

        /**
         * Timer for speed of item’s movement
         */
        HPC * speedTimer ;

        /**
         * Timer for speed of item’s drop
         */
        HPC * fallTimer ;

public:

        void setMoreData ( void * data ) ;
};

}

#endif

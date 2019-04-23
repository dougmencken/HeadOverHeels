// The free and open source remake of Head over Heels
//
// Copyright © 2019 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Masker_hpp_
#define Masker_hpp_

#include "Item.hpp"
#include "FreeItem.hpp"

namespace iso
{

/**
 * Masks image of free item behind image of another item
 */

class Masker
{

public:

        static void maskFreeItemBehindItem ( FreeItem & itemToMask, const Item & upwardItem ) ;

};

}

#endif

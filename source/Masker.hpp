// The free and open source remake of Head over Heels
//
// Copyright © 2026 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Masker_hpp_
#define Masker_hpp_

class DescribedItem ;
class FreeItem ;


/**
 * Masks image of free item behind image of another item
 */

class Masker
{

public:

        static void maskFreeItemBehindItem ( FreeItem & itemToMask, const DescribedItem & upwardItem ) ;

} ;

#endif

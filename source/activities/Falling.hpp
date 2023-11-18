// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Falling_hpp_
#define Falling_hpp_

#include <string>
#include <vector>

#include "KindOfActivity.hpp"
#include "FreeItem.hpp"


namespace activities
{

/**
 * Item falls
 */

class Falling : public KindOfActivity
{

protected:

        Falling( ) : KindOfActivity( ) {}

public:

        virtual ~Falling( ) {}

       /**
        * @return true if the item may fall or false when there’s a collision
        */
        virtual bool fall ( behaviors::Behavior * behavior ) ;

        static Falling & getInstance () ;

private:

       /**
        * When item falls on some other one, that one becomes item’s anchor
        */
        void assignAnchor ( const std::string & uniqueNameOfItem, Mediator * mediator, const std::vector < std::string > & itemsBelow ) ;

private:

        static Falling * instance ;

};

}

#endif

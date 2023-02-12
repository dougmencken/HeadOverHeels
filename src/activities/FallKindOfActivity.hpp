// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef FallKindOfActivity_hpp_
#define FallKindOfActivity_hpp_

#include <string>
#include <vector>

#include "KindOfActivity.hpp"
#include "FreeItem.hpp"


namespace iso
{

class Behavior ;
class Mediator ;


/**
 * Item falls
 */

class FallKindOfActivity : public KindOfActivity
{

protected:

        FallKindOfActivity( ) ;

public:

        virtual ~FallKindOfActivity( ) ;

       /**
        * @return true if item may fall or false when there’s collision
        */
        virtual bool fall ( Behavior * behavior ) ;

        static FallKindOfActivity & getInstance () ;

private:

       /**
        * When item falls on some other one, that one becomes item’s anchor
        */
        void assignAnchor ( const std::string & uniqueNameOfItem, Mediator * mediator, const std::vector < std::string > & itemsBelow ) ;

private:

        static FallKindOfActivity * instance ;

};

}

#endif

// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef FallKindOfActivity_hpp_
#define FallKindOfActivity_hpp_

#include <stack>
#include "Ism.hpp"
#include "KindOfActivity.hpp"


namespace isomot
{

class Behavior ;
class FreeItem ;


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
        * @return true if item falls or false if there’s collision
        */
        virtual bool fall ( Behavior * behavior ) ;

        static KindOfActivity * getInstance () ;

private:

        void assignAnchor ( FreeItem * freeItem, std::stack < int > items ) ;

private:

       /**
        * Único objeto de esta clase para toda la aplicación
        */
        static KindOfActivity * instance ;

};

}

#endif

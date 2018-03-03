// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef DisplaceKindOfActivity_hpp_
#define DisplaceKindOfActivity_hpp_

#include "Ism.hpp"
#include "KindOfActivity.hpp"


namespace isomot
{

class Behavior ;


class DisplaceKindOfActivity : public KindOfActivity
{

protected:

        DisplaceKindOfActivity( ) ;

public:

        virtual ~DisplaceKindOfActivity( ) ;

        static KindOfActivity * getInstance() ;

       /**
        * Displace item
        * @param behavior Behavior of item
        * @param canFall true if item falls, false if it flies
        * @return true when item displaced or changed activity, false when there’s collision
        */
        virtual bool displace ( Behavior * behavior, ActivityOfItem * activity, bool canFall ) ;

private:

        static KindOfActivity * instance ;

};

}

#endif

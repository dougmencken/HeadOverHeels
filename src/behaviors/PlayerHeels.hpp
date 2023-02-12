// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef PlayerHeels_hpp_
#define PlayerHeels_hpp_

#include "UserControlled.hpp"


namespace iso
{

class PlayerHeels : public UserControlled
{

public:

        PlayerHeels( const ItemPtr & item, const std::string & behavior ) ;

        virtual ~PlayerHeels( ) ;

        virtual bool update () ;

        virtual void behave () ;

};

}

#endif

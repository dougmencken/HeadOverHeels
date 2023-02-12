// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef PlayerHeadAndHeels_hpp_
#define PlayerHeadAndHeels_hpp_

#include "UserControlled.hpp"


namespace iso
{

class PlayerHeadAndHeels : public UserControlled
{

public:

        PlayerHeadAndHeels( const ItemPtr & item, const std::string & behavior ) ;

        virtual ~PlayerHeadAndHeels( ) ;

        virtual bool update () ;

        virtual void behave () ;

protected:

        virtual void wait( PlayerItem & playerItem ) ;

        virtual void blink( PlayerItem & playerItem ) ;

protected:

        std::map < std::string, unsigned int > blinkFrames ;

};

}

#endif

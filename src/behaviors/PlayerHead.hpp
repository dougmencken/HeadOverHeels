// The free and open source remake of Head over Heels
//
// Copyright © 2022 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef PlayerHead_hpp_
#define PlayerHead_hpp_

#include "UserControlled.hpp"

namespace iso
{

class PlayerHead : public UserControlled
{

public:

        PlayerHead( const ItemPtr & item, const std::string& behavior ) ;

        virtual ~PlayerHead( ) ;

        virtual bool update () ;

        virtual void behave () ;

protected:

        virtual void wait ( PlayerItem & playerItem ) ;

        virtual void blink ( PlayerItem & playerItem ) ;

protected:

        std::map < std::string, unsigned int > blinkFrames ;

};

}

#endif

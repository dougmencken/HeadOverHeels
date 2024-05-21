// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef CharacterHeadAndHeels_hpp_
#define CharacterHeadAndHeels_hpp_

#include "PlayerControlled.hpp"


namespace behaviors
{

class CharacterHeadAndHeels : public PlayerControlled
{

public:

        CharacterHeadAndHeels( AvatarItem & item ) ;

        virtual ~CharacterHeadAndHeels( ) {}

        virtual bool update () ;

        virtual void behave () ;

protected:

        virtual void wait () ;

        virtual void blink () ;

protected:

        std::map < std::string, unsigned int > blinkFrames ;

};

}

#endif

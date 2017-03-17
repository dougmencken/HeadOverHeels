// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef PlaySound_hpp_
#define PlaySound_hpp_

#include "SoundManager.hpp"


enum isomot::StateId ;

namespace gui
{

class PlaySound : public Action
{

public:

        PlaySound( isomot::StateId what ) : Action( ) {  whichSound = what ;  }

        virtual ~PlaySound( ) { }

        void doIt ()
        {
                isomot::SoundManager::getInstance()->stopEverySound (); //stop ( 0, whichSound );
                isomot::SoundManager::getInstance()->play ( 0, whichSound, /* loop */ false );
        }

        const char * getNameOfAction ()  {  return "PlaySound" ;  }

private:

        isomot::StateId whichSound ;

};

}

#endif

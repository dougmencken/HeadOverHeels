// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef PlaySound_hpp_
#define PlaySound_hpp_

#include "SoundManager.hpp"


namespace gui
{

class PlaySound : public Action
{

public:

        PlaySound( const iso::ActivityOfItem& what ) : Action( nilPointer ), whichSound( what ) { }

        virtual ~PlaySound( ) { }

        std::string getNameOfAction () const {  return "PlaySound" ;  }

protected:

        virtual void doAction ()
        {
                iso::SoundManager::getInstance().stopEverySound (); //stop ( "gui", whichSound );
                iso::SoundManager::getInstance().play ( "gui", whichSound, /* loop */ false );
        }

private:

        iso::ActivityOfItem whichSound ;

};

}

#endif

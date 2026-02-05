// The free and open source remake of Head over Heels
//
// Copyright © 2026 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
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

        explicit PlaySound( const std::string & what ) : Action( ), soundOf( what ) { }

        virtual ~PlaySound( ) { }

protected:

        virtual void act ()
        {
                SoundManager::getInstance().stopEverySound (); //stop ( "menus", soundOf );
                SoundManager::getInstance().play ( "menus", soundOf, /* loop */ false );
        }

private:

        std::string soundOf ;

};

}

#endif

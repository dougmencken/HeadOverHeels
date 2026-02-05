// The free and open source remake of Head over Heels
//
// Copyright © 2026 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef CreateAudioMenu_hpp_
#define CreateAudioMenu_hpp_

#include "ActionWithHandlingKeys.hpp"

#include "MenuWithValues.hpp"


namespace gui
{

class Label ;

/**
 * Create menu for adjusting music and sound effects
 */

class CreateAudioMenu : public ActionWithHandlingKeys
{

public:

        CreateAudioMenu( )
                : ActionWithHandlingKeys( )
                , audioOptions( )
                , labelEffects( nilPointer )
                , labelMusic( nilPointer )
                , playRoomTunes( nilPointer )
        {
                audioOptions.setVerticalOffset( 33 );
        }

        virtual ~CreateAudioMenu( ) {}

        virtual void handleKey ( const std::string & key ) ;

protected:

        virtual void act () ;

private:

        void updateOptions () ;

        MenuWithValues audioOptions ;

        Label * labelEffects ;
        Label * labelMusic ;
        Label * playRoomTunes ;

};

}

#endif

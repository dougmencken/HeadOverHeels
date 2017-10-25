// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef CreateAudioMenu_hpp_
#define CreateAudioMenu_hpp_

#include "Action.hpp"

#include <allegro.h>


namespace gui
{

class MenuWithValues ;
class Label ;

/**
 * Create menu for adjusting music and sound effects of game
 */

class CreateAudioMenu : public Action
{

public:

        /**
         * Constructor
         * @param picture Image where to draw
         */
        CreateAudioMenu( BITMAP* picture ) ;

        std::string getNameOfAction () const {  return "CreateAudioMenu" ;  }

protected:

        /**
         * Show the audio menu
         */
        virtual void doAction () ;

private:

        BITMAP * where ;

        MenuWithValues * listOfOptions ;

        Label * labelEffects ;
        Label * labelMusic ;

        Label * playRoomTunes ;

};

}

#endif

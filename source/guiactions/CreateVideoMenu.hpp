// The free and open source remake of Head over Heels
//
// Copyright © 2026 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef CreateVideoMenu_hpp_
#define CreateVideoMenu_hpp_

#include "ActionWithHandlingKeys.hpp"

#include "MenuWithValues.hpp"


namespace gui
{

class Label ;


/**
 * Create menu for adjusting video & graphics
 */

class CreateVideoMenu : public ActionWithHandlingKeys
{

public:

        CreateVideoMenu( )
                : ActionWithHandlingKeys( )
                , videoOptions ( ' ', 1 )
                , screenSize ( nilPointer )
                , fullScreen ( nilPointer )
                , drawShadows ( nilPointer )
                , drawSceneryDecor ( nilPointer )
                , drawRoomMiniatures ( nilPointer )
                , centerCameraOn ( nilPointer )
                , chooseGraphics ( nilPointer )
        {
                videoOptions.setVerticalOffset( 33 );
        }

        virtual ~CreateVideoMenu( ) {}

        virtual void handleKey ( const std::string & key ) ;

protected:

        virtual void act () ;

private:

        void updateOptions () ;

        MenuWithValues videoOptions ;

        Label * screenSize ;
        Label * fullScreen ;
        Label * drawShadows ;
        Label * drawSceneryDecor ;
        Label * drawRoomMiniatures ;
        Label * centerCameraOn ;
        Label * chooseGraphics ;

};

}

#endif

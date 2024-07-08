// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef CreateVideoMenu_hpp_
#define CreateVideoMenu_hpp_

#include "Action.hpp"


namespace gui
{

class MenuWithValues ;
class Label ;


/**
 * Create menu for adjusting video & graphics
 */

class CreateVideoMenu : public Action
{

public:

        CreateVideoMenu( )
                : Action( )
                , videoOptions ( nilPointer )
                , screenSize ( nilPointer )
                , fullScreen ( nilPointer )
                , drawShadows ( nilPointer )
                , drawSceneryDecor ( nilPointer )
                , drawRoomMiniatures ( nilPointer )
                , centerCameraOn ( nilPointer )
                , chooseGraphics ( nilPointer ) {}

        virtual ~CreateVideoMenu( ) {}

protected:

        virtual void act () ;

private:

        void updateOptions () ;

        MenuWithValues * videoOptions ;

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

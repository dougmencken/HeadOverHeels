// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
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
 * Create menu to modify video & graphics
 */

class CreateVideoMenu : public Action
{

public:

        CreateVideoMenu( ) ;

        virtual std::string getNameOfAction () const {  return "CreateVideoMenu" ;  }

protected:

        /**
         * Show this menu
         */
        virtual void doAction () ;

private:

        void updateLabels () ;

        MenuWithValues * listOfOptions ;

        Label * labelScreenSize ;
        Label * labelFullscreen ;
        Label * labelDrawShadows ;
        Label * labelDrawSceneryDecor ;
        Label * labelDrawRoomMiniatures ;
        Label * labelCenterCameraOn ;
        Label * labelChooseGraphics ;

};

}

#endif

// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef PresentTheMainMenu_hpp_
#define PresentTheMainMenu_hpp_

#include "Action.hpp"
#include "TheMainMenuSlide.hpp"


namespace gui
{

/**
 * Show the main menu of the game
 */

class PresentTheMainMenu : public Action
{

public:

        PresentTheMainMenu( ) : Action( ) {}

protected:

        virtual void act () ;

private :

        static TheMainMenuSlide * main_menu_slide ;

};

}

#endif

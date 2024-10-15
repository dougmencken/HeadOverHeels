// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef CreateMainMenu_hpp_
#define CreateMainMenu_hpp_

#include "Action.hpp"
#include "Menu.hpp"


namespace gui
{

/**
 * Show the main menu of the game
 */

class CreateMainMenu : public Action
{

public:

        CreateMainMenu( )
                : Action()
                , mainMenu( )
        {
                mainMenu.setVerticalOffset( 12 );
        }

protected:

        virtual void act () ;

private:

        Menu mainMenu ;

};

}

#endif

// The free and open source remake of Head over Heels
//
// Copyright © 2025 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef TheListOfSavedGamesSlide_hpp_
#define TheListOfSavedGamesSlide_hpp_

#include "SlideWithHeadAndHeels.hpp"
#include "TheListOfSavedGames.hpp"
#include "ContinueGame.hpp"
#include "PresentTheMainMenu.hpp"


namespace gui
{

class TheListOfSavedGamesSlide : public SlideWithHeadAndHeels
{

public :

        TheListOfSavedGamesSlide( bool forLoading )
                : SlideWithHeadAndHeels( )
                , menuOfSavedGames( forLoading )
        {
                placeHeadAndHeels( /* icons */ true, /* copyrights */ false );

                this->menuOfSavedGames.setVerticalOffset( 112 );

                setEscapeAction( this->menuOfSavedGames.isForLoading()
                                        ? static_cast< Action * >( /* to the main menu */ new PresentTheMainMenu() )
                                        : static_cast< Action * >( /* back to the game */ new ContinueGame( true ) ) );

                addWidget( & this->menuOfSavedGames );
                setKeyHandler( & this->menuOfSavedGames );
        }

        TheListOfSavedGames & getMenu () {  return this->menuOfSavedGames ;  }

private :

        TheListOfSavedGames menuOfSavedGames ;

} ;

}

#endif

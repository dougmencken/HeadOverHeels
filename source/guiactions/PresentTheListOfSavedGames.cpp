
#include "PresentTheListOfSavedGames.hpp"

#include "GuiManager.hpp"
#include "SlideWithHeadAndHeels.hpp"
#include "MayNotBePossible.hpp"


namespace gui
{

/* private static */
TheListOfSavedGamesSlide * PresentTheListOfSavedGames::saved_games_for_loading = nilPointer ;
TheListOfSavedGamesSlide * PresentTheListOfSavedGames::saved_games_for_saving = nilPointer ;


void PresentTheListOfSavedGames::act ()
{
        TheListOfSavedGamesSlide * slideWithSavedGames = nilPointer ;

        if ( isForLoading ) {
                if ( PresentTheListOfSavedGames::saved_games_for_loading == nilPointer )
                        PresentTheListOfSavedGames::saved_games_for_loading = new TheListOfSavedGamesSlide( /* for loading */ true ) ;

                slideWithSavedGames = PresentTheListOfSavedGames::saved_games_for_loading ;
        }
        else {
                if ( PresentTheListOfSavedGames::saved_games_for_saving == nilPointer )
                        PresentTheListOfSavedGames::saved_games_for_saving = new TheListOfSavedGamesSlide( false ) ;

                slideWithSavedGames = PresentTheListOfSavedGames::saved_games_for_saving ;
        }

        if ( slideWithSavedGames != nilPointer ) {
                slideWithSavedGames->getMenu().resetActiveOption ();
                slideWithSavedGames->getMenu().updateTheList ();

                GuiManager::getInstance().setSlideForAction( slideWithSavedGames, getNameOfAction() );
                GuiManager::getInstance().changeToSlideFor( getNameOfAction(), true );
        }
        else
                throw MayNotBePossible( std::string( "there’s nil slide for " ) + ( isForLoading ? "loading a saved game" : "saving a game" ) ) ;
}

}

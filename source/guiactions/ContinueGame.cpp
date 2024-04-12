
#include "ContinueGame.hpp"

#include "GameManager.hpp"
#include "GuiManager.hpp"
#include "GameMap.hpp"
#include "SoundManager.hpp"
#include "CreatePlanetsScreen.hpp"
#include "CreateEndScreen.hpp"
#include "CreateListOfSavedGames.hpp"
#include "CreateCongratulationsScreen.hpp"

using gui::ContinueGame;


ContinueGame::ContinueGame( bool inProgress )
        : Action( )
        , gameInProgress( inProgress )
{
}

void ContinueGame::act ()
{
        SoundManager::getInstance().stopOgg (); // or else hear the planets screen's music when resuming an old (saved) game

        GameManager & gameManager = GameManager::getInstance() ;
        gui::GuiManager & uiManager = gui::GuiManager::getInstance() ;

        uiManager.resetWhyTheGameIsPaused () ;

        // until the game is paused
        if ( gameInProgress )
                gameManager.resume () ;
        else
                gameManager.begin () ;

        //  when the game is paused
        if ( uiManager.getWhyTheGameIsPaused().isPlanetLiberated () )
        {
                // when some planet is liberated, show the screen with planets
                CreatePlanetsScreen * planetsAction = new CreatePlanetsScreen( true );

                if ( gameManager.isFreePlanet( "blacktooth" ) )
                        planetsAction->liberateBlacktooth();

                if ( gameManager.isFreePlanet( "egyptus" ) )
                        planetsAction->liberateEgyptus();

                if ( gameManager.isFreePlanet( "penitentiary" ) )
                        planetsAction->liberatePenitentiary();

                if ( gameManager.isFreePlanet( "safari" ) )
                        planetsAction->liberateSafari();

                if ( gameManager.isFreePlanet( "byblos" ) )
                        planetsAction->liberateByblos();

                planetsAction->doIt ();
        }
        else if ( uiManager.getWhyTheGameIsPaused().isTimeToSaveTheGame () )
        {
                // show the save game screen
                CreateListOfSavedGames * listOfGamesAction = new CreateListOfSavedGames( false );

                listOfGamesAction->doIt ();
        }
        else if ( uiManager.getWhyTheGameIsPaused().isGameOver ()
                        || uiManager.getWhyTheGameIsPaused().isInFreedomWithoutAllTheCrowns () )
        {
                CreateEndScreen * endScreenAction =
                        new CreateEndScreen(
                                GameMap::getInstance().howManyVisitedRooms(), gameManager.howManyFreePlanets()
                        );

                endScreenAction->doIt ();
        }
        else if ( uiManager.getWhyTheGameIsPaused().isThatFinalSuccessScreen () )
        {
                CreateCongratulationsScreen * congratulationsScreenAction =
                        new CreateCongratulationsScreen(
                                GameMap::getInstance().howManyVisitedRooms(), gameManager.howManyFreePlanets()
                        );

                congratulationsScreenAction->doIt ();
        }
}

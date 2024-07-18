
#include "ContinueGame.hpp"

#include "GameManager.hpp"
#include "GuiManager.hpp"
#include "GameMap.hpp"
#include "SoundManager.hpp"
#include "CreateGameOverSlide.hpp"
#include "CreateListOfSavedGames.hpp"
#include "ShowCongratulations.hpp"
#include "ShowSlideWithPlanets.hpp"

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
                // when some planet is liberated, show the slide with planets
                ShowSlideWithPlanets * planetsAction = new ShowSlideWithPlanets( true );
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
                CreateGameOverSlide * gameOverScoreAction =
                        new CreateGameOverSlide (
                                GameMap::getInstance().howManyVisitedRooms(), gameManager.howManyFreePlanets() );

                gameOverScoreAction->doIt ();
        }
        else if ( uiManager.getWhyTheGameIsPaused().isFinalSuccess () )
        {
                ShowCongratulations * congratulationsAction =
                        new ShowCongratulations (
                                GameMap::getInstance().howManyVisitedRooms(), gameManager.howManyFreePlanets() );

                congratulationsAction->doIt ();
        }
}


#include "ContinueGame.hpp"

#include "GameManager.hpp"
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

        ////if ( gameManager.getKeyMoments().isThereAny() )
                gameManager.resetKeyMoments() ;

        // until the game is paused
        if ( gameInProgress )
                gameManager.resume () ;
        else
                gameManager.begin () ;

        // when the game is paused or there’s a key moment in the game
        if ( gameManager.getKeyMoments().wasCrownTaken () ) {
                // when some planet is liberated, show the slide with planets
                ShowSlideWithPlanets * planetsAction = new ShowSlideWithPlanets( true );
                planetsAction->doIt ();
        }
        else if ( gameManager.getKeyMoments().isSavingGame () )
        {
                // show the save game screen
                CreateListOfSavedGames * listOfGamesAction = new CreateListOfSavedGames( false );
                listOfGamesAction->doIt ();
        }
        else if ( gameManager.getKeyMoments().isGameOver ()
                        || gameManager.getKeyMoments().arrivedInFreedomNotWithAllCrowns () )
        {
                CreateGameOverSlide * gameOverScoreAction =
                        new CreateGameOverSlide (
                                GameMap::getInstance().howManyVisitedRooms(), gameManager.howManyFreePlanets() );

                gameOverScoreAction->doIt ();
        }
        else if ( gameManager.getKeyMoments().isFinalSuccess () )
        {
                ShowCongratulations * congratulationsAction =
                        new ShowCongratulations (
                                GameMap::getInstance().howManyVisitedRooms(), gameManager.howManyFreePlanets() );

                congratulationsAction->doIt ();
        }
}

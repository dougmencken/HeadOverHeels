
#include "ContinueGame.hpp"
#include "GameManager.hpp"
#include "SoundManager.hpp"
#include "CreatePlanetsScreen.hpp"
#include "CreateEndScreen.hpp"
#include "CreateListOfSavedGames.hpp"
#include "CreateCongratulationsScreen.hpp"

using gui::ContinueGame;


ContinueGame::ContinueGame( bool gameInProgress )
        : Action( )
        , gameInProgress( gameInProgress )
{

}

void ContinueGame::doAction ()
{
        iso::SoundManager::getInstance().stopOgg (); // or hear music of planets screen when playing restored game

        iso::GameManager& gameManager = iso::GameManager::getInstance();
        iso::WhyPaused why = gameInProgress ? gameManager.resume () : gameManager.begin () ;

        if ( why.toCallOnlyWithinDoActionOfContinueGame_isPlanetLiberated () )
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
        else if ( why.toCallOnlyWithinDoActionOfContinueGame_isTimeToSaveTheGame () )
        {
                // show the save game screen
                CreateListOfSavedGames * listOfGamesAction = new CreateListOfSavedGames( false );

                listOfGamesAction->doIt ();
        }
        else if ( /* all lives lost */ why.toCallOnlyWithinDoActionOfContinueGame_isAllLivesLost ()
                    || /* in Freedom not with all crowns */ why.toCallOnlyWithinDoActionOfContinueGame_isInFreedomWithoutAllTheCrowns () )
        {
                CreateEndScreen * endScreenAction =
                        new CreateEndScreen(
                                gameManager.getVisitedRooms(), gameManager.countFreePlanets()
                        );

                endScreenAction->doIt ();
        }
        else if ( why.toCallOnlyWithinDoActionOfContinueGame_isThatFinalSuccessScreen () )
        {
                CreateCongratulationsScreen * congratulationsScreenAction =
                        new CreateCongratulationsScreen(
                                gameManager.getVisitedRooms(), gameManager.countFreePlanets()
                        );

                congratulationsScreenAction->doIt ();
        }
}

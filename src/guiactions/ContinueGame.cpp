
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

        // when some planet is set free, show screen with planets
        if ( why == iso::WhyPaused::FreePlanet )
        {
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
        // show screen to save game
        else if ( why == iso::WhyPaused::SaveGame )
        {
                CreateListOfSavedGames * listOfGamesAction = new CreateListOfSavedGames( false );

                listOfGamesAction->doIt ();
        }
        // it’s end of game
        else if ( why == iso::WhyPaused::GameOver || why == iso::WhyPaused::Freedom )
        {
                CreateEndScreen * endScreenAction =
                        new CreateEndScreen(
                                gameManager.getVisitedRooms(), gameManager.countFreePlanets()
                        );

                endScreenAction->doIt ();
        }
        else if ( why == iso::WhyPaused::Success )
        {
                CreateCongratulationsScreen * congratulationsScreenAction =
                        new CreateCongratulationsScreen(
                                gameManager.getVisitedRooms(), gameManager.countFreePlanets()
                        );

                congratulationsScreenAction->doIt ();
        }
}

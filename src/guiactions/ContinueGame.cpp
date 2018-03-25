
#include "ContinueGame.hpp"
#include "GameManager.hpp"
#include "SoundManager.hpp"
#include "CreatePlanetsScreen.hpp"
#include "CreateEndScreen.hpp"
#include "CreateListOfSavedGames.hpp"
#include "CreateCongratulationsScreen.hpp"

using gui::ContinueGame;
using isomot::SoundManager;


ContinueGame::ContinueGame( BITMAP* picture, bool gameInProgress )
        : Action( picture )
        , gameInProgress( gameInProgress )
{

}

void ContinueGame::doAction ()
{
        SoundManager::getInstance()->stopAnyOgg (); // or hear music of planets screen when playing after restore

        isomot::GameManager * gameManager = isomot::GameManager::getInstance();
        isomot::WhyPause why = gameInProgress ? gameManager->resume () : gameManager->begin () ;

        // when some planet is set free, show screen with planets
        if ( why == isomot::FreePlanet )
        {
                CreatePlanetsScreen * planetsAction = new CreatePlanetsScreen( getWhereToDraw(), true );

                if ( gameManager->isFreePlanet( "blacktooth" ) )
                        planetsAction->liberateBlacktooth();

                if ( gameManager->isFreePlanet( "egyptus" ) )
                        planetsAction->liberateEgyptus();

                if ( gameManager->isFreePlanet( "penitentiary" ) )
                        planetsAction->liberatePenitentiary();

                if ( gameManager->isFreePlanet( "safari" ) )
                        planetsAction->liberateSafari();

                if ( gameManager->isFreePlanet( "byblos" ) )
                        planetsAction->liberateByblos();

                planetsAction->doIt ();
        }
        // show screen to save game
        else if ( why == isomot::SaveGame )
        {
                CreateListOfSavedGames * listOfGamesAction = new CreateListOfSavedGames( getWhereToDraw(), false );

                listOfGamesAction->doIt ();
        }
        // it’s end of game
        else if ( why == isomot::GameOver || why == isomot::Freedom )
        {
                CreateEndScreen * endScreenAction =
                        new CreateEndScreen(
                                getWhereToDraw(), gameManager->getVisitedRooms(), gameManager->countFreePlanets()
                        );

                endScreenAction->doIt ();
        }
        else if ( why == isomot::Sucess )
        {
                CreateCongratulationsScreen * congratulationsScreenAction =
                        new CreateCongratulationsScreen(
                                getWhereToDraw(), gameManager->getVisitedRooms(), gameManager->countFreePlanets()
                        );

                congratulationsScreenAction->doIt ();
        }
}

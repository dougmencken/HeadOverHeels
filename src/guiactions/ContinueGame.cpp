
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
: where( picture ),
  gameInProgress( gameInProgress )
{

}

void ContinueGame::doIt ()
{
        SoundManager::getInstance()->stopAnyOgg (); // or hear music of planets screen when playing after restore

        isomot::GameManager * gameManager = isomot::GameManager::getInstance();
        isomot::WhyPause why = gameInProgress ? gameManager->resume () : gameManager->begin () ;

        // Cuando el gestor del juego finalice se alcanzará este punto
        // Dependiendo de la situación deberá mostrarse la pantalla de los planetas con el nuevo planeta
        // liberado, la pantalla para grabar la partida o la pantalla de fin del juego
        if ( why == isomot::FreePlanet )
        {
                CreatePlanetsScreen * planetsAction = new CreatePlanetsScreen( this->where, true );

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
        else if ( why == isomot::SaveGame )
        {
                CreateListOfSavedGames * listOfGamesAction = new CreateListOfSavedGames( this->where, false );

                listOfGamesAction->doIt ();
        }
        else if ( why == isomot::GameOver || why == isomot::Freedom )
        {
                CreateEndScreen * endScreenAction =
                        new CreateEndScreen(
                                this->where, gameManager->getVisitedRooms(), gameManager->countFreePlanets()
                        );

                endScreenAction->doIt ();
        }
        else if ( why == isomot::Sucess )
        {
                CreateCongratulationsScreen * congratulationsScreenAction =
                        new CreateCongratulationsScreen(
                                this->where, gameManager->getVisitedRooms(), gameManager->countFreePlanets()
                        );

                congratulationsScreenAction->doIt ();
        }
}

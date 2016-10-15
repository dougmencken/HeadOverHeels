#include "BeginGame.hpp"
#include "GameManager.hpp"
#include "SoundManager.hpp"
#include "CreatePlanetsScreen.hpp"
#include "CreateEndScreen.hpp"
#include "CreateListOfSavedGames.hpp"
#include "CreateCongratulationsScreen.hpp"

using gui::BeginGame;
using isomot::SoundManager;


BeginGame::BeginGame( BITMAP* picture, bool gameInProgress )
: where( picture ),
  gameInProgress( gameInProgress )
{

}

void BeginGame::doIt ()
{
        // Detiene la reproducción de la música
        SoundManager::getInstance()->stopOgg();

        isomot::GameManager* gameManager = isomot::GameManager::getInstance();
        isomot::StopCode stopCode = gameInProgress ? gameManager->restart() : gameManager->start();

        // Cuando el gestor del juego finalice se alcanzará este punto
        // Dependiendo de la situación deberá mostrarse la pantalla de los planetas con el nuevo planeta
        // liberado, la pantalla para grabar la partida o la pantalla de fin del juego
        if ( stopCode == isomot::FreePlanet )
        {
                CreatePlanetsScreen planetsScreen( this->where, true );

                if ( gameManager->isFreePlanet( isomot::Blacktooth ) )
                {
                        planetsScreen.blacktoothIsFree();
                }

                if ( gameManager->isFreePlanet( isomot::Egyptus ) )
                {
                        planetsScreen.egyptusIsFree();
                }

                if ( gameManager->isFreePlanet( isomot::Penitentiary ) )
                {
                        planetsScreen.penitentiaryIsFree();
                }

                if ( gameManager->isFreePlanet( isomot::Safari ) )
                {
                        planetsScreen.safariIsFree();
                }

                if ( gameManager->isFreePlanet( isomot::Byblos ) )
                {
                        planetsScreen.byblosIsFree();
                }

                planetsScreen.doIt ();
        }
        else if ( stopCode == isomot::SaveGame )
        {
                CreateListOfSavedGames listOfGames( this->where, false );
                listOfGames.doIt ();
        }
        else if ( stopCode == isomot::UserAction || stopCode == isomot::Freedom )
        {
                CreateEndScreen endScreen( this->where, gameManager->getVisitedRooms(), gameManager->freePlanetsCount() );
                endScreen.doIt ();
        }
        else if ( stopCode == isomot::Sucess )
        {
                CreateCongratulationsScreen congratulationsScreen( this->where, gameManager->getVisitedRooms(), gameManager->freePlanetsCount() );
                congratulationsScreen.doIt ();
        }
}

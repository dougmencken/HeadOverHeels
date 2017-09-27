
#include "LoadGame.hpp"
#include "Ism.hpp"
#include "GameManager.hpp"
#include "CreatePlanetsScreen.hpp"

using gui::LoadGame;
using gui::CreatePlanetsScreen;


LoadGame::LoadGame( BITMAP* picture, int slot )
: Action(),
  where( picture ),
  slot( slot )
{

}

void LoadGame::doIt ()
{
        std::stringstream ss;
        isomot::GameManager* gameManager = isomot::GameManager::getInstance();
        gameManager->resetPlanets();
        ss << isomot::homePath() << "savegame/save" << slot << ".xml";
        gameManager->loadGame( ss.str() );

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

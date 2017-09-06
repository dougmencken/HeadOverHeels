
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
        gameManager->resetFreePlanets();
        ss << isomot::homePath() << "savegame/savegame" << slot << ".xml";
        gameManager->loadGame( ss.str() );

        CreatePlanetsScreen * planetsAction = new CreatePlanetsScreen( this->where, true );

        if ( gameManager->isFreePlanet( isomot::Blacktooth ) )
                planetsAction->liberateBlacktooth();

        if ( gameManager->isFreePlanet( isomot::Egyptus ) )
                planetsAction->liberateEgyptus();

        if ( gameManager->isFreePlanet( isomot::Penitentiary ) )
                planetsAction->liberatePenitentiary();

        if ( gameManager->isFreePlanet( isomot::Safari ) )
                planetsAction->liberateSafari();

        if ( gameManager->isFreePlanet( isomot::Byblos ) )
                planetsAction->liberateByblos();

        planetsAction->doIt ();
}

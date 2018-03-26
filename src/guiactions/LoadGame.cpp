
#include "LoadGame.hpp"
#include "Ism.hpp"
#include "GameManager.hpp"
#include "CreatePlanetsScreen.hpp"

using gui::LoadGame;
using gui::CreatePlanetsScreen;


LoadGame::LoadGame( BITMAP* picture, unsigned int slot )
        : Action( picture )
        , slot( slot )
{

}

void LoadGame::doAction ()
{
        isomot::GameManager* gameManager = isomot::GameManager::getInstance();
        gameManager->resetPlanets();

        std::stringstream ss;
        ss << slot;
        gameManager->loadGame( isomot::homePath() + "savegame" + pathSeparator + "saved" + ss.str() + ".xml" );

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

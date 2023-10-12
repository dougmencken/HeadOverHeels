
#include "LoadGame.hpp"

#include "ospaths.hpp"

#include "GameManager.hpp"
#include "CreatePlanetsScreen.hpp"

using gui::LoadGame;
using gui::CreatePlanetsScreen;


LoadGame::LoadGame( unsigned int slot )
        : Action( )
        , slot( slot )
{

}

void LoadGame::doAction ()
{
        game::GameManager& gameManager = game::GameManager::getInstance();
        gameManager.resetPlanets();
        gameManager.loadGame( ospaths::homePath () + "savegame" + ospaths::pathSeparator () + "saved." + util::number2string( slot ) );

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

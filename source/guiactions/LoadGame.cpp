
#include "LoadGame.hpp"

#include "ospaths.hpp"

#include "GameManager.hpp"
#include "SoundManager.hpp"
#include "CreatePlanetsScreen.hpp"
#include "CreateMainMenu.hpp"

using gui::LoadGame;
using gui::CreatePlanetsScreen;


LoadGame::LoadGame( unsigned int slot )
        : Action( )
        , slot( slot )
{
}

void LoadGame::doAction ()
{
        GameManager & gameManager = GameManager::getInstance () ;
        gameManager.resetPlanets() ;

        bool loaded = gameManager.loadGame( ospaths::homePath () + "savegame" + ospaths::pathSeparator () + "saved." + util::number2string( slot ) );

        if ( ! loaded ) {
                SoundManager::getInstance().stopEverySound ();
                SoundManager::getInstance().play( "menus", "mistake", /* loop */ false );

                ( new CreateMainMenu() )->doIt () ;
                return ;
        }

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

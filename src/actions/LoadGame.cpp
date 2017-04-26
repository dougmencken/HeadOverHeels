
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
                planetsAction->blacktoothIsFree();

        if ( gameManager->isFreePlanet( isomot::Egyptus ) )
                planetsAction->egyptusIsFree();

        if ( gameManager->isFreePlanet( isomot::Penitentiary ) )
                planetsAction->penitentiaryIsFree();

        if ( gameManager->isFreePlanet( isomot::Safari ) )
                planetsAction->safariIsFree();

        if ( gameManager->isFreePlanet( isomot::Byblos ) )
                planetsAction->byblosIsFree();

        planetsAction->doIt ();
}

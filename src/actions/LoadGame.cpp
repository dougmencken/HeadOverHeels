#include "LoadGame.hpp"
#include "Ism.hpp"
#include "GameManager.hpp"
#include "CreatePlanetsScreen.hpp"

using gui::LoadGame;
using gui::CreatePlanetsScreen;

LoadGame::LoadGame(BITMAP* destination, int slot)
: Action(),
  destination(destination),
  slot(slot)
{

}

void LoadGame::execute()
{
  std::stringstream ss;
  isomot::GameManager* gameManager = isomot::GameManager::getInstance();
  gameManager->resetFreePlanets();
  ss << isomot::homePath() << "savegame/savegame" << slot << ".xml";
  gameManager->loadGame(ss.str());

  CreatePlanetsScreen planetsScreen(this->destination, true);

  if(gameManager->isFreePlanet(isomot::Blacktooth))
  {
    planetsScreen.blacktoothIsFree();
  }

  if(gameManager->isFreePlanet(isomot::Egyptus))
  {
    planetsScreen.egyptusIsFree();
  }

  if(gameManager->isFreePlanet(isomot::Penitentiary))
  {
    planetsScreen.penitentiaryIsFree();
  }

  if(gameManager->isFreePlanet(isomot::Safari))
  {
    planetsScreen.safariIsFree();
  }

  if(gameManager->isFreePlanet(isomot::Byblos))
  {
    planetsScreen.byblosIsFree();
  }

  planetsScreen.execute();
}

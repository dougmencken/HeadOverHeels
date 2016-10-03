#include "SaveGame.hpp"
#include "Ism.hpp"
#include "GameManager.hpp"
#include "StartGame.hpp"

using gui::SaveGame;
using gui::StartGame;

SaveGame::SaveGame(BITMAP* destination, int slot)
: Action(),
  destination(destination),
  slot(slot)
{

}

void SaveGame::execute()
{
  // Se graba la partida en la ubicación especificada
  if(slot != 0)
  {
    std::stringstream ss;
    isomot::GameManager* gameManager = isomot::GameManager::getInstance();
    ss << isomot::homePath() << "savegame/savegame" << slot << ".xml";
    gameManager->saveGame(ss.str());
  }

  // Se vuelve a la partida en curso
  StartGame startGame(this->destination, true);
  startGame.execute();
}



#include "SaveGame.hpp"
#include "Ism.hpp"
#include "GameManager.hpp"
#include "ContinueGame.hpp"

using gui::SaveGame;
using gui::ContinueGame;


SaveGame::SaveGame( BITMAP* picture, int slot )
: Action(),
  where( picture ),
  slot( slot )
{

}

void SaveGame::doIt ()
{
        // Se graba la partida en la ubicación especificada
        if ( slot != 0 )
        {
                std::stringstream ss;
                isomot::GameManager* gameManager = isomot::GameManager::getInstance();
                ss << isomot::homePath() << "savegame/savegame" << slot << ".xml";
                gameManager->saveGame( ss.str() );
        }

        ContinueGame game( this->where, true );
        game.doIt ();
}

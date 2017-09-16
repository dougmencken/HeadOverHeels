
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
        clear_keybuf();

        // Se graba la partida en la ubicación especificada
        if ( slot != 0 )
        {
                std::stringstream ss;
                isomot::GameManager* gameManager = isomot::GameManager::getInstance();
                ss << isomot::homePath() << "savegame/save" << slot << ".xml";
                gameManager->saveGame( ss.str() );
        }

        ContinueGame * game = new ContinueGame( this->where, true );
        game->doIt ();
}

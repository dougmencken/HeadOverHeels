#include "SaveGame.hpp"
#include "Ism.hpp"
#include "GameManager.hpp"
#include "BeginGame.hpp"

using gui::SaveGame;
using gui::BeginGame;

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

        // Se vuelve a la partida en curso
        BeginGame begin( this->where, true );
        begin.doIt ();
}

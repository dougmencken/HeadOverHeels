
#include "SaveGame.hpp"
#include "Ism.hpp"
#include "GameManager.hpp"
#include "ContinueGame.hpp"

using gui::SaveGame;
using gui::ContinueGame;


SaveGame::SaveGame( Picture* picture, unsigned int slot )
        : Action( picture )
        , slot( slot )
{

}

void SaveGame::doAction ()
{
        allegro::emptyKeyboardBuffer();

        if ( slot > 0 )
        {
                iso::GameManager& gameManager = iso::GameManager::getInstance();
                gameManager.saveGame( iso::homePath() + "savegame" + util::pathSeparator() + "saved." + util::number2string( slot ) );
        }

        ContinueGame * game = new ContinueGame( getWhereToDraw(), true );
        game->doIt ();
}

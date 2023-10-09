
#include "SaveGame.hpp"

#include "ospaths.hpp"

#include "GameManager.hpp"
#include "ContinueGame.hpp"

using gui::SaveGame;
using gui::ContinueGame;


SaveGame::SaveGame( unsigned int slot )
        : Action( )
        , slot( slot )
{

}

void SaveGame::doAction ()
{
        allegro::emptyKeyboardBuffer();

        if ( slot > 0 )
        {
                iso::GameManager& gameManager = iso::GameManager::getInstance();
                gameManager.saveGame( ospaths::homePath () + "savegame" + ospaths::pathSeparator () + "saved." + util::number2string( slot ) );
        }

        ContinueGame * game = new ContinueGame( true );
        game->doIt ();
}

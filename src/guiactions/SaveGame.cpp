
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
                isomot::GameManager* gameManager = isomot::GameManager::getInstance();
                gameManager->saveGame( isomot::homePath() + "savegame" + pathSeparator + "saved." + isomot::numberToString( slot ) );
        }

        ContinueGame * game = new ContinueGame( getWhereToDraw(), true );
        game->doIt ();
}

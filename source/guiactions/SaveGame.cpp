
#include "SaveGame.hpp"

#include "ospaths.hpp"

#include "GameManager.hpp"
#include "ContinueGame.hpp"

namespace gui
{

SaveGame::SaveGame( unsigned int slot )
        : Action( )
        , slot( slot )
{
}

void SaveGame::act ()
{
        allegro::emptyKeyboardBuffer();

        if ( slot > 0 )
        {
                std::string where = ospaths::homePath () + "savegame" ;
                ospaths::makeFolder( where );
                bool saved = GameManager::getInstance().saveGame (
                                ospaths::pathToFile( where, "saved." + util::number2string( slot ) )
                );
                if ( ! saved )
                        std::cout << "can’t save the game as \"" << "saved." << slot << "\"" << std::endl ;
        }

        // back to the game
        ( new ContinueGame() )->doIt ();
}

}

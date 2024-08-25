
#include "ContinueGame.hpp"

#include "GameManager.hpp"
#include "SoundManager.hpp"


void gui::ContinueGame::act ()
{
        SoundManager::getInstance().stopOgg (); // or else hear the planets screen’s music when resuming an old (saved) game

        GameManager & gameManager = GameManager::getInstance() ;

        gameManager.resetKeyMoments() ;

        if ( gameInProgress )
                gameManager.resume () ;
        else
                gameManager.begin () ;
}

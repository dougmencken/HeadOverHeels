
#include "ContinueGame.hpp"

#include "GameManager.hpp"
#include "SoundManager.hpp"

#include <pthread.h>


void gui::ContinueGame::act ()
{
        SoundManager::getInstance().stopOgg (); // or else hear the planets screen’s music when resuming an old (saved) game

        bool * pointer = new bool ;
        *pointer = GameManager::getInstance().isGameRunning() ;

        pthread_create( GameManager::getThread(), nilPointer, GameManager::beginOrResume, reinterpret_cast< void * >( pointer ) );
        pthread_detach( * GameManager::getThread() );
}

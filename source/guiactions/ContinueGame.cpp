
#include "ContinueGame.hpp"

#include "GameManager.hpp"
#include "SoundManager.hpp"

#include <pthread.h>


void gui::ContinueGame::act ()
{
        SoundManager::getInstance().stopOgg (); // or else hear the planets screen’s music when resuming an old (saved) game

        pthread_create( GameManager::getThread(),
                                /* thread attributes */ nilPointer,
                                GameManager::beginOrResume, nilPointer );
        pthread_detach( * GameManager::getThread() );
}

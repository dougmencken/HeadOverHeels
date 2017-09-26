
#include "Isomot.hpp"
#include "GameManager.hpp"
#include "GuiManager.hpp"
#include "ItemDataManager.hpp"
#include "MapManager.hpp"
#include "BonusManager.hpp"
#include "InputManager.hpp"
#include "SoundManager.hpp"
#include "Room.hpp"
#include "Mediator.hpp"
#include "Camera.hpp"
#include "PlayerItem.hpp"
#include "Behavior.hpp"


namespace isomot
{

Isomot::Isomot( ) :
        isEndRoom( false ),
        view( 0 ),
        itemDataManager( 0 ),
        mapManager( 0 )
{

}

Isomot::~Isomot( )
{
        delete this->mapManager;
        delete this->itemDataManager;

        if ( this->view != 0 )
        {
                destroy_bitmap( this->view );
                this->view = 0;
        }
}

void Isomot::beginNewGame ()
{
        prepare() ;
        offVidasInfinitas() ;
        offRightToInviolability() ;
        this->isEndRoom = false;

        // initial rooms
        mapManager->beginNewGame( "blacktooth/blacktooth01.xml", "blacktooth/blacktooth23.xml" );
        //mapManager->beginNewGame( "blacktooth/blacktooth85.xml", "blacktooth/blacktooth23.xml" );

        // go to the initial room
        mapManager->getActiveRoom()->activate ();

        std::cout << "play new game" << std::endl ;
        SoundManager::getInstance()->playOgg ( "music/begin.ogg", /* loop */ false );
}

void Isomot::continueSavedGame ( const sgxml::players::player_sequence& playerSequence )
{
        offVidasInfinitas() ;
        offRightToInviolability() ;
        this->isEndRoom = false;

        for ( sgxml::players::player_const_iterator i = playerSequence.begin (); i != playerSequence.end (); ++i )
        {
                this->mapManager->beginOldGameWithPlayer( *i );
        }

        std::cout << "continue previous game" << std::endl ;
        // no begin.ogg here
}

void Isomot::prepare ()
{
        // image where the isometric view will be drawn
        if ( this->view == 0 )
        {
                this->view = create_bitmap_ex( 32, ScreenWidth, ScreenHeight );
        }

        // data for elements of the game
        if ( this->itemDataManager == 0 )
        {
                this->itemDataManager = new ItemDataManager( "items.xml" );
                this->itemDataManager->loadItems ();
        }

        // data for map
        if ( this->mapManager == 0 )
        {
                this->mapManager = new MapManager( this, "map.xml" );
                this->mapManager->loadMap ();
        }
}

void Isomot::offVidasInfinitas ()
{
        if ( GameManager::getInstance()->areLivesInexhaustible () )
        {
                GameManager::getInstance()->toggleInfiniteLives ();
        }
}

void Isomot::offRightToInviolability ()
{
        if ( GameManager::getInstance()->isImmuneToCollisionsWithMortalItems () )
        {
                GameManager::getInstance()->toggleImmunityToCollisionsWithMortalItems ();
        }
}

void Isomot::pause ()
{
        // Detiene la actualización de los estados de los elementos y
        // la representación de la vista isométrica
        if ( mapManager->getActiveRoom() != 0 )
        {
                mapManager->getActiveRoom()->deactivate();
        }
}

void Isomot::resume ()
{
        // Reactiva la actualización de los estados de los elementos y
        // la representación de la vista isométrica
        if( mapManager->getActiveRoom() != 0 )
        {
                mapManager->getActiveRoom()->activate();
        }
}

void Isomot::reset()
{
        this->isEndRoom = false;

        // Destruye la vista isométrica actual
        if ( this->view != 0 )
        {
                destroy_bitmap( this->view );
                this->view = 0;
        }

        // El gestor del mapa debe limpiar todos sus datos a excepción del propio mapa
        this->mapManager->reset();

        // Elimina los bonus marcados como ausentes en todas las salas
        BonusManager::getInstance()->reset();
}

BITMAP* Isomot::update()
{
        Room* activeRoom = mapManager->getActiveRoom();
        GameManager* gameManager = GameManager::getInstance();

        if( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && key[ KEY_F ] )
        {
                gui::GuiManager::getInstance()->toggleFullScreenVideo ();
                key[ KEY_F ] = 0;
        }

        if( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && key[ KEY_I ] )
        {
                // Activa o desactiva las vidas infinitas
                gameManager->toggleInfiniteLives ();
                key[ KEY_I ] = 0;
        }

        if( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && key[ KEY_C ] )
        {
                gameManager->toggleImmunityToCollisionsWithMortalItems ();
                key[ KEY_C ] = 0;
        }

        if( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && key[ KEY_B ] )
        {
                gameManager->toggleBackgroundPicture ();
                key[ KEY_B ] = 0;
        }

        if( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && key[ KEY_S ] )
        {
                gameManager->eatFish ( activeRoom->getMediator()->getActivePlayer(), activeRoom );
                key[ KEY_S ] = 0;
        }

        if( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && key[ KEY_8_PAD ] )
        {
                activeRoom->getCamera()->setDeltaY( activeRoom->getCamera()->getDeltaY() - 2 );
        }
        if( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && key[ KEY_2_PAD ] )
        {
                activeRoom->getCamera()->setDeltaY( activeRoom->getCamera()->getDeltaY() + 2 );
        }
        if( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && key[ KEY_4_PAD ] )
        {
                activeRoom->getCamera()->setDeltaX( activeRoom->getCamera()->getDeltaX() - 2 );
        }
        if( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && key[ KEY_6_PAD ] )
        {
                activeRoom->getCamera()->setDeltaX( activeRoom->getCamera()->getDeltaX() + 2 );
        }

        if( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && key[ KEY_MINUS ] )
        {
                activeRoom->removeBars ();
        }

        // swap key changes character and possibly room
        if ( ! this->isEndRoom && InputManager::getInstance()->swap() )
        {
                activeRoom->getMediator()->getActivePlayer()->wait(); // stop active player
                if ( activeRoom->getMediator()->getActivePlayer()->getBehavior()->getActivityOfItem() == Wait )
                {
                        // swap in the same room
                        if ( activeRoom->swapPlayersInRoom( this->itemDataManager ) )
                        {
                                mapManager->updateActivePlayer();
                        }
                        // swap in different rooms
                        else
                        {
                                activeRoom = mapManager->swapRoom();
                        }
                }
                // don’t repeat the key
                InputManager::getInstance()->noRepeat( "swap" );
        }

        if ( activeRoom->isActive() )
        {
                activeRoom->draw();
        }
        // when "active" room is in fact inactive, there’s a change of room
        // or active player lost its life
        else
        {
                Direction exit = activeRoom->getExit();

                if ( exit == Restart )
                {
                        PlayerItem* player = activeRoom->getMediator()->getActivePlayer();

                        if ( player->getLives() != 0 || ( player->getLabel() == "headoverheels" && player->getLives() == 0 ) )
                        {
                                activeRoom = mapManager->restartRoom();
                        }
                        else
                        {
                                // when player is alive, just swap players
                                if ( activeRoom->continueWithAlivePlayer( this->itemDataManager ) )
                                {
                                        mapManager->updateActivePlayer();
                                }
                                else
                                {
                                        activeRoom = mapManager->removeRoomAndSwap ();
                                }
                        }
                }
                else
                {
                        activeRoom = mapManager->changeRoom( exit );

                        std::string scenery = activeRoom->getScenery ();
                        SoundManager::getInstance()->playOgg ( "music/" + scenery + ".ogg", /* loop */ false );
                }
        }

        // Si hay una sala activa, se dibuja
        if ( activeRoom != 0 )
        {
                blit (
                        activeRoom->getPicture(), this->view,
                        activeRoom->getCamera()->getDeltaX(), activeRoom->getCamera()->getDeltaY(),
                        0, 0,
                        activeRoom->getPicture()->w, activeRoom->getPicture()->h
                );

                // Delata al tramposo
                if ( GameManager::getInstance()->areLivesInexhaustible () )
                {
                        textout_ex( this->view, font, "VIDAS INFINITAS", 18, 10, makecol( 255, 255, 255 ), -1 );
                        textout_ex( this->view, font, "INFINITE LIVES", this->view->w - 128, 10, makecol( 255, 255, 255 ), -1 );
                }

                if ( GameManager::getInstance()->isImmuneToCollisionsWithMortalItems () )
                {
                        textout_ex( this->view, font, "RIGHT TO INVIOLABILITY", ( this->view->w >> 1 ) - 88, 10, makecol( 255, 255, 255 ), -1 );
                }

                // La sala final es muy especial
                if ( activeRoom->getIdentifier().compare( "blacktooth/blacktooth88.xml" ) == 0 )
                {
                        this->updateEndRoom();
                }
        }
        // Si no hay sala activa es que la partida ha terminado
        else
        {
                destroy_bitmap( this->view );
                this->view = 0;
        }

        return this->view;
}

void Isomot::updateEndRoom()
{
        Room* activeRoom = mapManager->getActiveRoom();

        if ( ! this->isEndRoom )
        {
                std::string player = activeRoom->getMediator()->getActivePlayer()->getLabel();
                activeRoom->getMediator()->removeItem( activeRoom->getMediator()->getActivePlayer() );

                // player who arrived here
                FreeItem* freeItem = new FreeItem( this->itemDataManager->findItemByLabel( player ), 66, 92, Top, South );
                activeRoom->addFreeItem( freeItem );

                // crea las coronas recuperadas

                GameManager* gameManager = GameManager::getInstance();
                int crowns = 0;

                // la corona de Safari
                if ( gameManager->isFreePlanet( Safari ) )
                {
                        freeItem = new FreeItem( this->itemDataManager->findItemByLabel( "crown" ), 66, 75, Top, NoDirection );
                        activeRoom->addFreeItem( freeItem );
                        crowns++;
                }
                // la corona de Egyptus
                if ( gameManager->isFreePlanet( Egyptus ) )
                {
                        freeItem = new FreeItem( this->itemDataManager->findItemByLabel( "crown" ), 66, 59, Top, NoDirection );
                        activeRoom->addFreeItem( freeItem );
                        crowns++;
                }
                // la corona de Penitentiary
                if ( gameManager->isFreePlanet( Penitentiary ) )
                {
                        freeItem = new FreeItem( this->itemDataManager->findItemByLabel( "crown" ), 65, 107, Top, NoDirection );
                        activeRoom->addFreeItem( freeItem );
                        crowns++;
                }
                // la corona de Byblos
                if ( gameManager->isFreePlanet( Byblos ) )
                {
                        freeItem = new FreeItem( this->itemDataManager->findItemByLabel( "crown" ), 65, 123, Top, NoDirection );
                        activeRoom->addFreeItem( freeItem );
                        crowns++;
                }
                // la corona de Blacktooth
                if ( gameManager->isFreePlanet( Blacktooth ) )
                {
                        freeItem = new FreeItem( this->itemDataManager->findItemByLabel( "crown" ), 65, 91, Top, NoDirection );
                        activeRoom->addFreeItem( freeItem );
                        crowns++;
                }

                // Si se han conseguido las cinco coronas se mostrará la pantalla de felicitación
                if ( crowns == 5 )
                {
                        gameManager->success();
                }
                // Si no se saldrá directamente a la pantalla resumen
                else
                {
                        gameManager->arriveFreedom();
                }

                // Ahora la sala ya está lista
                this->isEndRoom = true;
        }
        else
        {
                if ( activeRoom->getMediator()->findItemByLabel( "ball" ) == 0 && activeRoom->getMediator()->findItemByLabel( "bubbles" ) == 0 )
                {
                        FreeItem* freeItem = new FreeItem( this->itemDataManager->findItemByLabel( "ball" ), 146, 93, LayerHeight, NoDirection );
                        freeItem->assignBehavior( "behaivor of final ball", this->itemDataManager->findItemByLabel( "bubbles" ) );
                        activeRoom->addFreeItem( freeItem );
                }
        }
}

ItemDataManager* Isomot::getItemDataManager() const
{
        return itemDataManager;
}

MapManager* Isomot::getMapManager() const
{
        return mapManager;
}

}

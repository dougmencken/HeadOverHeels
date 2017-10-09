
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
        offInviolability() ;
        this->isEndRoom = false;

        // initial rooms
        mapManager->beginNewGame( "blacktooth/blacktooth01.xml", "blacktooth/blacktooth23.xml" );
        //mapManager->beginNewGame( "blacktooth/blacktooth85.xml", "blacktooth/blacktooth23.xml" );

        // go to first room
        mapManager->getActiveRoom()->activate ();

        std::cout << "play new game" << std::endl ;
        SoundManager::getInstance()->playOgg ( "music/begin.ogg", /* loop */ false );
}

void Isomot::continueSavedGame ( const sgxml::players::player_sequence& playerSequence )
{
        offVidasInfinitas() ;
        offInviolability() ;
        this->isEndRoom = false;

        for ( sgxml::players::player_const_iterator i = playerSequence.begin (); i != playerSequence.end (); ++i )
        {
                this->mapManager->beginOldGameWithCharacter( *i );
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

        if ( this->itemDataManager != 0 )
        {
                // set of graphics may change between games
                // new ItemDataManager is needed to refresh pictures of items
                delete this->itemDataManager;
                this->itemDataManager = 0;
        }

        this->itemDataManager = new ItemDataManager( "items.xml" );
        this->itemDataManager->loadItems ();

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

void Isomot::offInviolability ()
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
        if ( mapManager->getActiveRoom() != 0 )
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

        this->mapManager->binEveryRoom();

        // bin taken bonuses
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
                key[ KEY_MINUS ] = 0;
        }

        if( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && key[ KEY_R ] )
        {
                playTuneForScenery( activeRoom->getScenery () );
                activeRoom = mapManager->rebuildRoom();

                key[ KEY_R ] = 0;
        }

        if( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && key[ KEY_J ] )
        {
                PlayerItem* activePlayer = activeRoom->getMediator()->getActivePlayer();
                PlayerItem* otherPlayer = 0;

                Room* roomWithInactivePlayer = this->mapManager->getRoomOfInactivePlayer();
                if ( roomWithInactivePlayer != 0 )
                {
                        otherPlayer = roomWithInactivePlayer->getMediator()->getActivePlayer();
                }

                if ( otherPlayer != 0 && roomWithInactivePlayer != activeRoom )
                {
                        std::cout << "join characters in active room \"" << activeRoom->getNameOfFileWithDataAboutRoom() << "\" via pure magic" << std::endl ;

                        std::string nameOfAnotherPlayer = otherPlayer->getLabel();

                        int playerX = activePlayer->getX();
                        int playerY = activePlayer->getY();
                        int playerZ = activePlayer->getZ() + 2 * LayerHeight;
                        Direction way = otherPlayer->getDirection();

                        PlayerItem* joinedPlayer = new PlayerItem(
                                this->itemDataManager->findItemByLabel( nameOfAnotherPlayer ),
                                playerX, playerY, playerZ, way
                        ) ;

                        std::string behavior = "still";
                        if ( nameOfAnotherPlayer == "head" ) behavior = "behavior of Head";
                        else if ( nameOfAnotherPlayer == "heels" ) behavior = "behavior of Heels";

                        joinedPlayer->assignBehavior( behavior, reinterpret_cast< void * >( this->itemDataManager ) );

                        joinedPlayer->fillWithData( gameManager );

                        activeRoom->addPlayerToRoom( joinedPlayer, true );
                        joinedPlayer->getBehavior()->changeActivityOfItem( BeginWayInTeletransport );

                        roomWithInactivePlayer->removePlayerFromRoom( otherPlayer, true );
                        this->mapManager->removeRoom( roomWithInactivePlayer );

                        activeRoom->getMediator()->setActivePlayer( joinedPlayer );
                }

                key[ KEY_J ] = 0;
        }

        if( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && key[ KEY_L ] )
        {
                if ( gameManager->countFreePlanets() < 5 )
                {
                        if ( activeRoom->getMediator()->findItemByLabel( "crown" ) == 0 )
                        {
                                ItemData* chapeauData = this->itemDataManager->findItemByLabel( "crown" );

                                int x = ( activeRoom->getBound( South ) - activeRoom->getBound( North ) + chapeauData->widthX ) >> 1 ;
                                int y = ( activeRoom->getBound( West ) - activeRoom->getBound( East ) + chapeauData->widthY ) >> 1 ;

                                FreeItem* chapeau = new FreeItem( chapeauData, x, y, 250, NoDirection );
                                chapeau->assignBehavior( "behavior of something special", chapeauData );
                                activeRoom->addFreeItem( chapeau );
                        }
                }
                else
                {
                        PlayerItem* activePlayer = activeRoom->getMediator()->getActivePlayer();
                        std::string nameOfPlayer = activePlayer->getLabel();
                        Direction whichWay = activePlayer->getDirection();
                        int teleportedX = 0;
                        int teleportedY = 95;
                        int teleportedZ = 240;

                        PlayerItem* teleportedPlayer = new PlayerItem(
                                this->itemDataManager->findItemByLabel( nameOfPlayer ),
                                teleportedX, teleportedY, teleportedZ,
                                whichWay
                        ) ;

                        std::string behaviorOfPlayer = "still";
                        if ( nameOfPlayer == "head" ) behaviorOfPlayer = "behavior of Head";
                        else if ( nameOfPlayer == "heels" ) behaviorOfPlayer = "behavior of Heels";
                        else if ( nameOfPlayer == "headoverheels" ) behaviorOfPlayer = "behavior of Head over Heels";

                        teleportedPlayer->assignBehavior( behaviorOfPlayer, reinterpret_cast< void * >( this->itemDataManager ) );

                        teleportedPlayer->fillWithData( gameManager );

                        std::string nameOfRoomNearFinal = "blacktooth/blacktooth83.xml";
                        Room* roomWithTeleportToFinalScene = this->mapManager->createRoomThenAddItToListOfRooms( nameOfRoomNearFinal );
                        roomWithTeleportToFinalScene->addPlayerToRoom( teleportedPlayer, true );
                        teleportedPlayer->getBehavior()->changeActivityOfItem( BeginWayInTeletransport );

                        activeRoom->removePlayerFromRoom( activePlayer, true );

                        this->mapManager->setActiveRoom( roomWithTeleportToFinalScene );
                        this->mapManager->removeRoom( activeRoom );

                        roomWithTeleportToFinalScene->activate();
                        roomWithTeleportToFinalScene->getMediator()->setActivePlayer( teleportedPlayer );

                        activeRoom = roomWithTeleportToFinalScene;
                }

                key[ KEY_L ] = 0;
        }

        // swap key changes character and possibly room
        if ( ! this->isEndRoom && InputManager::getInstance()->swap() )
        {
                activeRoom->getMediator()->getActivePlayer()->wait(); // stop active player
                if ( activeRoom->getMediator()->getActivePlayer()->getBehavior()->getActivityOfItem() == Wait )
                {
                        // swap in the same room or between different rooms
                        if ( ! activeRoom->swapPlayersInRoom( this->itemDataManager ) )
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
                Direction exit = activeRoom->getWayOfExit();

                if ( exit == Restart )
                {
                        PlayerItem* player = activeRoom->getMediator()->getActivePlayer();

                        if ( player->getLives() != 0 || ( player->getLabel() == "headoverheels" && player->getLives() == 0 ) )
                        {
                                activeRoom = mapManager->rebuildRoom();
                        }
                        else
                        {
                                if ( ! activeRoom->continueWithAlivePlayer( ) )
                                {
                                        activeRoom = mapManager->removeRoomAndSwap ();
                                }
                        }
                }
                else
                {
                        activeRoom = mapManager->changeRoom( exit );

                        if ( activeRoom != 0 )
                        {
                                playTuneForScenery( activeRoom->getScenery () );
                        }
                }
        }

        // draw active room, if there’s any
        if ( activeRoom != 0 )
        {
                blit (
                        activeRoom->getPicture(), this->view,
                        activeRoom->getCamera()->getDeltaX(), activeRoom->getCamera()->getDeltaY(),
                        0, 0,
                        activeRoom->getPicture()->w, activeRoom->getPicture()->h
                );

                if ( GameManager::getInstance()->areLivesInexhaustible () )
                {
                        textout_ex( this->view, font, "VIDAS INFINITAS", 18, 10, makecol( 255, 255, 255 ), -1 );
                        textout_ex( this->view, font, "INFINITE LIVES", this->view->w - 128, 10, makecol( 255, 255, 255 ), -1 );
                }

                if ( GameManager::getInstance()->isImmuneToCollisionsWithMortalItems () )
                {
                        textout_ex( this->view, font, "INVIOLABILITY", ( this->view->w >> 1 ) - 50, 10, makecol( 255, 255, 255 ), -1 );
                }

                // la sala final es muy especial
                if ( activeRoom->getNameOfFileWithDataAboutRoom().compare( "blacktooth/blacktooth88.xml" ) == 0 )
                {
                        this->updateEndRoom();
                }
        }
        // there’s no active room, game over
        else
        {
                destroy_bitmap( this->view );
                this->view = 0;
        }

        return this->view;
}

void Isomot::playTuneForScenery ( const std::string& scenery )
{
        if ( scenery != "" )
        {
                SoundManager::getInstance()->playOgg ( "music/" + scenery + ".ogg", /* loop */ false );
        }
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
                if ( gameManager->isFreePlanet( "safari" ) )
                {
                        freeItem = new FreeItem( this->itemDataManager->findItemByLabel( "crown" ), 66, 75, Top, NoDirection );
                        activeRoom->addFreeItem( freeItem );
                        crowns++;
                }
                // la corona de Egyptus
                if ( gameManager->isFreePlanet( "egyptus" ) )
                {
                        freeItem = new FreeItem( this->itemDataManager->findItemByLabel( "crown" ), 66, 59, Top, NoDirection );
                        activeRoom->addFreeItem( freeItem );
                        crowns++;
                }
                // la corona de Penitentiary
                if ( gameManager->isFreePlanet( "penitentiary" ) )
                {
                        freeItem = new FreeItem( this->itemDataManager->findItemByLabel( "crown" ), 65, 107, Top, NoDirection );
                        activeRoom->addFreeItem( freeItem );
                        crowns++;
                }
                // la corona de Byblos
                if ( gameManager->isFreePlanet( "byblos" ) )
                {
                        freeItem = new FreeItem( this->itemDataManager->findItemByLabel( "crown" ), 65, 123, Top, NoDirection );
                        activeRoom->addFreeItem( freeItem );
                        crowns++;
                }
                // la corona de Blacktooth
                if ( gameManager->isFreePlanet( "blacktooth" ) )
                {
                        freeItem = new FreeItem( this->itemDataManager->findItemByLabel( "crown" ), 65, 91, Top, NoDirection );
                        activeRoom->addFreeItem( freeItem );
                        crowns++;
                }

                if ( crowns == 5 )
                {
                        // all five crowns are taken, show the greeting screen
                        gameManager->success();
                }
                else
                {
                        // if not, just go to the summary screen
                        gameManager->arriveFreedom();
                }

                // final room is done
                this->isEndRoom = true;
        }
        else
        {
                std::string scenery = /* "finale" */ "begin" ;
                SoundManager::getInstance()->playOgg ( "music/" + scenery + ".ogg", /* loop */ false );

                /// sleep( 2340 );

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

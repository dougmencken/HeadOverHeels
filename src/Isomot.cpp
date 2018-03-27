
#include "Isomot.hpp"
#include "Color.hpp"
#include "GameManager.hpp"
#include "GuiManager.hpp"
#include "ItemDataManager.hpp"
#include "MapManager.hpp"
#include "BonusManager.hpp"
#include "InputManager.hpp"
#include "SoundManager.hpp"
#include "Room.hpp"
#include "Mediator.hpp"
#include "GridItem.hpp"
#include "Camera.hpp"
#include "PlayerItem.hpp"
#include "Behavior.hpp"


namespace isomot
{

Isomot::Isomot( ) :
        view( nilPointer ),
        itemDataManager( nilPointer ),
        mapManager( nilPointer ),
        finalRoomTimer( nilPointer ),
        finalRoomBuilt( false )
{

}

Isomot::~Isomot( )
{
        delete this->mapManager;
        delete this->itemDataManager;
        delete this->finalRoomTimer;

        allegro::destroyBitmap( this->view );
}

void Isomot::beginNewGame ()
{
        prepare() ;

        offRecording() ;
        offVidasInfinitas() ;
        offInviolability() ;

        finalRoomBuilt = false;
        if ( finalRoomTimer != nilPointer ) finalRoomTimer->stop();

        // initial rooms
        mapManager->beginNewGame( "blacktooth1head.xml", "blacktooth23heels.xml" );
        //mapManager->beginNewGame( "blacktooth85.xml", "blacktooth23heels.xml" );

        // go to first room
        mapManager->getActiveRoom()->activate ();

        std::cout << "play new game" << std::endl ;
        ////SoundManager::getInstance()->playOgg ( "music/begin.ogg", /* loop */ false );
}

void Isomot::continueSavedGame ( const sgxml::players::player_sequence& playerSequence )
{
        offRecording() ;
        offVidasInfinitas() ;
        offInviolability() ;

        finalRoomBuilt = false;
        if ( finalRoomTimer != nilPointer ) finalRoomTimer->stop();

        for ( sgxml::players::player_const_iterator i = playerSequence.begin (); i != playerSequence.end (); ++i )
        {
                this->mapManager->beginOldGameWithCharacter( *i );
        }

        std::cout << "continue previous game" << std::endl ;
        /// no begin.ogg here
}

void Isomot::prepare ()
{
        // image where the isometric view will be drawn

        if ( this->view == nilPointer )
        {
                this->view = create_bitmap_ex( 32, ScreenWidth, ScreenHeight );
        }

        // data for elements of the game

        if ( this->itemDataManager != nilPointer )
        {
                // set of graphics may change between games
                // new ItemDataManager is needed to refresh pictures of items
                delete this->itemDataManager;
                this->itemDataManager = nilPointer ;
        }

        this->itemDataManager = new ItemDataManager( "items.xml" );
        this->itemDataManager->loadItems ();

        // data for map

        if ( this->mapManager == nilPointer )
        {
                this->mapManager = new MapManager( this, "map.xml" );
                this->mapManager->loadMap ();
        }
}

void Isomot::offRecording ()
{
        if ( GameManager::getInstance()->recordingCaptures () )
        {
                GameManager::getInstance()->toggleRecordingCaptures ();
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
        if ( mapManager->getActiveRoom() != nilPointer )
        {
                mapManager->getActiveRoom()->deactivate();
        }
}

void Isomot::resume ()
{
        if ( mapManager->getActiveRoom() != nilPointer )
        {
                mapManager->getActiveRoom()->activate();
        }
}

void Isomot::reset()
{
        finalRoomBuilt = false;
        if ( finalRoomTimer != nilPointer ) finalRoomTimer->stop();

        // bin isometric view
        allegro::destroyBitmap( this->view );

        this->mapManager->binEveryRoom();

        // bin taken bonuses
        BonusManager::getInstance()->reset();
}

BITMAP* Isomot::update()
{
        Room* activeRoom = mapManager->getActiveRoom();
        GameManager* gameManager = GameManager::getInstance();

        if ( key[ KEY_PRTSCR ] )
        {
                gameManager->toggleRecordingCaptures ();
                key[ KEY_PRTSCR ] = 0;
        }

        if( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && key[ KEY_F ] )
        {
                gui::GuiManager::getInstance()->toggleFullScreenVideo ();
                key[ KEY_F ] = 0;
        }

        if( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && key[ KEY_EQUALS ] )
        {
                gameManager->addLives( activeRoom->getMediator()->getLabelOfActiveCharacter(), 1 );
                key[ KEY_EQUALS ] = 0;
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

        if( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && key[ KEY_T ] )
        {
                GameManager::getInstance()->togglePlayMelodyOfScenery ();

                if ( GameManager::getInstance()->playMelodyOfScenery() )
                {
                        std::cout << "room tunes on" << std::endl ;
                        playTuneForScenery( activeRoom->getScenery () );
                } else
                        std::cout << "room tunes off" << std::endl ;

                key[ KEY_T ] = 0;
        }

        if( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && key[ KEY_S ] )
        {
                gameManager->eatFish ( activeRoom->getMediator()->getActiveCharacter(), activeRoom );
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
                unsigned int howManyBars = activeRoom->removeBars ();

                if ( howManyBars > 0 )
                        std::cout << howManyBars << " bars are gone" << std::endl ;

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
                PlayerItem* activePlayer = activeRoom->getMediator()->getActiveCharacter();
                PlayerItem* otherPlayer = nilPointer;

                Room* roomWithInactivePlayer = this->mapManager->getRoomOfInactivePlayer();
                if ( roomWithInactivePlayer != nilPointer )
                {
                        otherPlayer = roomWithInactivePlayer->getMediator()->getActiveCharacter();
                }

                if ( otherPlayer != nilPointer && roomWithInactivePlayer != activeRoom )
                {
                        std::cout << "join characters in active room \"" << activeRoom->getNameOfFileWithDataAboutRoom() << "\" via pure magic" << std::endl ;

                        std::string nameOfAnotherPlayer = otherPlayer->getLabel();

                        int playerX = activePlayer->getX();
                        int playerY = activePlayer->getY();
                        int playerZ = activePlayer->getZ() + 2 * LayerHeight;
                        Way way = otherPlayer->getOrientation();

                        PlayerItem* joinedPlayer = new PlayerItem(
                                this->itemDataManager->findDataByLabel( nameOfAnotherPlayer ),
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
                }

                key[ KEY_J ] = 0;
        }

        if( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && key[ KEY_L ] )
        {
                if ( gameManager->countFreePlanets() < 5 )
                {
                        if ( activeRoom->getMediator()->findItemByLabel( "crown" ) == nilPointer )
                        {
                                ItemData* chapeauData = this->itemDataManager->findDataByLabel( "crown" );

                                int x = ( activeRoom->getLimitAt( "south" ) - activeRoom->getLimitAt( "north" ) + chapeauData->getWidthX() ) >> 1 ;
                                int y = ( activeRoom->getLimitAt( "west" ) - activeRoom->getLimitAt( "east" ) + chapeauData->getWidthY() ) >> 1 ;

                                FreeItem* chapeau = new FreeItem( chapeauData, x, y, 250, Nowhere );
                                chapeau->assignBehavior( "behavior of something special", chapeauData );
                                activeRoom->addFreeItem( chapeau );
                        }
                }
                else
                {
                        PlayerItem* activePlayer = activeRoom->getMediator()->getActiveCharacter();
                        std::string nameOfPlayer = activePlayer->getLabel();
                        Way whichWay = activePlayer->getOrientation();
                        int teleportedX = 0;
                        int teleportedY = 95;
                        int teleportedZ = 240;

                        PlayerItem* teleportedPlayer = new PlayerItem(
                                this->itemDataManager->findDataByLabel( nameOfPlayer ),
                                teleportedX, teleportedY, teleportedZ,
                                whichWay
                        ) ;

                        std::string behaviorOfPlayer = "still";
                        if ( nameOfPlayer == "head" ) behaviorOfPlayer = "behavior of Head";
                        else if ( nameOfPlayer == "heels" ) behaviorOfPlayer = "behavior of Heels";
                        else if ( nameOfPlayer == "headoverheels" ) behaviorOfPlayer = "behavior of Head over Heels";

                        teleportedPlayer->assignBehavior( behaviorOfPlayer, reinterpret_cast< void * >( this->itemDataManager ) );

                        teleportedPlayer->fillWithData( gameManager );

                        std::string nameOfRoomNearFinal = "blacktooth83tofreedom.xml";
                        Room* roomWithTeleportToFinalScene = this->mapManager->createRoomThenAddItToListOfRooms( nameOfRoomNearFinal, true );
                        roomWithTeleportToFinalScene->addPlayerToRoom( teleportedPlayer, true );
                        teleportedPlayer->getBehavior()->changeActivityOfItem( BeginWayInTeletransport );

                        activeRoom->removePlayerFromRoom( activePlayer, true );

                        this->mapManager->setActiveRoom( roomWithTeleportToFinalScene );
                        this->mapManager->removeRoom( activeRoom );

                        roomWithTeleportToFinalScene->activate();
                        activeRoom = roomWithTeleportToFinalScene;
                }

                key[ KEY_L ] = 0;
        }

        // swap key changes character and possibly room
        if ( ! this->finalRoomBuilt && InputManager::getInstance()->swap() )
        {
                activeRoom->getMediator()->getActiveCharacter()->wait(); // stop active player

                if ( activeRoom->getMediator()->getActiveCharacter()->getBehavior()->getActivityOfItem() == Wait )
                {
                        // swap in the same room or between different rooms
                        if ( ! activeRoom->swapCharactersInRoom( this->itemDataManager ) )
                        {
                                activeRoom = mapManager->swapRoom();
                        }
                }

                InputManager::getInstance()->noRepeat( "swap" );
        }

        if ( activeRoom->getWayOfExit() == "no exit" )
        {
                activeRoom->drawRoom();
        }
        // there’s a change of room or active player lost its life
        else
        {
                if ( activeRoom->getWayOfExit() == "rebuild room" )
                {
                        PlayerItem* player = activeRoom->getMediator()->getActiveCharacter();

                        if ( player->getLives() != 0 || player->getLabel() == "headoverheels" )
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
                        Room* newRoom = mapManager->changeRoom( activeRoom->getWayOfExit() );

                        if ( newRoom != nilPointer && newRoom != activeRoom )
                        {
                                playTuneForScenery( newRoom->getScenery () );
                        }

                        activeRoom = newRoom;
                }
        }

        // draw active room, if there’s any
        if ( activeRoom != nilPointer )
        {
                blit (
                        activeRoom->getWhereToDraw(), this->view,
                        activeRoom->getCamera()->getDeltaX(), activeRoom->getCamera()->getDeltaY(),
                        0, 0,
                        activeRoom->getWhereToDraw()->w, activeRoom->getWhereToDraw()->h
                );

                std::string roomFile = activeRoom->getNameOfFileWithDataAboutRoom() ;
                const char* fromLastSlash = std::strrchr( roomFile.c_str (), pathSeparator[ 0 ] );
                if ( fromLastSlash != nilPointer )
                        roomFile = std::string( fromLastSlash + 1 );

                int whiteColor = Color::whiteColor()->toAllegroColor() ;

        if ( ! GameManager::getInstance()->hasBackgroundPicture () )
        {
                // show information about room

                int tilesX = activeRoom->getTilesX();
                int tilesY = activeRoom->getTilesY();

                std::ostringstream roomTiles;
                roomTiles << tilesX << "x" << tilesY;

                MapRoomData* connections = this->getMapManager()->findRoomData( activeRoom );
                Way wayToNextRoom( "nowhere" );

             /* std::string roomOnNorth = connections->findConnectedRoom( Way( "north" ), &wayToNextRoom );
                std::string roomOnSouth = connections->findConnectedRoom( Way( "south" ), &wayToNextRoom );
                std::string roomOnEast = connections->findConnectedRoom( Way( "east" ), &wayToNextRoom );
                std::string roomOnWest = connections->findConnectedRoom( Way( "west" ), &wayToNextRoom ); */

                std::string roomAbove = connections->findConnectedRoom( Way( "up" ), &wayToNextRoom );
                std::string roomBelow = connections->findConnectedRoom( Way( "down" ), &wayToNextRoom );
                std::string roomToTeleport = connections->findConnectedRoom( Way( "via teleport" ), &wayToNextRoom );
                std::string roomToTeleportToo = connections->findConnectedRoom( Way( "via second teleport" ), &wayToNextRoom );

                int gray50Color = Color::gray50Color()->toAllegroColor() ;

                textout_ex( this->view, font, roomFile.c_str (), 12, 8, whiteColor, -1 );
                textout_ex( this->view, font, roomTiles.str().c_str (), 12, 20, whiteColor, -1 );

                // draw miniature of room

                const unsigned int sizeOfTileForMap = 3 ;

                const unsigned int leftXmap = 24;
                const unsigned int topYmap = roomAbove.empty() ? 28 : 24 + ( 3 * sizeOfTileForMap );

                const unsigned int iniPosX = leftXmap + ( tilesY * ( sizeOfTileForMap << 1 ) );
                const unsigned int iniPosY = topYmap + ( sizeOfTileForMap << 1 );

                unsigned int posX = iniPosX ;
                unsigned int posY = iniPosY ;

                int doorXmid = tilesX >> 1;
                int doorYmid = tilesY >> 1;

                Door* eastDoor = activeRoom->getDoorAt( "east" );
                Door* southDoor = activeRoom->getDoorAt( "south" );
                Door* westDoor = activeRoom->getDoorAt( "west" );
                Door* northDoor = activeRoom->getDoorAt( "north" );

                if ( eastDoor != nilPointer && westDoor == nilPointer ) doorYmid ++;
                if ( northDoor != nilPointer && southDoor == nilPointer ) doorXmid ++;

                for ( int tile = 0 ; tile < tilesX ; tile ++ )
                {
                        if ( eastDoor != nilPointer && eastDoor->getCellX() + 1 != doorXmid )
                        {
                                doorXmid = eastDoor->getCellX() + 1;
                        }

                        if ( ! ( ( tile == doorXmid || tile + 1 == doorXmid )
                                && eastDoor != nilPointer ) )
                        {
                                for ( unsigned int pix = 0 ; pix < sizeOfTileForMap ; pix ++ )
                                {
                                        putpixel( this->view, posX++, posY, whiteColor );
                                        putpixel( this->view, posX++, posY++, whiteColor );
                                }
                        }
                        else
                        {
                                if ( eastDoor != nilPointer && ( tile + 1 == doorXmid ) )
                                {
                                        if ( tile > 0 /* not first tile */ )
                                                putpixel( this->view, posX, posY, gray50Color );
                                        else
                                                putpixel( this->view, posX + 2, posY + 1, gray50Color );
                                }

                                posX += sizeOfTileForMap << 1;
                                posY += sizeOfTileForMap;

                                if ( eastDoor != nilPointer && ( tile == doorXmid ) )
                                {
                                        if ( tile + 1 < tilesX /* not last tile */ )
                                                putpixel( this->view, posX - 1, posY - 1, gray50Color );
                                        else
                                                putpixel( this->view, posX - 3, posY - 2, gray50Color );
                                }
                        }
                }

                posX--; posY--;

                for ( int tile = 0 ; tile < tilesY ; tile ++ )
                {
                        if ( southDoor != nilPointer && southDoor->getCellY() + 1 != doorYmid )
                        {
                                doorYmid = southDoor->getCellY() + 1;
                        }

                        if ( ! ( ( tile == doorYmid || tile + 1 == doorYmid )
                                && southDoor != nilPointer ) )
                        {
                                for ( unsigned int pix = 0 ; pix < sizeOfTileForMap ; pix ++ )
                                {
                                        putpixel( this->view, posX--, posY, whiteColor );
                                        putpixel( this->view, posX--, posY++, whiteColor );
                                }
                        }
                        else
                        {
                                if ( southDoor != nilPointer && ( tile + 1 == doorYmid ) )
                                {
                                        if ( tile > 0 /* not first tile */ )
                                                putpixel( this->view, posX, posY, gray50Color );
                                        else
                                                putpixel( this->view, posX - 2, posY + 1, gray50Color );
                                }

                                posX -= sizeOfTileForMap << 1;
                                posY += sizeOfTileForMap;

                                if ( southDoor != nilPointer && ( tile == doorYmid ) )
                                {
                                        if ( tile + 1 < tilesY /* not last tile */ )
                                                putpixel( this->view, posX + 1, posY - 1, gray50Color );
                                        else
                                                putpixel( this->view, posX + 3, posY - 2, gray50Color  );
                                }
                        }
                }

                posX = iniPosX + 1 ;
                posY = iniPosY ;

                for ( int tile = 0 ; tile < tilesY ; tile ++ )
                {
                        if ( northDoor != nilPointer && northDoor->getCellY() + 1 != doorYmid )
                        {
                                doorYmid = northDoor->getCellY() + 1;
                        }

                        if ( ! ( ( tile == doorYmid || tile + 1 == doorYmid )
                                && northDoor != nilPointer ) )
                        {
                                for ( unsigned int pix = 0 ; pix < sizeOfTileForMap ; pix ++ )
                                {
                                        putpixel( this->view, posX--, posY, whiteColor );
                                        putpixel( this->view, posX--, posY++, whiteColor );
                                }
                        }
                        else
                        {
                                if ( northDoor != nilPointer && ( tile + 1 == doorYmid ) )
                                {
                                        if ( tile > 0 /* not first tile */ )
                                                putpixel( this->view, posX, posY, gray50Color );
                                        else
                                                putpixel( this->view, posX - 2, posY + 1, gray50Color );
                                }

                                posX -= sizeOfTileForMap << 1;
                                posY += sizeOfTileForMap;

                                if ( northDoor != nilPointer && ( tile == doorYmid ) )
                                {
                                        if ( tile + 1 < tilesY /* not last tile */ )
                                                putpixel( this->view, posX + 1, posY - 1, gray50Color );
                                        else
                                                putpixel( this->view, posX + 3, posY - 2, gray50Color );
                                }
                        }
                }

                posX++; posY--;

                for ( int tile = 0 ; tile < tilesX ; tile ++ )
                {
                        if ( westDoor != nilPointer && westDoor->getCellX() + 1 != doorXmid )
                        {
                                doorXmid = westDoor->getCellX() + 1;
                        }

                        if ( ! ( ( tile == doorXmid || tile + 1 == doorXmid )
                                && westDoor != nilPointer ) )
                        {
                                for ( unsigned int pix = 0 ; pix < sizeOfTileForMap ; pix ++ )
                                {
                                        putpixel( this->view, posX++, posY, whiteColor );
                                        putpixel( this->view, posX++, posY++, whiteColor );
                                }
                        }
                        else
                        {
                                if ( westDoor != nilPointer && ( tile + 1 == doorXmid ) )
                                {
                                        if ( tile > 0 /* not first tile */ )
                                                putpixel( this->view, posX, posY, gray50Color );
                                        else
                                                putpixel( this->view, posX + 2, posY + 1, gray50Color );
                                }

                                posX += sizeOfTileForMap << 1;
                                posY += sizeOfTileForMap;

                                if ( westDoor != nilPointer && ( tile == doorXmid ) )
                                {
                                        if ( tile + 1 < tilesX /* not last tile */ )
                                                putpixel( this->view, posX - 1, posY - 1, gray50Color );
                                        else
                                                putpixel( this->view, posX - 3, posY - 2, gray50Color );
                                }
                        }
                }

                // show teleports on miniature

                std::vector< std::list< GridItem* > > gridItemsInRoom = activeRoom->getMediator()->getGridItems();

                for ( unsigned int column = 0; column < gridItemsInRoom.size(); column ++ )
                {
                        std::list < GridItem * > columnOfItems = gridItemsInRoom[ column ];

                        for ( std::list< GridItem * >::const_iterator gi = columnOfItems.begin (); gi != columnOfItems.end (); ++ gi )
                        {
                                if ( ( *gi )->getLabel() == "teleport" && ! roomToTeleport.empty() )
                                {
                                        fillIsoTile( this->view, iniPosX, iniPosY, ( *gi )->getCellX(), ( *gi )->getCellY(), sizeOfTileForMap, Color::yellowColor() );
                                }
                                else if ( ( *gi )->getLabel() == "teleport-too" && ! roomToTeleportToo.empty() )
                                {
                                        fillIsoTile( this->view, iniPosX, iniPosY, ( *gi )->getCellX(), ( *gi )->getCellY(), sizeOfTileForMap, Color::magentaColor() );
                                }
                        }
                }

                // show when there’s a room above or below

                if ( ! roomAbove.empty() || ! roomBelow.empty() )
                {
                        int miniatureMidX = leftXmap + ( tilesY + tilesX ) * sizeOfTileForMap;
                        int abovePointerY = iniPosY - 2;
                        int belowPointerY = iniPosY + ( tilesY + tilesX ) * sizeOfTileForMap;
                        abovePointerY -= sizeOfTileForMap << 1;
                        belowPointerY += sizeOfTileForMap << 1;

                        ///if ( ! roomAbove.empty() ) putpixel( this->view, miniatureMidX, abovePointerY, Color::blueColor()->toAllegroColor() );
                        ///if ( ! roomBelow.empty() ) putpixel( this->view, miniatureMidX, belowPointerY, Color::blueColor()->toAllegroColor() );

                        int greenColor = Color::greenColor()->toAllegroColor() ;

                        const unsigned int linesEven = ( ( sizeOfTileForMap + 1 ) >> 1 ) << 1;

                        if ( ! roomAbove.empty() )
                        {
                                // draw middle line
                                line( this->view, miniatureMidX, abovePointerY - linesEven + 1, miniatureMidX, abovePointerY - ( linesEven << 1 ), greenColor );

                                int pos = 0;
                                for ( unsigned int off = linesEven ; off > 0 ; off -- , pos ++ )
                                {
                                        line( this->view, miniatureMidX - off, abovePointerY - pos, miniatureMidX - off, abovePointerY - pos - linesEven, greenColor );
                                        line( this->view, miniatureMidX + off, abovePointerY - pos, miniatureMidX + off, abovePointerY - pos - linesEven, greenColor );
                                }
                        }

                        if ( ! roomBelow.empty() )
                        {
                                // draw middle line
                                line( this->view, miniatureMidX, belowPointerY + linesEven - 1, miniatureMidX, belowPointerY + ( linesEven << 1 ), greenColor );

                                int pos = 0;
                                for ( unsigned int off = linesEven ; off > 0 ; off -- , pos ++ )
                                {
                                        line( this->view, miniatureMidX - off, belowPointerY + pos, miniatureMidX - off, belowPointerY + pos + linesEven, greenColor );
                                        line( this->view, miniatureMidX + off, belowPointerY + pos, miniatureMidX + off, belowPointerY + pos + linesEven, greenColor );
                                }
                        }
                }
        }

                // cheats

                if ( GameManager::getInstance()->areLivesInexhaustible () )
                {
                        ////textout_ex( this->view, font, "VIDAS INFINITAS", 18, 10, whiteColor, -1 );
                        textout_ex( this->view, font, "INFINITE LIVES", this->view->w - 128, 10, whiteColor, -1 );
                }

                if ( GameManager::getInstance()->isImmuneToCollisionsWithMortalItems () )
                {
                        textout_ex( this->view, font, "INVIOLABILITY", ( this->view->w >> 1 ) - 50, 10, whiteColor, -1 );
                }

                // la sala final es muy especial
                if ( roomFile == "finalroom.xml" )
                {
                        this->updateFinalRoom();
                }
        }
        // there’s no active room
        else
        {
                std::cout << "no room, game over" << std::endl ;
                allegro::destroyBitmap( this->view );
        }

        return this->view;
}

void Isomot::playTuneForScenery ( const std::string& scenery )
{
        if ( scenery != "" && GameManager::getInstance()->playMelodyOfScenery () )
        {
                SoundManager::getInstance()->playOgg ( "music/" + scenery + ".ogg", /* loop */ false );
        } else
                std::cout << "( ignore melody for scenery \"" << scenery << "\" )" << std::endl ;
}

void Isomot::updateFinalRoom()
{
        Room* activeRoom = mapManager->getActiveRoom();
        assert( activeRoom != nilPointer );
        Mediator* mediator = activeRoom->getMediator();
        assert( mediator != nilPointer );

        if ( ! this->finalRoomBuilt )
        {
                mediator->endUpdate();

                if ( mediator->getActiveCharacter() != nilPointer )
                {
                        std::string arrivedCharacter = mediator->getActiveCharacter()->getOriginalLabel();

                        activeRoom->removePlayerFromRoom( mediator->getActiveCharacter(), true );

                        std::cout << "character \"" << arrivedCharacter << "\" arrived to final room" << std::endl ;

                        ItemData* dataOfArrived = this->itemDataManager->findDataByLabel( arrivedCharacter );

                        if ( dataOfArrived != nilPointer )
                        {
                                FreeItem* character = new FreeItem( dataOfArrived, 66, 92, Top, South );
                                activeRoom->addFreeItem( character );
                        }
                }

                // crea las coronas recuperadas

                GameManager* gameManager = GameManager::getInstance();
                unsigned int crowns = 0;

                ItemData* dataForChapeau = this->itemDataManager->findDataByLabel( "crown" );

                // la corona de Safari
                if ( gameManager->isFreePlanet( "safari" ) )
                {
                        FreeItem* chapeau = new FreeItem( dataForChapeau, 66, 75, Top, Nowhere );
                        activeRoom->addFreeItem( chapeau );
                        crowns++;
                }
                // la corona de Egyptus
                if ( gameManager->isFreePlanet( "egyptus" ) )
                {
                        FreeItem* chapeau = new FreeItem( dataForChapeau, 66, 59, Top, Nowhere );
                        activeRoom->addFreeItem( chapeau );
                        crowns++;
                }
                // la corona de Penitentiary
                if ( gameManager->isFreePlanet( "penitentiary" ) )
                {
                        FreeItem* chapeau = new FreeItem( dataForChapeau, 65, 107, Top, Nowhere );
                        activeRoom->addFreeItem( chapeau );
                        crowns++;
                }
                // la corona de Byblos
                if ( gameManager->isFreePlanet( "byblos" ) )
                {
                        FreeItem* chapeau = new FreeItem( dataForChapeau, 65, 123, Top, Nowhere );
                        activeRoom->addFreeItem( chapeau );
                        crowns++;
                }
                // la corona de Blacktooth
                if ( gameManager->isFreePlanet( "blacktooth" ) )
                {
                        FreeItem* chapeau = new FreeItem( dataForChapeau, 65, 91, Top, Nowhere );
                        activeRoom->addFreeItem( chapeau );
                        crowns++;
                }

                mediator->beginUpdate();

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

                if ( finalRoomTimer == nilPointer ) finalRoomTimer = new Timer() ;
                finalRoomTimer->reset();
                finalRoomTimer->go();

                this->finalRoomBuilt = true;
                std::cout << "final room is okay" << std::endl ;

                SoundManager::getInstance()->playOgg ( "music/freedom.ogg", /* loop */ true );
        }
        else
        {
                if ( finalRoomTimer->getValue() > 4 /* each 4 seconds */ )
                {
                        FreeItem* finalBall = new FreeItem( this->itemDataManager->findDataByLabel( "ball" ), 146, 93, LayerHeight, Nowhere );
                        finalBall->assignBehavior( "behaivor of final ball", this->itemDataManager->findDataByLabel( "bubbles" ) );
                        activeRoom->addFreeItem( finalBall );

                        finalRoomTimer->reset();
                }
        }
}

/* static */
void Isomot::fillIsoTile( BITMAP* where, int x0, int y0, int tileX, int tileY, unsigned int sizeOfTile, const Color* color )
{
        for ( unsigned int piw = 0 ; piw < sizeOfTile ; piw ++ )
        {
                int x = x0 + ( tileX - tileY ) * ( sizeOfTile << 1 ) - ( piw << 1 ) ;
                int y = y0 + ( tileY + tileX ) * sizeOfTile + piw ;

                for ( unsigned int pix = 0 ; pix < sizeOfTile ; pix ++ )
                {
                        putpixel( where, x++, y, color->toAllegroColor() );
                        putpixel( where, x++, y++, color->toAllegroColor() );
                }
        }
}

}

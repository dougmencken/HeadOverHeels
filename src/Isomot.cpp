
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
        mapManager( nilPointer ),
        finalRoomTimer( nilPointer ),
        finalRoomBuilt( false )
{

}

Isomot::~Isomot( )
{
        delete this->mapManager;
        delete this->finalRoomTimer;

        delete this->view ;
}

void Isomot::beginNewGame ()
{
        prepare() ;

        offRecording() ;
        offVidasInfinitas() ;
        offInviolability() ;
        GameManager::getInstance()->setCharactersFly( false );

        finalRoomBuilt = false;
        if ( finalRoomTimer != nilPointer ) finalRoomTimer->stop();

        assert( mapManager != nilPointer );
        mapManager->beginNewGame( GameManager::getInstance()->getHeadRoom(), GameManager::getInstance()->getHeelsRoom() );

        assert( mapManager->getActiveRoom() != nilPointer );
        mapManager->getActiveRoom()->activate ();

        std::cout << "play new game" << std::endl ;
        SoundManager::getInstance()->playOgg ( "music/begin.ogg", /* loop */ false );
}

void Isomot::continueSavedGame ( tinyxml2::XMLElement* characters )
{
        offRecording() ;
        offVidasInfinitas() ;
        offInviolability() ;
        GameManager::getInstance()->setCharactersFly( false );

        finalRoomBuilt = false;
        if ( finalRoomTimer != nilPointer ) finalRoomTimer->stop();

        if ( characters != nilPointer )
        {
                for ( tinyxml2::XMLElement* player = characters->FirstChildElement( "player" ) ;
                                player != nilPointer ;
                                player = player->NextSiblingElement( "player" ) )
                {
                        std::string label = player->Attribute( "label" );

                        bool isActiveCharacter = false;
                        tinyxml2::XMLElement* active = player->FirstChildElement( "active" );
                        if ( active != nilPointer && std::string( active->FirstChild()->ToText()->Value() ) == "true" )
                                isActiveCharacter = true;

                        std::string room = "no room";
                        tinyxml2::XMLElement* roomFile = player->FirstChildElement( "room" );
                        if ( roomFile != nilPointer )
                                room = roomFile->FirstChild()->ToText()->Value();

                        tinyxml2::XMLElement* xElement = player->FirstChildElement( "x" );
                        tinyxml2::XMLElement* yElement = player->FirstChildElement( "y" );
                        tinyxml2::XMLElement* zElement = player->FirstChildElement( "z" );
                        int x = 0;
                        int y = 0;
                        int z = 0;
                        if ( xElement != nilPointer )
                                x = std::atoi( xElement->FirstChild()->ToText()->Value() );
                        if ( yElement != nilPointer )
                                y = std::atoi( yElement->FirstChild()->ToText()->Value() );
                        if ( zElement != nilPointer )
                                z = std::atoi( zElement->FirstChild()->ToText()->Value() );

                        unsigned int howManyLives = 0;
                        tinyxml2::XMLElement* lives = player->FirstChildElement( "lives" );
                        if ( lives != nilPointer )
                                howManyLives = std::atoi( lives->FirstChild()->ToText()->Value() );

                        std::string directionString = "nowhere";
                        tinyxml2::XMLElement* direction = player->FirstChildElement( "direction" );
                        if ( direction != nilPointer )
                                directionString = direction->FirstChild()->ToText()->Value() ;

                        std::string entryString = "just wait";
                        tinyxml2::XMLElement* entry = player->FirstChildElement( "entry" );
                        if ( entry != nilPointer )
                                entryString = entry->FirstChild()->ToText()->Value() ;

                        bool hasHorn = false;
                        tinyxml2::XMLElement* horn = player->FirstChildElement( "hasHorn" );
                        if ( horn != nilPointer && std::string( horn->FirstChild()->ToText()->Value() ) == "yes" )
                                hasHorn = true;

                        bool hasHandbag = false;
                        tinyxml2::XMLElement* handbag = player->FirstChildElement( "hasHandbag" );
                        if ( handbag != nilPointer && std::string( handbag->FirstChild()->ToText()->Value() ) == "yes" )
                                hasHandbag = true;

                        unsigned int howManyDoughnuts = 0;
                        tinyxml2::XMLElement* donuts = player->FirstChildElement( "donuts" );
                        if ( donuts != nilPointer )
                                howManyDoughnuts = std::atoi( donuts->FirstChild()->ToText()->Value() );

                        if ( label == "headoverheels" )
                        {
                                // formula for lives of composite character from lives of simple characters is
                                // lives H & H = 100 * lives Head + lives Heels

                                unsigned char headLives = 0;
                                unsigned char heelsLives = 0;

                                while ( howManyLives > 100 )
                                {
                                        howManyLives -= 100;
                                        headLives++;
                                }

                                heelsLives = static_cast< unsigned char >( howManyLives );

                                GameManager::getInstance()->setHeadLives( headLives );
                                GameManager::getInstance()->setHorn( hasHorn );
                                GameManager::getInstance()->setDonuts( howManyDoughnuts );
                                GameManager::getInstance()->setHeelsLives( heelsLives );
                                GameManager::getInstance()->setHandbag( hasHandbag );
                                GameManager::getInstance()->setHighSpeed( 0 );
                                GameManager::getInstance()->setHighJumps( 0 );
                                GameManager::getInstance()->setHeadShield( 0 );
                                GameManager::getInstance()->setHeelsShield( 0 );
                        }
                        else if ( label == "head" )
                        {
                                GameManager::getInstance()->setHeadLives( howManyLives );
                                GameManager::getInstance()->setHorn( hasHorn );
                                GameManager::getInstance()->setDonuts( howManyDoughnuts );
                                GameManager::getInstance()->setHighSpeed( 0 );
                                GameManager::getInstance()->setHeadShield( 0 );
                        }
                        else if ( label == "heels" )
                        {
                                GameManager::getInstance()->setHeelsLives( howManyLives );
                                GameManager::getInstance()->setHandbag( hasHandbag );
                                GameManager::getInstance()->setHighJumps( 0 );
                                GameManager::getInstance()->setHeelsShield( 0 );
                        }

                        mapManager->beginOldGameWithCharacter( room, label, x, y, z, Way( directionString ), entryString, isActiveCharacter );
                }
        }

        std::cout << "continue previous game" << std::endl ;
        /// no begin.ogg here
}

void Isomot::prepare ()
{
        if ( this->view == nilPointer )
        {
                // image where the isometric view will be drawn
                this->view = new Picture( ScreenWidth(), ScreenHeight() );
        }

        // set of graphics may change between games
        // new ItemDataManager is needed to refresh pictures of items
        GameManager::getInstance()->binItemDataManager();

        ItemDataManager* itemDataManager = new ItemDataManager( "items.xml" );
        itemDataManager->loadItems ();
        GameManager::getInstance()->setItemDataManager( itemDataManager );

        if ( this->mapManager == nilPointer )
        {
                // map of game
                this->mapManager = new MapManager( this );
                this->mapManager->loadMap ( isomot::sharePath() + "map" + pathSeparator + "map.xml" );
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
        delete this->view ;
        this->view = nilPointer ;

        this->mapManager->binEveryRoom();

        // bin taken bonuses
        BonusManager::getInstance()->reset();
}

Picture* Isomot::update()
{
        Room* activeRoom = mapManager->getActiveRoom();
        GameManager* gameManager = GameManager::getInstance();

        if ( allegro::isKeyPushed( "PrintScreen" ) )
        {
                gameManager->toggleRecordingCaptures ();
                allegro::releaseKey( "PrintScreen" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "f" ) )
        {
                gui::GuiManager::getInstance()->toggleFullScreenVideo ();
                allegro::releaseKey( "f" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isKeyPushed( "PageUp" ) )
        {
                if ( ! gameManager->charactersFly() )
                {
                        gameManager->setCharactersFly( true );
                        std::cout << "characters fly and don’t fall" << std::endl ;
                }
                allegro::releaseKey( "PageUp" );
        }
        if ( allegro::isAltKeyPushed() && allegro::isKeyPushed( "PageDown" ) )
        {
                if ( gameManager->charactersFly() )
                {
                        gameManager->setCharactersFly( false );
                        std::cout << "characters feel gravity again" << std::endl ;
                }
                allegro::releaseKey( "PageDown" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "=" ) )
        {
                gameManager->addLives( activeRoom->getMediator()->getLabelOfActiveCharacter(), 1 );
                allegro::releaseKey( "=" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "i" ) )
        {
                gameManager->toggleInfiniteLives ();
                allegro::releaseKey( "i" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "c" ) )
        {
                gameManager->toggleImmunityToCollisionsWithMortalItems ();
                allegro::releaseKey( "c" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "b" ) )
        {
                gameManager->toggleBackgroundPicture ();
                allegro::releaseKey( "b" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "t" ) )
        {
                GameManager::getInstance()->togglePlayMelodyOfScenery ();

                if ( GameManager::getInstance()->playMelodyOfScenery() )
                {
                        std::cout << "room tunes on" << std::endl ;
                        playTuneForScenery( activeRoom->getScenery () );
                } else
                        std::cout << "room tunes off" << std::endl ;

                allegro::releaseKey( "t" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "s" ) )
        {
                gameManager->eatFish ( activeRoom->getMediator()->getActiveCharacter(), activeRoom );
                allegro::releaseKey( "s" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "Pad 8" ) )
        {
                activeRoom->getCamera()->setDeltaY( activeRoom->getCamera()->getDeltaY() - 2 );
        }
        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "Pad 2" ) )
        {
                activeRoom->getCamera()->setDeltaY( activeRoom->getCamera()->getDeltaY() + 2 );
        }
        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "Pad 4" ) )
        {
                activeRoom->getCamera()->setDeltaX( activeRoom->getCamera()->getDeltaX() - 2 );
        }
        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "Pad 6" ) )
        {
                activeRoom->getCamera()->setDeltaX( activeRoom->getCamera()->getDeltaX() + 2 );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "-" ) )
        {
                unsigned int howManyBars = activeRoom->removeBars ();

                if ( howManyBars > 0 )
                        std::cout << howManyBars << " bars are gone" << std::endl ;

                allegro::releaseKey( "-" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "v" ) )
        {
                activeRoom->dontDisappearOnJump ();
                allegro::releaseKey( "v" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "r" ) )
        {
                playTuneForScenery( activeRoom->getScenery () );
                activeRoom = mapManager->rebuildRoom();

                allegro::releaseKey( "r" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "j" ) )
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
                                GameManager::getInstance()->getItemDataManager()->findDataByLabel( nameOfAnotherPlayer ),
                                playerX, playerY, playerZ, way
                        ) ;

                        std::string behavior = "still";
                        if ( nameOfAnotherPlayer == "head" ) behavior = "behavior of Head";
                        else if ( nameOfAnotherPlayer == "heels" ) behavior = "behavior of Heels";

                        joinedPlayer->assignBehavior( behavior, reinterpret_cast< void * >( GameManager::getInstance()->getItemDataManager() ) );

                        joinedPlayer->fillWithData( gameManager );

                        activeRoom->addPlayerToRoom( joinedPlayer, true );
                        joinedPlayer->getBehavior()->changeActivityOfItem( Activity::BeginWayInTeletransport );

                        roomWithInactivePlayer->removePlayerFromRoom( otherPlayer, true );
                        this->mapManager->removeRoom( roomWithInactivePlayer );
                }

                allegro::releaseKey( "j" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "l" ) )
        {
                if ( gameManager->countFreePlanets() < 5 )
                {
                        if ( activeRoom->getMediator()->findItemByLabel( "crown" ) == nilPointer )
                        {
                                ItemData* chapeauData = GameManager::getInstance()->getItemDataManager()->findDataByLabel( "crown" );

                                int x = ( activeRoom->getLimitAt( "south" ) - activeRoom->getLimitAt( "north" ) + chapeauData->getWidthX() ) >> 1 ;
                                int y = ( activeRoom->getLimitAt( "west" ) - activeRoom->getLimitAt( "east" ) + chapeauData->getWidthY() ) >> 1 ;

                                FreeItem* chapeau = new FreeItem( chapeauData, x, y, 250, Way::Nowhere );
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
                                GameManager::getInstance()->getItemDataManager()->findDataByLabel( nameOfPlayer ),
                                teleportedX, teleportedY, teleportedZ,
                                whichWay
                        ) ;

                        std::string behaviorOfPlayer = "still";
                        if ( nameOfPlayer == "head" ) behaviorOfPlayer = "behavior of Head";
                        else if ( nameOfPlayer == "heels" ) behaviorOfPlayer = "behavior of Heels";
                        else if ( nameOfPlayer == "headoverheels" ) behaviorOfPlayer = "behavior of Head over Heels";

                        teleportedPlayer->assignBehavior( behaviorOfPlayer, reinterpret_cast< void * >( GameManager::getInstance()->getItemDataManager() ) );

                        teleportedPlayer->fillWithData( gameManager );

                        std::string nameOfRoomNearFinal = "blacktooth83tofreedom.xml";
                        Room* roomWithTeleportToFinalScene = this->mapManager->createRoomThenAddItToListOfRooms( nameOfRoomNearFinal, true );
                        roomWithTeleportToFinalScene->addPlayerToRoom( teleportedPlayer, true );
                        teleportedPlayer->getBehavior()->changeActivityOfItem( Activity::BeginWayInTeletransport );

                        activeRoom->removePlayerFromRoom( activePlayer, true );

                        this->mapManager->setActiveRoom( roomWithTeleportToFinalScene );
                        this->mapManager->removeRoom( activeRoom );

                        roomWithTeleportToFinalScene->activate();
                        activeRoom = roomWithTeleportToFinalScene;
                }

                allegro::releaseKey( "l" );
        }

        // swap key changes character and possibly room
        if ( ! this->finalRoomBuilt && InputManager::getInstance()->swapTyped() )
        {
                activeRoom->getMediator()->getActiveCharacter()->wait(); // stop active player

                if ( activeRoom->getMediator()->getActiveCharacter()->getBehavior()->getActivityOfItem() == Activity::Wait )
                {
                        // swap in the same room or between different rooms
                        if ( ! activeRoom->swapCharactersInRoom( GameManager::getInstance()->getItemDataManager() ) )
                        {
                                activeRoom = mapManager->swapRoom();
                        }
                }

                InputManager::getInstance()->releaseKeyFor( "swap" );
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
                allegro::bitBlit (
                        activeRoom->getWhereToDraw()->getAllegroPict(), view->getAllegroPict(),
                        activeRoom->getCamera()->getDeltaX(), activeRoom->getCamera()->getDeltaY(),
                        0, 0,
                        activeRoom->getWhereToDraw()->getWidth(), activeRoom->getWhereToDraw()->getHeight()
                );

                std::string roomFile = activeRoom->getNameOfFileWithDataAboutRoom() ;

                int allegroWhiteColor = Color::whiteColor().toAllegroColor() ;

                if ( ! GameManager::getInstance()->hasBackgroundPicture () )
                {
                        // show information about room and draw miniature of room

                        std::ostringstream roomTiles;
                        roomTiles << activeRoom->getTilesX() << "x" << activeRoom->getTilesY();

                        allegro::textOut( roomFile, view->getAllegroPict(), 12, 8, allegroWhiteColor );
                        allegro::textOut( roomTiles.str(), view->getAllegroPict(), 12, 20, allegroWhiteColor );

                        const unsigned int sizeOfTileForMiniature = 3 ;
                        Isomot::drawMiniatureOfRoom( activeRoom, this->mapManager, view, sizeOfTileForMiniature );
                }

                // cheats

                if ( GameManager::getInstance()->areLivesInexhaustible () )
                {
                        ////allegro::textOut( "VIDAS INFINITAS", view->getAllegroPict(), 18, 10, allegroWhiteColor );
                        allegro::textOut( "INFINITE LIVES", view->getAllegroPict(), view->getWidth() - 128, 10, allegroWhiteColor );
                }

                if ( GameManager::getInstance()->isImmuneToCollisionsWithMortalItems () )
                {
                        allegro::textOut( "INVIOLABILITY", view->getAllegroPict(), ( view->getWidth() >> 1 ) - 50, 10, allegroWhiteColor );
                }

                // la sala final es muy especial
                if ( roomFile == "finalroom.xml" )
                {
                        updateFinalRoom();
                }
        }
        // there’s no active room
        else
        {
                std::cout << "no room, game over" << std::endl ;
                delete this->view ;
                this->view = nilPointer ;
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

                        ItemData* dataOfArrived = GameManager::getInstance()->getItemDataManager()->findDataByLabel( arrivedCharacter );

                        if ( dataOfArrived != nilPointer )
                        {
                                FreeItem* character = new FreeItem( dataOfArrived, 66, 92, Top, Way::South );
                                activeRoom->addFreeItem( character );
                        }
                }

                // crea las coronas recuperadas

                GameManager* gameManager = GameManager::getInstance();
                unsigned int crowns = 0;

                ItemData* dataForChapeau = GameManager::getInstance()->getItemDataManager()->findDataByLabel( "crown" );

                // la corona de Safari
                if ( gameManager->isFreePlanet( "safari" ) )
                {
                        FreeItem* chapeau = new FreeItem( dataForChapeau, 66, 75, Top, Way::Nowhere );
                        activeRoom->addFreeItem( chapeau );
                        crowns++;
                }
                // la corona de Egyptus
                if ( gameManager->isFreePlanet( "egyptus" ) )
                {
                        FreeItem* chapeau = new FreeItem( dataForChapeau, 66, 59, Top, Way::Nowhere );
                        activeRoom->addFreeItem( chapeau );
                        crowns++;
                }
                // la corona de Penitentiary
                if ( gameManager->isFreePlanet( "penitentiary" ) )
                {
                        FreeItem* chapeau = new FreeItem( dataForChapeau, 65, 107, Top, Way::Nowhere );
                        activeRoom->addFreeItem( chapeau );
                        crowns++;
                }
                // la corona de Byblos
                if ( gameManager->isFreePlanet( "byblos" ) )
                {
                        FreeItem* chapeau = new FreeItem( dataForChapeau, 65, 123, Top, Way::Nowhere );
                        activeRoom->addFreeItem( chapeau );
                        crowns++;
                }
                // la corona de Blacktooth
                if ( gameManager->isFreePlanet( "blacktooth" ) )
                {
                        FreeItem* chapeau = new FreeItem( dataForChapeau, 65, 91, Top, Way::Nowhere );
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
                        FreeItem* finalBall = new FreeItem (
                                GameManager::getInstance()->getItemDataManager()->findDataByLabel( "ball" ),
                                146, 93, LayerHeight, Way::Nowhere
                        );
                        finalBall->assignBehavior( "behaivor of final ball", GameManager::getInstance()->getItemDataManager()->findDataByLabel( "bubbles" ) );
                        activeRoom->addFreeItem( finalBall );

                        finalRoomTimer->reset();
                }
        }
}

/* static */
void Isomot::drawMiniatureOfRoom( Room* room, MapManager* mapManager, const Picture* where, const unsigned int sizeOfTile )
{
        assert( room != nilPointer );
        assert( mapManager != nilPointer );
        assert( where != nilPointer );

        MapRoomData* connections = mapManager->findRoomData( room );
        assert( connections != nilPointer );

        Way wayToNextRoom( "nowhere" );

        std::string roomAbove = connections->findConnectedRoom( "up", &wayToNextRoom );
        std::string roomBelow = connections->findConnectedRoom( "down", &wayToNextRoom );
        std::string roomToTeleport = connections->findConnectedRoom( "via teleport", &wayToNextRoom );
        std::string roomToTeleportToo = connections->findConnectedRoom( "via second teleport", &wayToNextRoom );

        int tilesX = room->getTilesX();
        int tilesY = room->getTilesY();

        const unsigned int leftXmap = 24;
        const unsigned int topYmap = roomAbove.empty() ? 28 : 24 + ( 3 * sizeOfTile );

        const unsigned int iniPosX = leftXmap + ( tilesY * ( sizeOfTile << 1 ) );
        const unsigned int iniPosY = topYmap + ( sizeOfTile << 1 );

        unsigned int posX = iniPosX ;
        unsigned int posY = iniPosY ;

        int doorXmid = tilesX >> 1;
        int doorYmid = tilesY >> 1;

        Door* eastDoor = room->getDoorAt( "east" );
        Door* southDoor = room->getDoorAt( "south" );
        Door* westDoor = room->getDoorAt( "west" );
        Door* northDoor = room->getDoorAt( "north" );

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
                        for ( unsigned int pix = 0 ; pix < sizeOfTile ; pix ++ )
                        {
                                where->setPixelAt( posX++, posY, Color::whiteColor() );
                                where->setPixelAt( posX++, posY++, Color::whiteColor() );
                        }
                }
                else
                {
                        if ( eastDoor != nilPointer && ( tile + 1 == doorXmid ) )
                        {
                                if ( tile > 0 /* not first tile */ )
                                        where->setPixelAt( posX, posY, Color::gray50Color() );
                                else
                                        where->setPixelAt( posX + 2, posY + 1, Color::gray50Color() );
                        }

                        posX += sizeOfTile << 1;
                        posY += sizeOfTile;

                        if ( eastDoor != nilPointer && ( tile == doorXmid ) )
                        {
                                if ( tile + 1 < tilesX /* not last tile */ )
                                        where->setPixelAt( posX - 1, posY - 1, Color::gray50Color() );
                                else
                                        where->setPixelAt( posX - 3, posY - 2, Color::gray50Color() );
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
                        for ( unsigned int pix = 0 ; pix < sizeOfTile ; pix ++ )
                        {
                                where->setPixelAt( posX--, posY, Color::whiteColor() );
                                where->setPixelAt( posX--, posY++, Color::whiteColor() );
                        }
                }
                else
                {
                        if ( southDoor != nilPointer && ( tile + 1 == doorYmid ) )
                        {
                                if ( tile > 0 /* not first tile */ )
                                        where->setPixelAt( posX, posY, Color::gray50Color() );
                                else
                                        where->setPixelAt( posX - 2, posY + 1, Color::gray50Color() );
                        }

                        posX -= sizeOfTile << 1;
                        posY += sizeOfTile;

                        if ( southDoor != nilPointer && ( tile == doorYmid ) )
                        {
                                if ( tile + 1 < tilesY /* not last tile */ )
                                        where->setPixelAt( posX + 1, posY - 1, Color::gray50Color() );
                                else
                                        where->setPixelAt( posX + 3, posY - 2, Color::gray50Color()  );
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
                        for ( unsigned int pix = 0 ; pix < sizeOfTile ; pix ++ )
                        {
                                where->setPixelAt( posX--, posY, Color::whiteColor() );
                                where->setPixelAt( posX--, posY++, Color::whiteColor() );
                        }
                }
                else
                {
                        if ( northDoor != nilPointer && ( tile + 1 == doorYmid ) )
                        {
                                if ( tile > 0 /* not first tile */ )
                                        where->setPixelAt( posX, posY, Color::gray50Color() );
                                else
                                        where->setPixelAt( posX - 2, posY + 1, Color::gray50Color() );
                        }

                        posX -= sizeOfTile << 1;
                        posY += sizeOfTile;

                        if ( northDoor != nilPointer && ( tile == doorYmid ) )
                        {
                                if ( tile + 1 < tilesY /* not last tile */ )
                                        where->setPixelAt( posX + 1, posY - 1, Color::gray50Color() );
                                else
                                        where->setPixelAt( posX + 3, posY - 2, Color::gray50Color() );
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
                        for ( unsigned int pix = 0 ; pix < sizeOfTile ; pix ++ )
                        {
                                where->setPixelAt( posX++, posY, Color::whiteColor() );
                                where->setPixelAt( posX++, posY++, Color::whiteColor() );
                        }
                }
                else
                {
                        if ( westDoor != nilPointer && ( tile + 1 == doorXmid ) )
                        {
                                if ( tile > 0 /* not first tile */ )
                                        where->setPixelAt( posX, posY, Color::gray50Color() );
                                else
                                        where->setPixelAt( posX + 2, posY + 1, Color::gray50Color() );
                        }

                        posX += sizeOfTile << 1;
                        posY += sizeOfTile;

                        if ( westDoor != nilPointer && ( tile == doorXmid ) )
                        {
                                if ( tile + 1 < tilesX /* not last tile */ )
                                        where->setPixelAt( posX - 1, posY - 1, Color::gray50Color() );
                                else
                                        where->setPixelAt( posX - 3, posY - 2, Color::gray50Color() );
                        }
                }
        }

        // show teleports on miniature

        std::vector< std::list< GridItem* > > gridItemsInRoom = room->getMediator()->getGridItems();

        for ( unsigned int column = 0; column < gridItemsInRoom.size(); column ++ )
        {
                std::list < GridItem * > columnOfItems = gridItemsInRoom[ column ];

                for ( std::list< GridItem * >::const_iterator gi = columnOfItems.begin (); gi != columnOfItems.end (); ++ gi )
                {
                        if ( ( *gi )->getLabel() == "teleport" && ! roomToTeleport.empty() )
                        {
                                fillIsoTile( where->getAllegroPict(), iniPosX, iniPosY, ( *gi )->getCellX(), ( *gi )->getCellY(), sizeOfTile, Color::yellowColor() );
                        }
                        else if ( ( *gi )->getLabel() == "teleport-too" && ! roomToTeleportToo.empty() )
                        {
                                fillIsoTile( where->getAllegroPict(), iniPosX, iniPosY, ( *gi )->getCellX(), ( *gi )->getCellY(), sizeOfTile, Color::magentaColor() );
                        }
                }
        }

        // show when there’s a room above or below

        if ( ! roomAbove.empty() || ! roomBelow.empty() )
        {
                int miniatureMidX = leftXmap + ( tilesY + tilesX ) * sizeOfTile;
                int abovePointerY = iniPosY - 2;
                int belowPointerY = iniPosY + ( tilesY + tilesX ) * sizeOfTile;
                abovePointerY -= sizeOfTile << 1;
                belowPointerY += sizeOfTile << 1;

                if ( ! roomAbove.empty() ) where->setPixelAt( miniatureMidX, abovePointerY, Color::blueColor() );
                if ( ! roomBelow.empty() ) where->setPixelAt( miniatureMidX, belowPointerY, Color::blueColor() );

                int greenColor = Color::greenColor().toAllegroColor() ;

                const unsigned int linesEven = ( ( sizeOfTile + 1 ) >> 1 ) << 1;

                if ( ! roomAbove.empty() )
                {
                        // draw middle line
                        allegro::drawLine( where->getAllegroPict(), miniatureMidX, abovePointerY - linesEven + 1, miniatureMidX, abovePointerY - ( linesEven << 1 ), greenColor );

                        int pos = 0;
                        for ( unsigned int off = linesEven ; off > 0 ; off -- , pos ++ )
                        {
                                allegro::drawLine( where->getAllegroPict(), miniatureMidX - off, abovePointerY - pos, miniatureMidX - off, abovePointerY - pos - linesEven, greenColor );
                                allegro::drawLine( where->getAllegroPict(), miniatureMidX + off, abovePointerY - pos, miniatureMidX + off, abovePointerY - pos - linesEven, greenColor );
                        }
                }

                if ( ! roomBelow.empty() )
                {
                        // draw middle line
                        allegro::drawLine( where->getAllegroPict(), miniatureMidX, belowPointerY + linesEven - 1, miniatureMidX, belowPointerY + ( linesEven << 1 ), greenColor );

                        int pos = 0;
                        for ( unsigned int off = linesEven ; off > 0 ; off -- , pos ++ )
                        {
                                allegro::drawLine( where->getAllegroPict(), miniatureMidX - off, belowPointerY + pos, miniatureMidX - off, belowPointerY + pos + linesEven, greenColor );
                                allegro::drawLine( where->getAllegroPict(), miniatureMidX + off, belowPointerY + pos, miniatureMidX + off, belowPointerY + pos + linesEven, greenColor );
                        }
                }
        }
}

/* static */
void Isomot::fillIsoTile( const allegro::Pict& where, int x0, int y0, int tileX, int tileY, const unsigned int sizeOfTile, const Color& color )
{
        for ( unsigned int piw = 0 ; piw < sizeOfTile ; piw ++ )
        {
                int x = x0 + ( tileX - tileY ) * ( sizeOfTile << 1 ) - ( piw << 1 ) ;
                int y = y0 + ( tileY + tileX ) * sizeOfTile + piw ;

                for ( unsigned int pix = 0 ; pix < sizeOfTile ; pix ++ )
                {
                        where.setPixelAt( x++, y, color.toAllegroColor() );
                        where.setPixelAt( x++, y++, color.toAllegroColor() );
                }
        }
}

}


#include "Isomot.hpp"
#include "Color.hpp"
#include "FlickeringColor.hpp"
#include "GameManager.hpp"
#include "GuiManager.hpp"
#include "BonusManager.hpp"
#include "InputManager.hpp"
#include "SoundManager.hpp"
#include "Room.hpp"
#include "Mediator.hpp"
#include "Camera.hpp"
#include "PlayerItem.hpp"
#include "Behavior.hpp"


namespace iso
{

Isomot::Isomot( ) :
        view( nilPointer ),
        mapManager(),
        itemDataManager(),
        paused( false ),
        finalRoomTimer( new Timer() ),
        finalRoomBuilt( false ),
        sizeOfTileForMiniature( 3 ),
        cameraFollowsCharacter( true )
{
}

Isomot::~Isomot( )
{
        binView ();
}

void Isomot::binView ()
{
        delete view ;
        view = nilPointer ;

# if defined( DEBUG ) && DEBUG
        chequerboard.clear ();
# endif
}

void Isomot::prepare ()
{
        offRecording() ;
        offVidasInfinitas() ;
        offInviolability() ;
        GameManager::getInstance().setCharactersFly( false );

        paused = false ;

        finalRoomBuilt = false ;
        finalRoomTimer->stop();

        // image where the isometric view is drawn
        if ( view == nilPointer )
                view = new Picture( ScreenWidth(), ScreenHeight() );
        else
                view->fillWithColor( Color::orangeColor() );

        // bin taken bonuses
        BonusManager::getInstance().reset();

        mapManager.binRoomsInPlay();
}

void Isomot::fillItemDataManager ()
{
        itemDataManager.readDataAboutItems( "items.xml" );
}

void Isomot::beginNewGame ()
{
        prepare ();
        fillItemDataManager ();

        mapManager.beginNewGame( GameManager::getInstance().getHeadRoom(), GameManager::getInstance().getHeelsRoom() );

        std::cout << "play new game" << std::endl ;
        SoundManager::getInstance().playOgg ( "music/begin.ogg", /* loop */ false );
}

void Isomot::continueSavedGame ( tinyxml2::XMLElement* characters )
{
        prepare ();

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

                        std::string orientationString = "nowhere";
                        tinyxml2::XMLElement* orientation = player->FirstChildElement( "orientation" );
                        if ( orientation == nilPointer ) orientation = player->FirstChildElement( "direction" );
                        if ( orientation != nilPointer )
                                orientationString = orientation->FirstChild()->ToText()->Value() ;

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

                                GameManager::getInstance().setHeadLives( headLives );
                                GameManager::getInstance().setHeelsLives( heelsLives );
                                GameManager::getInstance().setHorn( hasHorn );
                                GameManager::getInstance().setDonuts( howManyDoughnuts );
                                GameManager::getInstance().setHandbag( hasHandbag );
                                GameManager::getInstance().setHighSpeed( 0 );
                                GameManager::getInstance().setHighJumps( 0 );
                                GameManager::getInstance().setHeadShield( 0 );
                                GameManager::getInstance().setHeelsShield( 0 );
                        }
                        else if ( label == "head" )
                        {
                                GameManager::getInstance().setHeadLives( howManyLives );
                                GameManager::getInstance().setHorn( hasHorn );
                                GameManager::getInstance().setDonuts( howManyDoughnuts );
                                GameManager::getInstance().setHighSpeed( 0 );
                                GameManager::getInstance().setHeadShield( 0 );
                        }
                        else if ( label == "heels" )
                        {
                                GameManager::getInstance().setHeelsLives( howManyLives );
                                GameManager::getInstance().setHandbag( hasHandbag );
                                GameManager::getInstance().setHighJumps( 0 );
                                GameManager::getInstance().setHeelsShield( 0 );
                        }

                        mapManager.beginOldGameWithCharacter( room, label, x, y, z, Way( orientationString ), entryString, isActiveCharacter );
                }
        }

        std::cout << "continue previous game" << std::endl ;
        // no begin.ogg here
}

void Isomot::offRecording ()
{
        if ( GameManager::getInstance().recordingCaptures () )
        {
                GameManager::getInstance().toggleRecordingCaptures ();
        }
}

void Isomot::offVidasInfinitas ()
{
        if ( GameManager::getInstance().areLivesInexhaustible () )
        {
                GameManager::getInstance().toggleInfiniteLives ();
        }
}

void Isomot::offInviolability ()
{
        if ( GameManager::getInstance().isImmuneToCollisionsWithMortalItems () )
        {
                GameManager::getInstance().toggleImmunityToCollisionsWithMortalItems ();
        }
}

void Isomot::pause ()
{
        if ( mapManager.getActiveRoom() != nilPointer )
        {
                mapManager.getActiveRoom()->deactivate();
        }

        paused = true ;
}

void Isomot::resume ()
{
        if ( mapManager.getActiveRoom() != nilPointer )
        {
                mapManager.getActiveRoom()->activate();
        }

        paused = false ;
}

Picture* Isomot::updateMe ()
{
        if ( view == nilPointer ) return nilPointer ;

# if defined( DEBUG ) && DEBUG
        if ( chequerboard == nilPointer )
        {
                chequerboard = PicturePtr( new Picture( view->getWidth(), view->getHeight() ) );
                chequerboard->fillWithTransparencyChequerboard ();
        }

        allegro::bitBlit( chequerboard->getAllegroPict(), view->getAllegroPict() );
# else
        Color backgroundColor = Color::blackColor();
        if ( GameManager::getInstance().charactersFly() ) backgroundColor = Color::darkBlueColor();

        view->fillWithColor( backgroundColor );
# endif

        Room* activeRoom = mapManager.getActiveRoom();
        if ( activeRoom == nilPointer ) return view ;
        std::string roomFile = activeRoom->getNameOfFileWithDataAboutRoom() ;

        handleMagicKeys ();

        // la sala final es muy especial
        if ( roomFile == "finalroom.xml" )
        {
                updateFinalRoom();
        }
        else
        {
                Room* previousRoom = activeRoom ;
                PlayerItem& activeCharacter = * activeRoom->getMediator()->getActiveCharacter() ;

                // swap key changes character and possibly room
                if ( InputManager::getInstance().swapTyped() )
                {
                        activeCharacter.wait(); // stop active player

                        if ( activeCharacter.getBehavior()->getActivityOfItem() == Activity::Wait )
                        {
                                // swap in the same room or between different rooms
                                if ( ! activeRoom->swapCharactersInRoom() )
                                {
                                        activeRoom = mapManager.swapRoom();
                                }
                        }

                        InputManager::getInstance().releaseKeyFor( "swap" );
                }

                if ( activeCharacter.getWayOfExit() == "rebuild room" )
                {
                        // active player lost its life

                        if ( activeCharacter.getLives() != 0 || activeCharacter.getLabel() == "headoverheels" )
                        {
                                activeRoom = mapManager.rebuildRoom();
                        }
                        else
                        {
                                if ( ! activeRoom->continueWithAlivePlayer( ) )
                                {
                                        activeRoom = mapManager.noLivesSwap ();
                                }
                        }
                }
                else if ( activeCharacter.getWayOfExit() != "no exit" )
                {
                        // there’s change of room

                        Room* newRoom = mapManager.changeRoom();

                        if ( newRoom != nilPointer && newRoom != activeRoom )
                        {
                                playTuneForScenery( newRoom->getScenery () );
                        }

                        activeRoom = newRoom;
                }

                if ( activeRoom != previousRoom )
                        activeRoom->getCamera()->centerRoom();
        }

        // draw active room

        activeRoom->drawRoom();

        if ( cameraFollowsCharacter )
                activeRoom->getCamera()->centerOnItem( * activeRoom->getMediator()->getActiveCharacter() );

        const int cameraDeltaX = activeRoom->getCamera()->getDeltaX();
        const int cameraDeltaY = activeRoom->getCamera()->getDeltaY();

        allegro::bitBlit (
                activeRoom->getWhereToDraw()->getAllegroPict(), view->getAllegroPict(),
                cameraDeltaX, cameraDeltaY,
                0, 0,
                view->getWidth(), view->getHeight()
        );

        AllegroColor allegroWhiteColor = Color::whiteColor().toAllegroColor() ;

        if ( ! GameManager::getInstance().hasBackgroundPicture () )
        {
                // show information about room and draw miniature of room

                std::ostringstream roomTiles;
                roomTiles << activeRoom->getTilesX() << "x" << activeRoom->getTilesY();

                allegro::textOut( roomFile, view->getAllegroPict(), 12, 8, allegroWhiteColor );
                allegro::textOut( roomTiles.str(), view->getAllegroPict(), 12, 20, allegroWhiteColor );

                Isomot::drawMiniatureOfRoom( *activeRoom, view, sizeOfTileForMiniature );
        }

        // cheats

        if ( GameManager::getInstance().areLivesInexhaustible () )
        {
                ///allegro::textOut( "VIDAS INFINITAS", view->getAllegroPict(), 18, 10, allegroWhiteColor );
                allegro::textOut( "INFINITE LIVES", view->getAllegroPict(), view->getWidth() - 128, 10, allegroWhiteColor );
        }

        if ( GameManager::getInstance().isImmuneToCollisionsWithMortalItems () )
        {
                allegro::textOut( "INVIOLABILITY", view->getAllegroPict(), ( view->getWidth() - 13 * 8 ) >> 1, 10, allegroWhiteColor );
        }

        if ( paused )
        {
                Color::multiplyWithColor( view, Color::gray50Color() );
        }

        return this->view;
}

void Isomot::handleMagicKeys ()
{
        Room* activeRoom = mapManager.getActiveRoom();
        GameManager& gameManager = GameManager::getInstance();

        if ( allegro::isKeyPushed( "PrintScreen" ) )
        {
                gameManager.toggleRecordingCaptures ();
                allegro::releaseKey( "PrintScreen" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "f" ) )
        {
                gui::GuiManager::getInstance().toggleFullScreenVideo ();
                allegro::releaseKey( "f" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isKeyPushed( "PageUp" ) )
        {
                if ( ! gameManager.charactersFly() )
                {
                        gameManager.setCharactersFly( true );
                        std::cout << "characters fly and don’t fall" << std::endl ;
                }
                allegro::releaseKey( "PageUp" );
        }
        if ( allegro::isAltKeyPushed() && allegro::isKeyPushed( "PageDown" ) )
        {
                if ( gameManager.charactersFly() )
                {
                        gameManager.setCharactersFly( false );
                        std::cout << "characters feel gravity again" << std::endl ;
                }
                allegro::releaseKey( "PageDown" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "=" ) )
        {
                gameManager.addLives( activeRoom->getMediator()->getLabelOfActiveCharacter(), 1 );
                allegro::releaseKey( "=" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "i" ) )
        {
                gameManager.toggleInfiniteLives ();
                allegro::releaseKey( "i" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "c" ) )
        {
                gameManager.toggleImmunityToCollisionsWithMortalItems ();
                allegro::releaseKey( "c" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "b" ) )
        {
                gameManager.toggleBackgroundPicture ();
                allegro::releaseKey( "b" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "t" ) )
        {
                GameManager::getInstance().togglePlayMelodyOfScenery ();

                if ( GameManager::getInstance().playMelodyOfScenery() )
                {
                        std::cout << "room tunes on" << std::endl ;
                        playTuneForScenery( activeRoom->getScenery () );
                } else
                        std::cout << "room tunes off" << std::endl ;

                allegro::releaseKey( "t" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "s" ) )
        {
                gameManager.eatFish ( * activeRoom->getMediator()->getActiveCharacter(), activeRoom );
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
        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "Pad 5" ) )
        {
                activeRoom->getCamera()->centerRoom();
                cameraFollowsCharacter = false ;
        }
        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "Pad 0" ) )
        {
                cameraFollowsCharacter = true ;
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
                activeRoom = mapManager.rebuildRoom();

                allegro::releaseKey( "r" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "j" ) )
        {
                PlayerItemPtr activePlayer = activeRoom->getMediator()->getActiveCharacter();
                PlayerItemPtr otherPlayer ;

                Room* roomWithInactivePlayer = mapManager.getRoomOfInactivePlayer();
                if ( roomWithInactivePlayer != nilPointer )
                {
                        otherPlayer = roomWithInactivePlayer->getMediator()->getActiveCharacter() ;
                }

                if ( otherPlayer != nilPointer && roomWithInactivePlayer != activeRoom )
                {
                        std::cout << "join characters in active room \"" << activeRoom->getNameOfFileWithDataAboutRoom() << "\" via pure magic" << std::endl ;

                        std::string nameOfAnotherPlayer = otherPlayer->getLabel();

                        int playerX = activePlayer->getX();
                        int playerY = activePlayer->getY();
                        int playerZ = activePlayer->getZ() + 2 * LayerHeight;
                        Way way = otherPlayer->getOrientation();

                        PlayerItemPtr joinedPlayer( new PlayerItem(
                                itemDataManager.findDataByLabel( nameOfAnotherPlayer ),
                                playerX, playerY, playerZ, way
                        ) );

                        std::string behavior = "still";
                        if ( nameOfAnotherPlayer == "head" ) behavior = "behavior of Head";
                        else if ( nameOfAnotherPlayer == "heels" ) behavior = "behavior of Heels";

                        joinedPlayer->setBehaviorOf( behavior );

                        joinedPlayer->fillWithData( gameManager );

                        activeRoom->addPlayerToRoom( joinedPlayer, true );
                        joinedPlayer->getBehavior()->changeActivityOfItem( Activity::BeginWayInTeletransport );

                        roomWithInactivePlayer->removePlayerFromRoom( *otherPlayer, true );
                        mapManager.removeRoomInPlay( roomWithInactivePlayer );
                }

                allegro::releaseKey( "j" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "l" ) )
        {
                if ( gameManager.countFreePlanets() < 5 )
                {
                        if ( activeRoom->getMediator()->findItemByLabel( "crown" ) == nilPointer )
                        {
                                const ItemData* chapeauData = itemDataManager.findDataByLabel( "crown" );

                                int x = ( activeRoom->getLimitAt( "south" ) - activeRoom->getLimitAt( "north" ) + chapeauData->getWidthX() ) >> 1 ;
                                int y = ( activeRoom->getLimitAt( "west" ) - activeRoom->getLimitAt( "east" ) + chapeauData->getWidthY() ) >> 1 ;

                                FreeItemPtr chapeau( new FreeItem( chapeauData, x, y, 250, Way::Nowhere ) );
                                chapeau->setBehaviorOf( "behavior of something special" );
                                activeRoom->addFreeItem( chapeau );
                        }
                }
                else
                {
                        PlayerItemPtr activePlayer = activeRoom->getMediator()->getActiveCharacter();
                        std::string nameOfPlayer = activePlayer->getLabel();
                        Way whichWay = activePlayer->getOrientation();
                        int teleportedX = 0;
                        int teleportedY = 95;
                        int teleportedZ = 240;

                        PlayerItemPtr teleportedPlayer( new PlayerItem(
                                itemDataManager.findDataByLabel( nameOfPlayer ),
                                teleportedX, teleportedY, teleportedZ,
                                whichWay
                        ) ) ;

                        std::string behaviorOfPlayer = "still";
                        if ( nameOfPlayer == "head" ) behaviorOfPlayer = "behavior of Head";
                        else if ( nameOfPlayer == "heels" ) behaviorOfPlayer = "behavior of Heels";
                        else if ( nameOfPlayer == "headoverheels" ) behaviorOfPlayer = "behavior of Head over Heels";

                        teleportedPlayer->setBehaviorOf( behaviorOfPlayer );

                        teleportedPlayer->fillWithData( gameManager );

                        std::string nameOfRoomNearFinal = "blacktooth83tofreedom.xml";
                        Room* roomWithTeleportToFinalScene = mapManager.getRoomThenAddItToRoomsInPlay( nameOfRoomNearFinal, true );
                        roomWithTeleportToFinalScene->addPlayerToRoom( teleportedPlayer, true );
                        teleportedPlayer->getBehavior()->changeActivityOfItem( Activity::BeginWayInTeletransport );

                        activeRoom->removePlayerFromRoom( *activePlayer, true );

                        mapManager.setActiveRoom( roomWithTeleportToFinalScene );
                        mapManager.removeRoomInPlay( activeRoom );

                        roomWithTeleportToFinalScene->activate();
                        activeRoom = roomWithTeleportToFinalScene;
                }

                allegro::releaseKey( "l" );
        }

        if ( allegro::isShiftKeyPushed() && allegro::isKeyPushed( "Pad -" ) )
        {
                if ( sizeOfTileForMiniature > 2 ) sizeOfTileForMiniature --;
                allegro::releaseKey( "Pad -" );
        }
        if ( allegro::isShiftKeyPushed() && allegro::isKeyPushed( "Pad +" ) )
        {
                if ( sizeOfTileForMiniature < 10 ) sizeOfTileForMiniature ++;
                allegro::releaseKey( "Pad +" );
        }
}

void Isomot::playTuneForScenery ( const std::string& scenery )
{
        if ( scenery != "" && GameManager::getInstance().playMelodyOfScenery () )
        {
                SoundManager::getInstance().playOgg ( "music/" + scenery + ".ogg", /* loop */ false );
        } else
                std::cout << "( ignore melody for scenery \"" << scenery << "\" )" << std::endl ;
}

void Isomot::updateFinalRoom()
{
        Room* activeRoom = mapManager.getActiveRoom();
        assert( activeRoom != nilPointer );
        Mediator* mediator = activeRoom->getMediator();
        assert( mediator != nilPointer );

        if ( ! this->finalRoomBuilt )
        {
                mediator->endUpdate();

                std::string arrivedCharacter = mediator->getActiveCharacter()->getOriginalLabel();

                activeRoom->removePlayerFromRoom( * mediator->getActiveCharacter(), true );

                std::cout << "character \"" << arrivedCharacter << "\" arrived to final room" << std::endl ;

                const ItemData* dataOfArrived = itemDataManager.findDataByLabel( arrivedCharacter );

                if ( dataOfArrived != nilPointer )
                {
                        FreeItemPtr character( new FreeItem( dataOfArrived, 66, 92, Top, Way::South ) );
                        activeRoom->addFreeItem( character );
                }

                activeRoom->getCamera()->centerRoom ();

                // crea las coronas recuperadas

                GameManager& gameManager = GameManager::getInstance();
                unsigned int crowns = 0;

                const ItemData* dataForChapeau = itemDataManager.findDataByLabel( "crown" );

                // la corona de Safari
                if ( gameManager.isFreePlanet( "safari" ) )
                {
                        FreeItemPtr chapeau( new FreeItem( dataForChapeau, 66, 75, Top, Way::Nowhere ) );
                        activeRoom->addFreeItem( chapeau );
                        crowns++;
                }
                // la corona de Egyptus
                if ( gameManager.isFreePlanet( "egyptus" ) )
                {
                        FreeItemPtr chapeau( new FreeItem( dataForChapeau, 66, 59, Top, Way::Nowhere ) );
                        activeRoom->addFreeItem( chapeau );
                        crowns++;
                }
                // la corona de Penitentiary
                if ( gameManager.isFreePlanet( "penitentiary" ) )
                {
                        FreeItemPtr chapeau( new FreeItem( dataForChapeau, 65, 107, Top, Way::Nowhere ) );
                        activeRoom->addFreeItem( chapeau );
                        crowns++;
                }
                // la corona de Byblos
                if ( gameManager.isFreePlanet( "byblos" ) )
                {
                        FreeItemPtr chapeau( new FreeItem( dataForChapeau, 65, 123, Top, Way::Nowhere ) );
                        activeRoom->addFreeItem( chapeau );
                        crowns++;
                }
                // la corona de Blacktooth
                if ( gameManager.isFreePlanet( "blacktooth" ) )
                {
                        FreeItemPtr chapeau( new FreeItem( dataForChapeau, 65, 91, Top, Way::Nowhere ) );
                        activeRoom->addFreeItem( chapeau );
                        crowns++;
                }

                mediator->beginUpdate();

                if ( crowns == 5 )
                {
                        // all five crowns are taken, show the greeting screen
                        gameManager.success();
                }
                else
                {
                        // if not, just go to the summary screen
                        gameManager.arriveInFreedom();
                }

                finalRoomTimer->reset();
                finalRoomTimer->go();

                this->finalRoomBuilt = true;
                std::cout << "final room is okay" << std::endl ;

                SoundManager::getInstance().playOgg ( "music/freedom.ogg", /* loop */ true );
        }
        else
        {
                if ( finalRoomTimer->getValue() > 4 /* each 4 seconds */ )
                {
                        FreeItemPtr finalBall( new FreeItem (
                                itemDataManager.findDataByLabel( "ball" ),
                                146, 93, LayerHeight, Way::Nowhere
                        ) );
                        finalBall->setBehaviorOf( "behaivor of final ball" );
                        activeRoom->addFreeItem( finalBall );

                        finalRoomTimer->reset();
                }
        }
}

/* static */
void Isomot::drawMiniatureOfRoom( const Room& room, const Picture* where, const unsigned int sizeOfTile )
{
        assert( where != nilPointer );

        if ( sizeOfTile < 2 ) return ;

        const RoomConnections* connections = room.getConnections();
        assert( connections != nilPointer );

        Way wayToNextRoom( "nowhere" );

        std::string roomAbove = connections->findConnectedRoom( "up", &wayToNextRoom );
        std::string roomBelow = connections->findConnectedRoom( "down", &wayToNextRoom );
        std::string roomToTeleport = connections->findConnectedRoom( "via teleport", &wayToNextRoom );
        std::string roomToTeleportToo = connections->findConnectedRoom( "via second teleport", &wayToNextRoom );

        const unsigned int tilesX = room.getTilesX();
        const unsigned int tilesY = room.getTilesY();

        const unsigned int leftXmap = 24;
        const unsigned int topYmap = roomAbove.empty() ? 28 : 24 + ( 3 * sizeOfTile );

        const unsigned int iniX = leftXmap + ( tilesY * ( sizeOfTile << 1 ) );
        const unsigned int iniY = topYmap + ( sizeOfTile << 1 );

        unsigned int firstTileX = 0;
        unsigned int firstTileY = 0;
        unsigned int lastTileX = tilesX - 1;
        unsigned int lastTileY = tilesY - 1;

        Door* eastDoor = room.getDoorAt( "east" );
        Door* southDoor = room.getDoorAt( "south" );
        Door* northDoor = room.getDoorAt( "north" );
        Door* westDoor = room.getDoorAt( "west" );

        Door* eastnorthDoor = room.getDoorAt( "eastnorth" );
        Door* eastsouthDoor = room.getDoorAt( "eastsouth" );
        Door* southeastDoor = room.getDoorAt( "southeast" );
        Door* southwestDoor = room.getDoorAt( "southwest" );
        Door* northeastDoor = room.getDoorAt( "northeast" );
        Door* northwestDoor = room.getDoorAt( "northwest" );
        Door* westnorthDoor = room.getDoorAt( "westnorth" );
        Door* westsouthDoor = room.getDoorAt( "westsouth" );

        if ( northDoor != nilPointer || northeastDoor != nilPointer || northwestDoor != nilPointer )
                firstTileX++ ;

        if ( eastDoor != nilPointer || eastnorthDoor != nilPointer || eastsouthDoor != nilPointer )
                firstTileY++ ;

        if ( southDoor != nilPointer || southeastDoor != nilPointer || southwestDoor != nilPointer )
                lastTileX --;

        if ( westDoor != nilPointer || westnorthDoor != nilPointer || westsouthDoor != nilPointer )
                lastTileY --;

        bool narrowRoomAlongX = ( lastTileY == firstTileY + 1 ) ;
        bool narrowRoomAlongY = ( lastTileX == firstTileX + 1 ) ;

        // draw doors

        if ( eastDoor != nilPointer && ! narrowRoomAlongY )
        {
                unsigned int eastDoorXmid = eastDoor->getCellX() + 1;
                drawEastDoorOnMiniature( where->getAllegroPict(), iniX, iniY, eastDoorXmid, eastDoor->getCellY(), sizeOfTile, Color::whiteColor() );
        }
        if ( eastnorthDoor != nilPointer )
        {
                unsigned int eastnorthDoorXmid = eastnorthDoor->getCellX() + 1;
                drawEastDoorOnMiniature( where->getAllegroPict(), iniX, iniY, eastnorthDoorXmid, eastnorthDoor->getCellY(), sizeOfTile, Color::whiteColor() );
        }
        if ( eastsouthDoor != nilPointer )
        {
                unsigned int eastsouthDoorXmid = eastsouthDoor->getCellX() + 1;
                drawEastDoorOnMiniature( where->getAllegroPict(), iniX, iniY, eastsouthDoorXmid, eastsouthDoor->getCellY(), sizeOfTile, Color::whiteColor() );
        }

        if ( northDoor != nilPointer && ! narrowRoomAlongX )
        {
                unsigned int northDoorYmid = northDoor->getCellY() + 1;
                drawNorthDoorOnMiniature( where->getAllegroPict(), iniX, iniY, northDoor->getCellX(), northDoorYmid, sizeOfTile, Color::whiteColor() );
        }
        if ( northeastDoor != nilPointer )
        {
                unsigned int northeastDoorYmid = northeastDoor->getCellY() + 1;
                drawNorthDoorOnMiniature( where->getAllegroPict(), iniX, iniY, northeastDoor->getCellX(), northeastDoorYmid, sizeOfTile, Color::whiteColor() );
        }
        if ( northwestDoor != nilPointer )
        {
                unsigned int northwestDoorYmid = northwestDoor->getCellY() + 1;
                drawNorthDoorOnMiniature( where->getAllegroPict(), iniX, iniY, northwestDoor->getCellX(), northwestDoorYmid, sizeOfTile, Color::whiteColor() );
        }

        if ( westDoor != nilPointer && ! narrowRoomAlongY )
        {
                unsigned int westDoorXmid = westDoor->getCellX() + 1;
                drawWestDoorOnMiniature( where->getAllegroPict(), iniX, iniY, westDoorXmid, westDoor->getCellY(), sizeOfTile, Color::whiteColor() );
        }
        if ( westnorthDoor != nilPointer )
        {
                unsigned int westnorthDoorXmid = westnorthDoor->getCellX() + 1;
                drawWestDoorOnMiniature( where->getAllegroPict(), iniX, iniY, westnorthDoorXmid, westnorthDoor->getCellY(), sizeOfTile, Color::whiteColor() );
        }
        if ( westsouthDoor != nilPointer )
        {
                unsigned int westsouthDoorXmid = westsouthDoor->getCellX() + 1;
                drawWestDoorOnMiniature( where->getAllegroPict(), iniX, iniY, westsouthDoorXmid, westsouthDoor->getCellY(), sizeOfTile, Color::whiteColor() );
        }

        if ( southDoor != nilPointer && ! narrowRoomAlongX )
        {
                unsigned int southDoorYmid = southDoor->getCellY() + 1;
                drawSouthDoorOnMiniature( where->getAllegroPict(), iniX, iniY, southDoor->getCellX(), southDoorYmid, sizeOfTile, Color::whiteColor() );
        }
        if ( southeastDoor != nilPointer )
        {
                unsigned int southeastDoorYmid = southeastDoor->getCellY() + 1;
                drawSouthDoorOnMiniature( where->getAllegroPict(), iniX, iniY, southeastDoor->getCellX(), southeastDoorYmid, sizeOfTile, Color::whiteColor() );
        }
        if ( southwestDoor != nilPointer )
        {
                unsigned int southwestDoorYmid = southwestDoor->getCellY() + 1;
                drawSouthDoorOnMiniature( where->getAllegroPict(), iniX, iniY, southwestDoor->getCellX(), southwestDoorYmid, sizeOfTile, Color::whiteColor() );
        }

        // draw boundaries of room

        drawIsoSquare( where->getAllegroPict(), iniX, iniY, tilesX, tilesY, sizeOfTile, Color::gray50Color() );

        // draw walls

        int minXne = firstTileX ;
        int minYne = firstTileY ;
        int maxXsw = lastTileX ;
        int maxYsw = lastTileY ;

        room.getMediator()->lockGridItemsMutex ();

        const std::vector< std::vector< GridItemPtr > > & gridItemsInRoom = room.getGridItems();

        for ( unsigned int column = 0; column < gridItemsInRoom.size(); column ++ )
        {
                const std::vector< GridItemPtr >& columnOfItems = gridItemsInRoom[ column ];

                for ( std::vector< GridItemPtr >::const_iterator gi = columnOfItems.begin (); gi != columnOfItems.end (); ++ gi )
                {
                        const GridItem& item = *( *gi ) ;

                        std::string label = item.getLabel();
                        int tileX = item.getCellX();
                        int tileY = item.getCellY();

                        if ( label == "invisible-wall-x" )
                        {
                                maxYsw = tileY - 1 ;
                        }
                        else if ( label.find( "wall-x" ) != std::string::npos )
                        {
                                minYne = tileY + 1 ;
                        }

                        if ( label == "invisible-wall-y" )
                        {
                                maxXsw = tileX - 1 ;
                        }
                        else if ( label.find( "wall-y" ) != std::string::npos )
                        {
                                minXne = tileX + 1 ;
                        }
                }
        }

        room.getMediator()->unlockGridItemsMutex ();

        for ( unsigned int tile = firstTileX ; tile <= lastTileX ; tile ++ )
        {
                if ( static_cast< int >( tile ) >= minXne )
                {
                        if ( ! ( eastDoor != nilPointer && ( static_cast< int >( tile ) == eastDoor->getCellX() + 1 || static_cast< int >( tile ) == eastDoor->getCellX() ) ) &&
                                ! ( eastnorthDoor != nilPointer && ( static_cast< int >( tile ) == eastnorthDoor->getCellX() + 1 || static_cast< int >( tile ) == eastnorthDoor->getCellX() ) ) &&
                                ! ( eastsouthDoor != nilPointer && ( static_cast< int >( tile ) == eastsouthDoor->getCellX() + 1 || static_cast< int >( tile ) == eastsouthDoor->getCellX() ) ) )
                        {
                                drawIsoTile (
                                        where->getAllegroPict(), iniX, iniY,
                                        tile, firstTileY,
                                        sizeOfTile, Color::whiteColor(),
                                        true, false, false, false );
                        }
                }

                if ( static_cast< int >( tile ) <= maxXsw )
                {
                        if ( ! ( westDoor != nilPointer && ( static_cast< int >( tile ) == westDoor->getCellX() + 1 || static_cast< int >( tile ) == westDoor->getCellX() ) ) &&
                                ! ( westnorthDoor != nilPointer && ( static_cast< int >( tile ) == westnorthDoor->getCellX() + 1 || static_cast< int >( tile ) == westnorthDoor->getCellX() ) ) &&
                                ! ( westsouthDoor != nilPointer && ( static_cast< int >( tile ) == westsouthDoor->getCellX() + 1 || static_cast< int >( tile ) == westsouthDoor->getCellX() ) ) )
                        {
                                drawIsoTile (
                                        where->getAllegroPict(), iniX, iniY,
                                        tile, lastTileY,
                                        sizeOfTile, Color::whiteColor(),
                                        false, false, true, false );
                        }
                }

                if ( narrowRoomAlongX )
                {
                        if ( northDoor != nilPointer )
                        {
                                drawIsoTile ( where->getAllegroPict(), iniX, iniY, 0, firstTileY,
                                                sizeOfTile, Color::gray75Color(),
                                                true, false, false, false );

                                drawIsoTile ( where->getAllegroPict(), iniX, iniY, 0, lastTileY,
                                                sizeOfTile, Color::gray75Color(),
                                                false, false, true, false );
                        }

                        if ( southDoor != nilPointer )
                        {
                                drawIsoTile ( where->getAllegroPict(), iniX, iniY, tilesX - 1, firstTileY,
                                                sizeOfTile, Color::gray75Color(),
                                                true, false, false, false );

                                drawIsoTile ( where->getAllegroPict(), iniX, iniY, tilesX - 1, lastTileY,
                                                sizeOfTile, Color::gray75Color(),
                                                false, false, true, false );
                        }
                }
        }

        for ( unsigned int tile = firstTileY ; tile <= lastTileY ; tile ++ )
        {
                if ( static_cast< int >( tile ) <= maxYsw )
                {
                        if ( ! ( southDoor != nilPointer && ( static_cast< int >( tile ) == southDoor->getCellY() + 1 || static_cast< int >( tile ) == southDoor->getCellY() ) ) &&
                                ! ( southeastDoor != nilPointer && ( static_cast< int >( tile ) == southeastDoor->getCellY() + 1 || static_cast< int >( tile ) == southeastDoor->getCellY() ) ) &&
                                ! ( southwestDoor != nilPointer && ( static_cast< int >( tile ) == southwestDoor->getCellY() + 1 || static_cast< int >( tile ) == southwestDoor->getCellY() ) ) )
                        {
                                drawIsoTile (
                                        where->getAllegroPict(), iniX, iniY,
                                        lastTileX, tile,
                                        sizeOfTile, Color::whiteColor(),
                                        false, false, false, true );
                        }
                }

                if ( static_cast< int >( tile ) >= minYne )
                {
                        if ( ! ( northDoor != nilPointer && ( static_cast< int >( tile ) == northDoor->getCellY() + 1 || static_cast< int >( tile ) == northDoor->getCellY() ) ) &&
                                ! ( northeastDoor != nilPointer && ( static_cast< int >( tile ) == northeastDoor->getCellY() + 1 || static_cast< int >( tile ) == northeastDoor->getCellY() ) ) &&
                                ! ( northwestDoor != nilPointer && ( static_cast< int >( tile ) == northwestDoor->getCellY() + 1 || static_cast< int >( tile ) == northwestDoor->getCellY() ) ) )
                        {
                                drawIsoTile (
                                        where->getAllegroPict(), iniX, iniY,
                                        firstTileX, tile,
                                        sizeOfTile, Color::whiteColor(),
                                        false, true, false, false );
                        }
                }

                if ( narrowRoomAlongY )
                {
                        if ( eastDoor != nilPointer )
                        {
                                drawIsoTile ( where->getAllegroPict(), iniX, iniY, firstTileX, 0,
                                                sizeOfTile, Color::gray75Color(),
                                                false, true, false, false );

                                drawIsoTile ( where->getAllegroPict(), iniX, iniY, lastTileX, 0,
                                                sizeOfTile, Color::gray75Color(),
                                                false, false, false, true );
                        }

                        if ( westDoor != nilPointer )
                        {
                                drawIsoTile ( where->getAllegroPict(), iniX, iniY, firstTileX, tilesY - 1,
                                                sizeOfTile, Color::gray75Color(),
                                                false, true, false, false );

                                drawIsoTile ( where->getAllegroPict(), iniX, iniY, lastTileX, tilesY - 1,
                                                sizeOfTile, Color::gray75Color(),
                                                false, false, false, true );
                        }
                }
        }

        room.getMediator()->lockGridItemsMutex ();

        for ( unsigned int column = 0; column < gridItemsInRoom.size(); column ++ )
        {
                const std::vector< GridItemPtr >& columnOfItems = gridItemsInRoom[ column ];

                for ( std::vector< GridItemPtr >::const_iterator gi = columnOfItems.begin (); gi != columnOfItems.end (); ++ gi )
                {
                        const GridItem& item = *( *gi ) ;

                        std::string label = item.getLabel();
                        int tileX = item.getCellX();
                        int tileY = item.getCellY();

                        // show teleports

                        if ( label == "teleport" && ! roomToTeleport.empty() )
                        {
                                fillIsoTile( where->getAllegroPict(), iniX, iniY, tileX, tileY, sizeOfTile, Color::yellowColor() );
                        }
                        else if ( label == "teleport-too" && ! roomToTeleportToo.empty() )
                        {
                                fillIsoTile( where->getAllegroPict(), iniX, iniY, tileX, tileY, sizeOfTile, Color::magentaColor() );
                        }

                        // show triple room’s walls

                        if ( label.find( "wall-x" ) != std::string::npos )
                        {
                                if ( label.find( "invisible-wall" ) != std::string::npos )
                                        drawIsoTile( where->getAllegroPict(), iniX, iniY, tileX, tileY, sizeOfTile, Color::whiteColor(), true, false, false, false );
                                else
                                        drawIsoTile( where->getAllegroPict(), iniX, iniY, tileX, tileY, sizeOfTile, Color::whiteColor(), false, false, true, false );
                        }
                        if ( label.find( "wall-y" ) != std::string::npos )
                        {
                                if ( label.find( "invisible-wall" ) != std::string::npos )
                                        drawIsoTile( where->getAllegroPict(), iniX, iniY, tileX, tileY, sizeOfTile, Color::whiteColor(), false, true, false, false );
                                else
                                        drawIsoTile( where->getAllegroPict(), iniX, iniY, tileX, tileY, sizeOfTile, Color::whiteColor(), false, false, false, true );
                        }
                }
        }

        room.getMediator()->unlockGridItemsMutex ();

        room.getMediator()->lockFreeItemsMutex ();

        const std::vector< FreeItemPtr > & freeItemsInRoom = room.getFreeItems();

        for ( std::vector< FreeItemPtr >::const_iterator fi = freeItemsInRoom.begin (); fi != freeItemsInRoom.end (); ++ fi )
        {
                if ( *fi == nilPointer || ( *fi )->whichKindOfItem() == "player item" ) continue ;
                const FreeItem& item = *( *fi ) ;

                const ItemData* dataOfItem = item.getDataOfItem();
                std::string label = dataOfItem->getLabel();

                // show tools

                if ( label == "handbag" || label == "horn" || label == "donuts" )
                {
                        unsigned int roomTileSize = room.getSizeOfOneTile() ;

                        // free coordinates to tile coordinates
                        int tileX = ( item.getX() - ( ( roomTileSize - dataOfItem->getWidthX() ) >> 1 ) ) / roomTileSize ;
                        int tileY = ( ( item.getY() + ( ( roomTileSize - dataOfItem->getWidthY() ) >> 1 ) + 1 ) / roomTileSize ) - 1 ;

                        fillIsoTile( where->getAllegroPict(), iniX, iniY, tileX, tileY, sizeOfTile, Color::orangeColor() );
                }

                // show rabbit bonuses

                if ( label == "extra-life" || label == "high-jumps" || label == "high-speed" || label == "shield" )
                {
                        unsigned int roomTileSize = room.getSizeOfOneTile() ;

                        // free coordinates to tile coordinates
                        int tileX = ( item.getX() - ( ( roomTileSize - dataOfItem->getWidthX() ) >> 1 ) ) / roomTileSize ;
                        int tileY = ( ( item.getY() + ( ( roomTileSize - dataOfItem->getWidthY() ) >> 1 ) + 1 ) / roomTileSize ) - 1 ;

                        fillIsoTile( where->getAllegroPict(), iniX, iniY, tileX, tileY, sizeOfTile, Color::cyanColor() );
                }

                // show fish

                if ( label == "reincarnation-fish" || label == "mortal-fish" )
                {
                        unsigned int roomTileSize = room.getSizeOfOneTile() ;

                        // free coordinates to tile coordinates
                        int tileX = ( item.getX() - ( ( roomTileSize - dataOfItem->getWidthX() ) >> 1 ) ) / roomTileSize ;
                        int tileY = ( ( item.getY() + ( ( roomTileSize - dataOfItem->getWidthY() ) >> 1 ) + 1 ) / roomTileSize ) - 1 ;

                        if ( label == "reincarnation-fish" )
                                fillIsoTile( where->getAllegroPict(), iniX, iniY, tileX, tileY, sizeOfTile, Color::greenColor() );
                        else if ( label == "mortal-fish" )
                                fillIsoTile( where->getAllegroPict(), iniX, iniY, tileX, tileY, sizeOfTile, Color::redColor() );
                }
        }

        room.getMediator()->unlockFreeItemsMutex ();

        // show characters in room

        const std::vector< PlayerItemPtr >& charactersInRoom = room.getPlayersYetInRoom();

        for ( std::vector< PlayerItemPtr >::const_iterator pi = charactersInRoom.begin (); pi != charactersInRoom.end (); ++ pi )
        {
                const PlayerItem& character = *( *pi ) ;

                unsigned int roomTileSize = room.getSizeOfOneTile() ;

                // range of tiles where character is
                int xBegin = character.getX() / roomTileSize;
                int xEnd = ( character.getX() + character.getWidthX() - 1 ) / roomTileSize;
                int yBegin = ( character.getY() - character.getWidthY() + 1 ) / roomTileSize;
                int yEnd = character.getY() / roomTileSize;

                /* for ( int x = xBegin ; x <= xEnd ; ++ x )
                        for ( int y = yBegin ; y <= yEnd ; ++ y )
                                drawIsoTile( where->getAllegroPict(), iniX, iniY, x, y, sizeOfTile, Color::blueColor(), true, true, true, true ) ; */

                int deltaWx = ( roomTileSize - character.getWidthX() ) >> 1 ;
                int deltaWy = ( roomTileSize - character.getWidthY() ) >> 1 ;

                int tileX = ( character.getX() > deltaWx ) ? ( character.getX() - deltaWx ) / roomTileSize : 0 ;
                int tileY = ( ( character.getY() + deltaWy + 1 ) / roomTileSize ) - 1 ;
                if ( tileY < 0 ) tileY = 0 ;

                if ( xBegin == xEnd && yBegin == yEnd )
                {
                        tileX = xBegin ;
                        tileY = yEnd ;
                }
                else
                {
                        if ( tileX < xBegin ) tileX = xBegin ;
                        else if ( tileX > xEnd ) tileX = xEnd ;

                        if ( tileY < yBegin ) tileY = yBegin ;
                        else if ( tileY > yEnd ) tileY = yEnd ;
                }

                fillIsoTileInside( where->getAllegroPict(), iniX, iniY, tileX, tileY, sizeOfTile, FlickeringColor::flickerWhiteAndTransparent(), true );
        }

        // show when there’s a room above or below

        if ( ! roomAbove.empty() || ! roomBelow.empty() )
        {
                int miniatureMidX = leftXmap + ( tilesY + tilesX ) * sizeOfTile;
                int abovePointerY = iniY - 2;
                int belowPointerY = iniY + ( tilesY + tilesX ) * sizeOfTile;
                abovePointerY -= sizeOfTile << 1;
                belowPointerY += sizeOfTile << 1;

                AllegroColor greenColor = Color::greenColor().toAllegroColor() ;

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

        // paint final room

        if ( room.getNameOfFileWithDataAboutRoom() == "finalroom.xml" )
        {
                for ( unsigned int x = 0 ; x < tilesX ; ++ x )
                {
                        for ( unsigned int y = 0 ; y < tilesY ; ++ y )
                        {
                                drawIsoTile( where->getAllegroPict(), iniX, iniY, x, y, sizeOfTile, Color::greenColor(), true, true, true, true ) ;
                                fillIsoTileInside( where->getAllegroPict(), iniX, iniY, x, y, sizeOfTile, Color::yellowColor(), true ) ;
                        }
                }
        }
}

/* static */
void Isomot::drawEastDoorOnMiniature( const allegro::Pict& where, int x0, int y0, unsigned int tilesX, unsigned int tilesY, const unsigned int sizeOfTile, const Color& color )
{
        if ( color.isKeyColor() ) return ;
        if ( sizeOfTile < 2 ) return ;

        drawIsoTile( where, x0, y0, tilesX - 1, tilesY, sizeOfTile, color, false, true, false, false );

        {
                int x = x0 + ( ( tilesX - 1 ) - ( tilesY + 1 ) ) * ( sizeOfTile << 1 ) ;
                int y = y0 + ( ( tilesY + 1 ) + ( tilesX - 1 ) ) * sizeOfTile ;
                where.drawPixelAt( x, y, color.toAllegroColor() );
                where.drawPixelAt( x + 1, y, color.toAllegroColor() );
        }

        drawIsoTile( where, x0, y0, tilesX, tilesY, sizeOfTile, color, false, false, false, true );

        {
                int x = x0 + ( ( tilesX + 1 ) - ( tilesY + 1 ) ) * ( sizeOfTile << 1 ) ;
                int y = y0 + ( ( tilesY + 1 ) + ( tilesX + 1 ) ) * sizeOfTile ;
                where.drawPixelAt( x - 2, y - 1, color.toAllegroColor() );
                where.drawPixelAt( x - 1, y - 1, color.toAllegroColor() );
        }
}

/* static */
void Isomot::drawNorthDoorOnMiniature( const allegro::Pict& where, int x0, int y0, unsigned int tilesX, unsigned int tilesY, const unsigned int sizeOfTile, const Color& color )
{
        if ( color.isKeyColor() ) return ;
        if ( sizeOfTile < 2 ) return ;

        drawIsoTile( where, x0, y0, tilesX, tilesY - 1, sizeOfTile, color, true, false, false, false );

        {
                int x = x0 + ( ( tilesX + 1 ) - ( tilesY - 1 ) ) * ( sizeOfTile << 1 ) ;
                int y = y0 + ( ( tilesY - 1 ) + ( tilesX + 1 ) ) * sizeOfTile ;
                where.drawPixelAt( x, y, color.toAllegroColor() );
                where.drawPixelAt( x + 1, y, color.toAllegroColor() );
        }

        drawIsoTile( where, x0, y0, tilesX, tilesY, sizeOfTile, color, false, false, true, false );

        {
                int x = x0 + ( ( tilesX + 1 ) - ( tilesY + 1 ) ) * ( sizeOfTile << 1 ) ;
                int y = y0 + ( ( tilesY + 1 ) + ( tilesX + 1 ) ) * sizeOfTile ;
                where.drawPixelAt( x + 2, y - 1, color.toAllegroColor() );
                where.drawPixelAt( x + 3, y - 1, color.toAllegroColor() );
        }
}

/* static */
void Isomot::drawWestDoorOnMiniature( const allegro::Pict& where, int x0, int y0, unsigned int tilesX, unsigned int tilesY, const unsigned int sizeOfTile, const Color& color )
{
        if ( color.isKeyColor() ) return ;
        if ( sizeOfTile < 2 ) return ;

        drawIsoTile( where, x0, y0, tilesX - 1, tilesY, sizeOfTile, color, false, true, false, false );

        {
                int x = x0 + ( ( tilesX - 1 ) - tilesY ) * ( sizeOfTile << 1 ) ;
                int y = y0 + ( tilesY + ( tilesX - 1 ) ) * sizeOfTile ;
                where.drawPixelAt( x + 2, y - 1, color.toAllegroColor() );
                where.drawPixelAt( x + 3, y - 1, color.toAllegroColor() );
        }

        drawIsoTile( where, x0, y0, tilesX, tilesY, sizeOfTile, color, false, false, false, true );

        {
                int x = x0 + ( ( tilesX + 1 ) - tilesY ) * ( sizeOfTile << 1 ) ;
                int y = y0 + ( tilesY + ( tilesX + 1 ) ) * sizeOfTile ;
                where.drawPixelAt( x, y - 2, color.toAllegroColor() );
                where.drawPixelAt( x + 1, y - 2, color.toAllegroColor() );
        }
}

/* static */
void Isomot::drawSouthDoorOnMiniature( const allegro::Pict& where, int x0, int y0, unsigned int tilesX, unsigned int tilesY, const unsigned int sizeOfTile, const Color& color )
{
        if ( color.isKeyColor() ) return ;
        if ( sizeOfTile < 2 ) return ;

        drawIsoTile( where, x0, y0, tilesX, tilesY - 1, sizeOfTile, color, true, false, false, false );

        {
                int x = x0 + ( tilesX - ( tilesY - 1 ) ) * ( sizeOfTile << 1 ) ;
                int y = y0 + ( ( tilesY - 1 ) + tilesX ) * sizeOfTile ;
                where.drawPixelAt( x - 2, y - 1, color.toAllegroColor() );
                where.drawPixelAt( x - 1, y - 1, color.toAllegroColor() );
        }

        drawIsoTile( where, x0, y0, tilesX, tilesY, sizeOfTile, color, false, false, true, false );

        {
                int x = x0 + ( tilesX - ( tilesY + 1 ) ) * ( sizeOfTile << 1 ) ;
                int y = y0 + ( ( tilesY + 1 ) + tilesX ) * sizeOfTile ;
                where.drawPixelAt( x, y - 2, color.toAllegroColor() );
                where.drawPixelAt( x + 1, y - 2, color.toAllegroColor() );
        }
}

/* static */
void Isomot::drawIsoSquare( const allegro::Pict& where, int x0, int y0, unsigned int tilesX, unsigned int tilesY, const unsigned int sizeOfTile, const Color& color )
{
        if ( color.isKeyColor() ) return ;
        if ( sizeOfTile < 2 ) return ;

        unsigned int posX = x0 ;
        unsigned int posY = y0 ;

        for ( unsigned int tile = 0 ; tile < tilesX ; tile ++ )
        {
                for ( unsigned int pix = 0 ; pix < sizeOfTile ; pix ++ )
                {
                        where.drawPixelAt( posX++, posY, color.toAllegroColor() );
                        where.drawPixelAt( posX++, posY++, color.toAllegroColor() );
                }
        }

        posX--; posY--;

        for ( unsigned int tile = 0 ; tile < tilesY ; tile ++ )
        {
                for ( unsigned int pix = 0 ; pix < sizeOfTile ; pix ++ )
                {
                        where.drawPixelAt( posX--, posY, color.toAllegroColor() );
                        where.drawPixelAt( posX--, posY++, color.toAllegroColor() );
                }
        }

        posX = x0 + 1 ;
        posY = y0 ;

        for ( unsigned int tile = 0 ; tile < tilesY ; tile ++ )
        {
                for ( unsigned int pix = 0 ; pix < sizeOfTile ; pix ++ )
                {
                        where.drawPixelAt( posX--, posY, color.toAllegroColor() );
                        where.drawPixelAt( posX--, posY++, color.toAllegroColor() );
                }
        }

        posX++; posY--;

        for ( unsigned int tile = 0 ; tile < tilesX ; tile ++ )
        {
                for ( unsigned int pix = 0 ; pix < sizeOfTile ; pix ++ )
                {
                        where.drawPixelAt( posX++, posY, color.toAllegroColor() );
                        where.drawPixelAt( posX++, posY++, color.toAllegroColor() );
                }
        }
}

/* static */
void Isomot::drawIsoTile( const allegro::Pict& where, int x0, int y0, int tileX, int tileY, const unsigned int sizeOfTile, const Color& color, bool loX, bool loY, bool hiX, bool hiY )
{
        if ( color.isKeyColor() ) return ;
        if ( sizeOfTile < 2 ) return ;

        int x = 0 ;
        int y = 0 ;

        if ( loX )
        {
                x = x0 + ( tileX - tileY ) * ( sizeOfTile << 1 ) ;
                y = y0 + ( tileY + tileX ) * sizeOfTile ;

                for ( unsigned int pix = 0 ; pix < sizeOfTile ; pix ++ )
                {
                        where.drawPixelAt( x++, y, color.toAllegroColor() );
                        where.drawPixelAt( x++, y++, color.toAllegroColor() );
                }
        }

        if ( loY )
        {
                x = x0 + ( tileX - tileY ) * ( sizeOfTile << 1 ) + 1 ;
                y = y0 + ( tileY + tileX ) * sizeOfTile ;

                for ( unsigned int pix = 0 ; pix < sizeOfTile ; pix ++ )
                {
                        where.drawPixelAt( x--, y, color.toAllegroColor() );
                        where.drawPixelAt( x--, y++, color.toAllegroColor() );
                }
        }

        if ( hiX )
        {
                x = x0 + ( tileX - tileY ) * ( sizeOfTile << 1 ) - ( ( sizeOfTile - 1 ) << 1 ) ;
                y = y0 + ( tileY + tileX ) * sizeOfTile + ( sizeOfTile - 1 ) ;

                for ( unsigned int pix = 0 ; pix < sizeOfTile ; pix ++ )
                {
                        where.drawPixelAt( x++, y, color.toAllegroColor() );
                        where.drawPixelAt( x++, y++, color.toAllegroColor() );
                }
        }

        if ( hiY )
        {
                x = x0 + ( tileX - tileY ) * ( sizeOfTile << 1 ) + ( ( sizeOfTile - 1 ) << 1 ) + 1 ;
                y = y0 + ( tileY + tileX ) * sizeOfTile + ( sizeOfTile - 1 ) ;

                for ( unsigned int pix = 0 ; pix < sizeOfTile ; pix ++ )
                {
                        where.drawPixelAt( x--, y, color.toAllegroColor() );
                        where.drawPixelAt( x--, y++, color.toAllegroColor() );
                }
        }
}

/* static */
void Isomot::fillIsoTile( const allegro::Pict& where, int x0, int y0, int tileX, int tileY, const unsigned int sizeOfTile, const Color& color )
{
        if ( color.isKeyColor() ) return ;
        if ( sizeOfTile < 2 ) return ;

        for ( unsigned int piw = 0 ; piw < sizeOfTile ; piw ++ )
        {
                int x = x0 + ( tileX - tileY ) * ( sizeOfTile << 1 ) - ( piw << 1 ) ;
                int y = y0 + ( tileY + tileX ) * sizeOfTile + piw ;

                for ( unsigned int pix = 0 ; pix < sizeOfTile ; pix ++ )
                {
                        where.drawPixelAt( x++, y, color.toAllegroColor() );
                        where.drawPixelAt( x++, y++, color.toAllegroColor() );
                }
        }
}

/* static */
void Isomot::fillIsoTileInside( const allegro::Pict& where, int x0, int y0, int tileX, int tileY, const unsigned int sizeOfTile, const Color& color, bool fullFill )
{
        if ( color.isKeyColor() ) return ;
        if ( sizeOfTile < 2 ) return ;

        if ( sizeOfTile == 2 )
        {
                int x = x0 + ( tileX - tileY ) * ( sizeOfTile << 1 ) ;
                int y = y0 + ( tileY + tileX ) * sizeOfTile + 1 ;

                where.drawPixelAt( x++, y, color.toAllegroColor() );
                where.drawPixelAt( x++, y++, color.toAllegroColor() );
        }
        else
        {
                for ( unsigned int piw = 1 ; piw < ( sizeOfTile - 1 ) ; piw ++ )
                {
                        int x = x0 + ( tileX - tileY ) * ( sizeOfTile << 1 ) - ( piw << 1 ) + 2 ;
                        int y = y0 + ( tileY + tileX ) * sizeOfTile + piw + 1 ;

                        for ( unsigned int pix = 1 ; pix < ( sizeOfTile - 1 ) ; pix ++ )
                        {
                                if ( fullFill )
                                {
                                        where.drawPixelAt( x, y - 1, color.toAllegroColor() );
                                        where.drawPixelAt( x + 1, y - 1, color.toAllegroColor() );

                                        where.drawPixelAt( x, y + 1, color.toAllegroColor() );
                                        where.drawPixelAt( x + 1, y + 1, color.toAllegroColor() );

                                        where.drawPixelAt( x - 2, y, color.toAllegroColor() );
                                        where.drawPixelAt( x - 1, y, color.toAllegroColor() );

                                        where.drawPixelAt( x + 2, y, color.toAllegroColor() );
                                        where.drawPixelAt( x + 3, y, color.toAllegroColor() );
                                }

                                where.drawPixelAt( x++, y, color.toAllegroColor() );
                                where.drawPixelAt( x++, y++, color.toAllegroColor() );
                        }
                }
        }
}

}

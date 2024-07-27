
#include "Isomot.hpp"

#include "Color.hpp"
#include "GameManager.hpp"
#include "GuiManager.hpp"
#include "BonusManager.hpp"
#include "InputManager.hpp"
#include "SoundManager.hpp"
#include "GamePreferences.hpp"
#include "GameMap.hpp"
#include "Room.hpp"
#include "Mediator.hpp"
#include "Camera.hpp"
#include "Miniature.hpp"
#include "AvatarItem.hpp"
#include "ItemDescriptions.hpp"
#include "Behavior.hpp"


Isomot::Isomot( ) :
        view( nilPointer ),
        paused( false ),
        finalRoomTimer( new Timer() ),
        finalRoomBuilt( false ),
        sizeOfTileForMiniature( 3 ),
        cameraFollowsCharacter( false ),
        drawOnChequerboard( false )
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

        chequerboard.clear ();
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
                view = new Picture( GamePreferences::getScreenWidth(), GamePreferences::getScreenHeight() );
        else
                view->fillWithColor( Color::byName( "orange" ) );

        GameMap::getInstance().binRoomsInPlay();
}

void Isomot::beginNewGame ()
{
        prepare ();

        // bin taken bonuses
        BonusManager::getInstance().clearAbsentBonuses () ;

        GameMap::getInstance().beginNewGame( GameManager::getInstance().getHeadRoom(), GameManager::getInstance().getHeelsRoom() );

        std::cout << "play new game" << std::endl ;
        SoundManager::getInstance().playOgg ( "music/begin.ogg", /* loop */ false );
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
        Room * activeRoom = GameMap::getInstance().getActiveRoom() ;
        if ( activeRoom != nilPointer )
                activeRoom->deactivate() ;

        paused = true ;
}

void Isomot::resume ()
{
        Room * activeRoom = GameMap::getInstance().getActiveRoom() ;
        if ( activeRoom != nilPointer )
                activeRoom->activate() ;

        paused = false ;
}

Picture* Isomot::updateMe ()
{
        if ( view == nilPointer ) return nilPointer ;

        const allegro::Pict& previousWhere = allegro::Pict::getWhereToDraw() ;
        allegro::Pict::setWhereToDraw( view->getAllegroPict() );

        if ( chequerboard == nilPointer )
        {
                chequerboard = PicturePtr( new Picture( view->getWidth(), view->getHeight() ) );
                chequerboard->fillWithTransparencyChequerboard ();
        }

        if ( drawOnChequerboard )
        {
                allegro::bitBlit( chequerboard->getAllegroPict(), view->getAllegroPict() );
        }
        else
        {
                Color backgroundColor = Color::blackColor();
                if ( GameManager::getInstance().charactersFly() ) backgroundColor = Color::byName( "dark blue" );

                view->fillWithColor( backgroundColor );
        }

        handleMagicKeys ();

        GameMap & map = GameMap::getInstance () ;

        Room* activeRoom = map.getActiveRoom() ;
        if ( activeRoom == nilPointer ) return this->view ;

        // la sala final es muy especial
        if ( activeRoom->getNameOfRoomDescriptionFile() == "finalroom.xml" )
        {
                updateFinalRoom();
        }
        else if ( activeRoom->getMediator()->getActiveCharacter() != nilPointer )
        {
                AvatarItem & activeCharacter = * activeRoom->getMediator()->getActiveCharacter() ;
                Room* previousRoom = activeRoom ;

                // the swap key changes character and possibly room
                if ( InputManager::getInstance().swapTyped() )
                {
                        activeCharacter.wait(); // stop the active character

                        if ( activeCharacter.getBehavior()->getCurrentActivity() == activities::Activity::Waiting )
                        {
                                // swap in the same room or between different rooms
                                if ( ! activeRoom->swapCharactersInRoom() )
                                        activeRoom = map.swapRoom() ;
                        }

                        InputManager::getInstance().releaseKeyFor( "swap" );
                }

                if ( ! activeCharacter.getWayOfExit().empty() )
                {
                        // move to the next room
                        Room* newRoom = map.changeRoom() ;

                        if ( newRoom != nilPointer && newRoom != activeRoom )
                                playTuneForScenery( newRoom->getScenery () );

                        activeRoom = newRoom;
                }

                if ( softCenteringOnRoom )
                        softCenteringOnRoom = activeRoom->getCamera()->softCenterRoom () ;
                else
                if ( activeRoom != previousRoom && ! cameraFollowsCharacter )
                        activeRoom->getCamera()->instantCenterRoom () ;
        }

        // draw the active room

        activeRoom = map.getActiveRoom() ;
        if ( activeRoom == nilPointer ) return this->view ;

        activeRoom->drawRoom () ;

        if ( cameraFollowsCharacter && activeRoom->getMediator()->getActiveCharacter() != nilPointer )
                activeRoom->getCamera()->centerOnItem( * activeRoom->getMediator()->getActiveCharacter() );

        const int cameraDeltaX = activeRoom->getCamera()->getOffset().getX ();
        const int cameraDeltaY = activeRoom->getCamera()->getOffset().getY ();

        const Color& roomColor = Color::byName( activeRoom->getColor() );

        if ( GameManager::getInstance().isSimpleGraphicsSet() )
                Color::multiplyWithColor( * activeRoom->getWhereToDraw(), roomColor );

        allegro::Pict::setWhereToDraw( view->getAllegroPict() );

        allegro::drawSprite (
                activeRoom->getWhereToDraw()->getAllegroPict(),
                - cameraDeltaX, - cameraDeltaY
        );

        if ( GameManager::getInstance().drawRoomMiniatures () )
        {
                // show information about the current room and draw the miniature

                std::ostringstream roomTiles;
                roomTiles << activeRoom->getTilesOnX() << "x" << activeRoom->getTilesOnY();

                allegro::textOut( activeRoom->getNameOfRoomDescriptionFile(), 12, 8, roomColor.toAllegroColor() );
                allegro::textOut( roomTiles.str(), 12, 20, roomColor.toAllegroColor() );

                Miniature miniatureOfRoom( *activeRoom, 24, 24, sizeOfTileForMiniature );

                std::string roomAtSouth = activeRoom->getConnections()->getConnectedRoomAt( "south" );
                std::string roomAtNorth = activeRoom->getConnections()->getConnectedRoomAt( "north" );
                std::string roomAtEast = activeRoom->getConnections()->getConnectedRoomAt( "east" );
                std::string roomAtWest = activeRoom->getConnections()->getConnectedRoomAt( "west" );

                if ( ! roomAtSouth.empty () )
                {
                        std::pair< int, int > secondRoomOffset = miniatureOfRoom.calculatePositionOfConnectedMiniature( "south", 0 );
                        Miniature miniatureOfConnectedRoom( * map.getOrBuildRoomByFile( roomAtSouth ),
                                                                secondRoomOffset.first, secondRoomOffset.second,
                                                                sizeOfTileForMiniature );
                        miniatureOfConnectedRoom.draw ();
                }

                if ( ! roomAtNorth.empty() )
                {
                        std::pair< int, int > secondRoomOffset = miniatureOfRoom.calculatePositionOfConnectedMiniature( "north", 0 );
                        Miniature miniatureOfConnectedRoom( * map.getOrBuildRoomByFile( roomAtNorth ),
                                                                secondRoomOffset.first, secondRoomOffset.second,
                                                                sizeOfTileForMiniature );
                        miniatureOfConnectedRoom.draw ();
                }

                if ( ! roomAtEast.empty() )
                {
                        std::pair< int, int > secondRoomOffset = miniatureOfRoom.calculatePositionOfConnectedMiniature( "east", 0 );
                        Miniature miniatureOfConnectedRoom( * map.getOrBuildRoomByFile( roomAtEast ),
                                                                secondRoomOffset.first, secondRoomOffset.second,
                                                                sizeOfTileForMiniature );
                        miniatureOfConnectedRoom.draw ();
                }

                if ( ! roomAtWest.empty() )
                {
                        std::pair< int, int > secondRoomOffset = miniatureOfRoom.calculatePositionOfConnectedMiniature( "west", 0 );
                        Miniature miniatureOfConnectedRoom( * map.getOrBuildRoomByFile( roomAtWest ),
                                                                secondRoomOffset.first, secondRoomOffset.second,
                                                                sizeOfTileForMiniature );
                        miniatureOfConnectedRoom.draw ();
                }

                miniatureOfRoom.draw ();
        }

        // cheats

        if ( GameManager::getInstance().areLivesInexhaustible () )
        {
                std::string infiniteLives = "VIDAS INFINITAS" ;
                allegro::textOut( infiniteLives, view->getWidth() - ( 8 * infiniteLives.length() ) - 16, 10, Color::whiteColor().toAllegroColor() );
        }

        if ( GameManager::getInstance().isImmuneToCollisionsWithMortalItems () )
        {
                std::string immunity = "INVIOLABILITY" ;
                allegro::textOut( immunity, ( view->getWidth() - ( immunity.length() * 8 ) ) >> 1, 10, Color::whiteColor().toAllegroColor() );
        }

        if ( paused )
                Color::multiplyWithColor( * view, Color::byName( "gray" ) );

        allegro::Pict::setWhereToDraw( previousWhere );

        return this->view ;
}

void Isomot::handleMagicKeys ()
{
        GameManager & gameManager = GameManager::getInstance() ;
        GameMap & map = GameMap::getInstance() ;

        Room * activeRoom = map.getActiveRoom() ;

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "v" ) )
        {
                gameManager.toggleRecordingCaptures ();
                allegro::releaseKey( "v" ) ;
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
                gameManager.getGameInfo().addLivesByName( activeRoom->getMediator()->getNameOfActiveCharacter(), 1 );
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

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "y" ) )
        {
                const AvatarItemPtr & activeCharacter = activeRoom->getMediator()->getActiveCharacter() ;
                if ( activeCharacter != nilPointer ) activeCharacter->activateShield ();
                allegro::releaseKey( "y" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "b" ) )
        {
                gameManager.toggleSceneryDecor ();
                allegro::releaseKey( "b" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "m" ) )
        {
                gameManager.toggleRoomMiniatures ();
                allegro::releaseKey( "m" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "w" ) )
        {
                gameManager.toggleDrawingOfWalls ();
                allegro::releaseKey( "w" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "t" ) )
        {
                gameManager.togglePlayMelodyOfScenery ();

                if ( gameManager.playMelodyOfScenery() )
                {
                        std::cout << "room tunes on" << std::endl ;
                        playTuneForScenery( activeRoom->getScenery () );
                } else
                        std::cout << "room tunes off" << std::endl ;

                allegro::releaseKey( "t" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "s" ) )
        {
                if ( activeRoom->getMediator()->getActiveCharacter() != nilPointer )
                        gameManager.eatFish ( * activeRoom->getMediator()->getActiveCharacter(), activeRoom );

                allegro::releaseKey( "s" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "-" ) )
        {
                unsigned int howManyBars = activeRoom->removeItemsOfKind( "bars-ns" );
                howManyBars += activeRoom->removeItemsOfKind( "bars-ew" );
                unsigned int howManyBricks = activeRoom->removeItemsOfKind( "brick2" );
                howManyBricks += activeRoom->removeItemsOfKind( "brick1" );

                if ( howManyBars > 0 ) std::cout << howManyBars << " bars are gone" << std::endl ;
                if ( howManyBricks > 0 ) std::cout << howManyBricks << " bricks are gone" << std::endl ;

                allegro::releaseKey( "-" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "d" ) )
        {
                activeRoom->dontDisappearOnJump ();
                allegro::releaseKey( "d" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "r" ) )
        {
                playTuneForScenery( activeRoom->getScenery () );
                map.rebuildRoom() ;
                activeRoom = map.getActiveRoom() ;

                allegro::releaseKey( "r" );
        }

        // [ option / alt ] & [ shift ] & [ j ] summons the second character in the active room
        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "j" ) )
        {
                AvatarItemPtr activeCharacter = activeRoom->getMediator()->getActiveCharacter ();
                if ( activeCharacter != nilPointer )
                {
                        Room* roomWithInactiveCharacter = map.getRoomOfInactiveCharacter() ;
                        AvatarItemPtr otherCharacter = ( roomWithInactiveCharacter != nilPointer )
                                                                ? roomWithInactiveCharacter->getMediator()->getActiveCharacter()
                                                                : AvatarItemPtr() ;

                        if ( otherCharacter != nilPointer && roomWithInactiveCharacter != activeRoom )
                        {
                                std::cout << "both characters suddenly found themselves together in the active room \""
                                                << activeRoom->getNameOfRoomDescriptionFile() << "\"" << std::endl ;

                                const std::string & nameOfAnotherCharacter = otherCharacter->getKind () ;

                                int characterX = activeCharacter->getX() ;
                                int characterY = activeCharacter->getY() ;
                                int characterZ = activeCharacter->getZ() + 2 * Room::LayerHeight ;
                                const std::string & heading = otherCharacter->getHeading ();

                                AvatarItemPtr joinedCharacter( new AvatarItem(
                                        * ItemDescriptions::descriptions().getDescriptionByKind( nameOfAnotherCharacter ),
                                        characterX, characterY, characterZ, heading
                                ) );

                                activeRoom->placeCharacterInRoom( joinedCharacter, true );
                                joinedCharacter->getBehavior()->setCurrentActivity( activities::Activity::EndTeletransportation );

                                roomWithInactiveCharacter->removeCharacterFromRoom( *otherCharacter, true );
                                map.removeRoomInPlay( roomWithInactiveCharacter );
                        }
                }

                allegro::releaseKey( "j" );
        }

        /*** THE FUTURE CHEAT /////////
        // [ option / alt ] & [ shift ] & [ h ] for a hyperspace jump to a random room on the game map
        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "h" ) )
        {
                std::vector< std::string > allRooms ;
                map.getAllRoomFiles( allRooms );

                const std::string & randomRoom = allRooms[ rand() % allRooms.size() ] ;
                std::cout << "hyper-jumping to the lucky map \"" << randomRoom << "\"" << std::endl ;

                AvatarItemPtr activeCharacter = activeRoom->getMediator()->getActiveCharacter() ;
                if ( activeCharacter != nilPointer )
                {
                        activeCharacter->getBehavior()->setCurrentActivity( activities::Activity::BeginTeletransportation );
                        // ....
                }

                allegro::releaseKey( "h" );
        } ****/

        // [ option / alt ] & [ shift ] & [ l ] to liberate the current planet,
        //      the planet’s crown appears in the active room,
        //      and when all the crowns are collected teleports the active character to the last room
        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "l" ) )
        {
                AvatarItemPtr activeCharacter = activeRoom->getMediator()->getActiveCharacter();
                if ( activeCharacter != nilPointer )
                {
                        if ( gameManager.howManyFreePlanets() < 5 )
                        {
                                if ( activeRoom->getMediator()->findItemOfKind( "crown" ) == nilPointer )
                                {
                                        const DescriptionOfItem* chapeauDescription = ItemDescriptions::descriptions().getDescriptionByKind( "crown" );
                                        assert( chapeauDescription != nilPointer );
                                        FreeItemPtr chapeau( new FreeItem( *chapeauDescription,
                                                                                activeCharacter->getX(), activeCharacter->getY(),
                                                                                chapeauDescription->getHeight() + Room::LayerHeight * Room::MaxLayers ) );
                                        chapeau->setBehaviorOf( "behavior of bonus" );
                                        activeRoom->addFreeItem( chapeau );
                                }
                        }
                        else
                        {
                                const std::string & nameOfCharacter = activeCharacter->getKind () ;
                                const std::string & heading = activeCharacter->getHeading () ;
                                int teleportedX = 0;
                                int teleportedY = 95;
                                int teleportedZ = 240;

                                AvatarItemPtr teleportedCharacter( new AvatarItem(
                                        * ItemDescriptions::descriptions().getDescriptionByKind( nameOfCharacter ),
                                        teleportedX, teleportedY, teleportedZ,
                                        heading
                                ) ) ;

                                std::string nameOfRoomNearFinal = "blacktooth83tofreedom.xml";
                                Room* roomWithTeleportToFinalScene = map.getRoomThenAddItToRoomsInPlay( nameOfRoomNearFinal, true );
                                roomWithTeleportToFinalScene->placeCharacterInRoom( teleportedCharacter, true );
                                teleportedCharacter->getBehavior()->setCurrentActivity( activities::Activity::EndTeletransportation );

                                activeRoom->removeCharacterFromRoom( *activeCharacter, true );

                                map.setActiveRoom( roomWithTeleportToFinalScene );
                                map.removeRoomInPlay( activeRoom );

                                roomWithTeleportToFinalScene->activate();
                                activeRoom = roomWithTeleportToFinalScene;
                        }
                }

                allegro::releaseKey( "l" );
        }

        if ( allegro::isShiftKeyPushed() && allegro::isKeyPushed( "Pad 8" ) )
        {
                activeRoom->getCamera()->shiftAlongY( - 2 );
                cameraFollowsCharacter = false ;
        }
        if ( allegro::isShiftKeyPushed() && allegro::isKeyPushed( "Pad 2" ) )
        {
                activeRoom->getCamera()->shiftAlongY( + 2 );
                cameraFollowsCharacter = false ;
        }
        if ( allegro::isShiftKeyPushed() && allegro::isKeyPushed( "Pad 4" ) )
        {
                activeRoom->getCamera()->shiftAlongX( - 2 );
                cameraFollowsCharacter = false ;
        }
        if ( allegro::isShiftKeyPushed() && allegro::isKeyPushed( "Pad 6" ) )
        {
                activeRoom->getCamera()->shiftAlongX( + 2 );
                cameraFollowsCharacter = false ;
        }
        if ( allegro::isShiftKeyPushed() && allegro::isKeyPushed( "Pad 5" ) )
        {
                softCenteringOnRoom = true ;
                cameraFollowsCharacter = false ;
        }
        if ( allegro::isShiftKeyPushed() && allegro::isKeyPushed( "Pad 0" ) )
        {
                cameraFollowsCharacter = true ;
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

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "." ) )
        {
                drawOnChequerboard = ! drawOnChequerboard ;
                allegro::releaseKey( "." );
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
        Room* activeRoom = GameMap::getInstance().getActiveRoom();
        assert( activeRoom != nilPointer );
        Mediator* mediator = activeRoom->getMediator();
        assert( mediator != nilPointer );

        if ( ! this->finalRoomBuilt )
        {
                mediator->endUpdating ();

                std::string arrivedCharacter = mediator->getActiveCharacter()->getOriginalKind();

                activeRoom->removeCharacterFromRoom( * mediator->getActiveCharacter(), true );

                std::cout << "character \"" << arrivedCharacter << "\" arrived to the final room" << std::endl ;

                const DescriptionOfItem* arrived = ItemDescriptions::descriptions().getDescriptionByKind( arrivedCharacter );
                if ( arrived != nilPointer ) {
                        FreeItemPtr character( new FreeItem( *arrived, 66, 92, Room::FloorZ, "south" ) );
                        activeRoom->addFreeItem( character );
                }

                activeRoom->getCamera()->instantCenterRoom ();

                // crea las coronas

                unsigned int crowns = 0;

                const DescriptionOfItem & descriptionOfChapeau = * ItemDescriptions::descriptions().getDescriptionByKind( "crown" );

                GameManager & gameManager = GameManager::getInstance() ;

                // la corona de Safari
                if ( gameManager.isFreePlanet( "safari" ) )
                {
                        FreeItemPtr chapeau( new FreeItem( descriptionOfChapeau, 66, 75, Room::FloorZ ) );
                        activeRoom->addFreeItem( chapeau );
                        crowns ++ ;
                }
                // la corona de Egyptus
                if ( gameManager.isFreePlanet( "egyptus" ) )
                {
                        FreeItemPtr chapeau( new FreeItem( descriptionOfChapeau, 66, 59, Room::FloorZ ) );
                        activeRoom->addFreeItem( chapeau );
                        crowns ++ ;
                }
                // la corona de Penitentiary
                if ( gameManager.isFreePlanet( "penitentiary" ) )
                {
                        FreeItemPtr chapeau( new FreeItem( descriptionOfChapeau, 65, 107, Room::FloorZ ) );
                        activeRoom->addFreeItem( chapeau );
                        crowns ++ ;
                }
                // la corona de Byblos
                if ( gameManager.isFreePlanet( "byblos" ) )
                {
                        FreeItemPtr chapeau( new FreeItem( descriptionOfChapeau, 65, 123, Room::FloorZ ) );
                        activeRoom->addFreeItem( chapeau );
                        crowns ++ ;
                }
                // la corona de Blacktooth
                if ( gameManager.isFreePlanet( "blacktooth" ) )
                {
                        FreeItemPtr chapeau( new FreeItem( descriptionOfChapeau, 65, 91, Room::FloorZ ) );
                        activeRoom->addFreeItem( chapeau );
                        ++ crowns ;
                }

                mediator->beginUpdating ();

                // it's time to count the crowns
                gameManager.inFreedomWithSoManyCrowns( crowns );

                finalRoomTimer->go() ;

                this->finalRoomBuilt = true;
                std::cout << "final room is okay" << std::endl ;

                SoundManager::getInstance().playOgg ( "music/freedom.ogg", /* loop */ true );
        }
        else
        {
                if ( finalRoomTimer->getValue() > 4 /* each 4 seconds */ )
                {
                        FreeItemPtr finalBall( new FreeItem (
                                * ItemDescriptions::descriptions().getDescriptionByKind( "ball" ),
                                146, 93, Room::LayerHeight, "none"
                        ) );
                        finalBall->setBehaviorOf( "behaivor of final ball" );
                        activeRoom->addFreeItem( finalBall );

                        finalRoomTimer->go() ;
                }
        }
}


#include "Isomot.hpp"

#include "Color.hpp"
#include "GameManager.hpp"
#include "GuiManager.hpp"
#include "BonusManager.hpp"
#include "InputManager.hpp"
#include "SoundManager.hpp"
#include "Room.hpp"
#include "Mediator.hpp"
#include "Camera.hpp"
#include "Miniature.hpp"
#include "AvatarItem.hpp"
#include "ItemDescriptions.hpp"
#include "Behavior.hpp"

#include "screen.hpp"


Isomot::Isomot( ) :
        view( nilPointer ),
        mapManager( ),
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
                view = new Picture( variables::getScreenWidth(), variables::getScreenHeight() );
        else
                view->fillWithColor( Color::byName( "orange" ) );

        // bin taken bonuses
        BonusManager::getInstance().reset();

        mapManager.binRoomsInPlay();
}

void Isomot::beginNewGame ()
{
        prepare ();

        mapManager.beginNewGame( GameManager::getInstance().getHeadRoom(), GameManager::getInstance().getHeelsRoom() );

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

        Room* activeRoom = mapManager.getActiveRoom();
        if ( activeRoom == nilPointer ) return view ;

        // la sala final es muy especial
        if ( activeRoom->getNameOfRoomDescriptionFile() == "finalroom.xml" )
        {
                updateFinalRoom();
        }
        else
        {
                assert( activeRoom->getMediator()->getActiveCharacter() != nilPointer );
                AvatarItem & activeCharacter = * activeRoom->getMediator()->getActiveCharacter() ;
                Room* previousRoom = activeRoom ;

                // swap key changes character and possibly room
                if ( InputManager::getInstance().swapTyped() )
                {
                        activeCharacter.wait(); // stop the character currently active

                        if ( activeCharacter.getBehavior()->getActivityOfItem() == activities::Activity::Wait )
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
                        // the active character lost one life

                        if ( activeCharacter.getLives() != 0 || activeCharacter.getLabel() == "headoverheels" )
                        {
                                activeRoom = mapManager.rebuildRoom();
                        }
                        else
                        {
                                if ( ! activeRoom->continueWithAliveCharacter () )
                                {
                                        activeRoom = mapManager.noLivesSwap ();
                                }
                        }
                }
                else if ( activeCharacter.getWayOfExit() != "did not quit" )
                {
                        // there’s change of room

                        Room* newRoom = mapManager.changeRoom();

                        if ( newRoom != nilPointer && newRoom != activeRoom )
                        {
                                playTuneForScenery( newRoom->getScenery () );
                        }

                        activeRoom = newRoom;
                }

                if ( softCenteringOnRoom )
                        softCenteringOnRoom = activeRoom->getCamera()->softCenterRoom () ;
                else
                if ( activeRoom != previousRoom && ! cameraFollowsCharacter )
                        activeRoom->getCamera()->instantCenterRoom () ;
        }

        // draw active room

        activeRoom->drawRoom();

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
                // show information about room and draw the miniature of room

                std::ostringstream roomTiles;
                roomTiles << activeRoom->getTilesX() << "x" << activeRoom->getTilesY();

                allegro::textOut( activeRoom->getNameOfRoomDescriptionFile(), 12, 8, roomColor.toAllegroColor() );
                allegro::textOut( roomTiles.str(), 12, 20, roomColor.toAllegroColor() );

                Miniature miniatureOfRoom( *activeRoom, 24, 24, sizeOfTileForMiniature );

                Way connectsAt( "nowhere" );
                std::string roomAtSouth = activeRoom->getConnections()->findConnectedRoom( "south", &connectsAt );
                std::string roomAtNorth = activeRoom->getConnections()->findConnectedRoom( "north", &connectsAt );
                std::string roomAtEast = activeRoom->getConnections()->findConnectedRoom( "east", &connectsAt );
                std::string roomAtWest = activeRoom->getConnections()->findConnectedRoom( "west", &connectsAt );

                if ( ! roomAtSouth.empty () )
                {
                        std::pair< int, int > secondRoomOffset = miniatureOfRoom.calculatePositionOfConnectedMiniature( "south", 0 );
                        Miniature miniatureOfConnectedRoom( * mapManager.getOrBuildRoomByFile( roomAtSouth ),
                                                                secondRoomOffset.first, secondRoomOffset.second,
                                                                sizeOfTileForMiniature );
                        miniatureOfConnectedRoom.draw ();
                }

                if ( ! roomAtNorth.empty() )
                {
                        std::pair< int, int > secondRoomOffset = miniatureOfRoom.calculatePositionOfConnectedMiniature( "north", 0 );
                        Miniature miniatureOfConnectedRoom( * mapManager.getOrBuildRoomByFile( roomAtNorth ),
                                                                secondRoomOffset.first, secondRoomOffset.second,
                                                                sizeOfTileForMiniature );
                        miniatureOfConnectedRoom.draw ();
                }

                if ( ! roomAtEast.empty() )
                {
                        std::pair< int, int > secondRoomOffset = miniatureOfRoom.calculatePositionOfConnectedMiniature( "east", 0 );
                        Miniature miniatureOfConnectedRoom( * mapManager.getOrBuildRoomByFile( roomAtEast ),
                                                                secondRoomOffset.first, secondRoomOffset.second,
                                                                sizeOfTileForMiniature );
                        miniatureOfConnectedRoom.draw ();
                }

                if ( ! roomAtWest.empty() )
                {
                        std::pair< int, int > secondRoomOffset = miniatureOfRoom.calculatePositionOfConnectedMiniature( "west", 0 );
                        Miniature miniatureOfConnectedRoom( * mapManager.getOrBuildRoomByFile( roomAtWest ),
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
        {
                Color::multiplyWithColor( * view, Color::byName( "gray" ) );
        }

        allegro::Pict::setWhereToDraw( previousWhere );

        return this->view;
}

void Isomot::handleMagicKeys ()
{
        Room* activeRoom = mapManager.getActiveRoom();
        GameManager & gameManager = GameManager::getInstance() ;

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
                gameManager.getGameInfo().addLivesByName( activeRoom->getMediator()->getLabelOfActiveCharacter(), 1 );
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
                gameManager.toggleSceneryDecor ();
                allegro::releaseKey( "b" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "m" ) )
        {
                gameManager.toggleRoomMiniatures ();
                allegro::releaseKey( "m" );
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

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "Pad 8" ) )
        {
                activeRoom->getCamera()->shiftAlongY( - 2 );
        }
        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "Pad 2" ) )
        {
                activeRoom->getCamera()->shiftAlongY( + 2 );
        }
        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "Pad 4" ) )
        {
                activeRoom->getCamera()->shiftAlongX( - 2 );
        }
        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "Pad 6" ) )
        {
                activeRoom->getCamera()->shiftAlongX( + 2 );
        }
        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "Pad 5" ) )
        {
                softCenteringOnRoom = true ;
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

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "d" ) )
        {
                activeRoom->dontDisappearOnJump ();
                allegro::releaseKey( "d" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "r" ) )
        {
                playTuneForScenery( activeRoom->getScenery () );
                activeRoom = mapManager.rebuildRoom();

                allegro::releaseKey( "r" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "j" ) )
        {
                AvatarItemPtr activeCharacter = activeRoom->getMediator()->getActiveCharacter();
                if ( activeCharacter != nilPointer )
                {
                        AvatarItemPtr otherCharacter ;

                        Room* roomWithInactiveCharacter = mapManager.getRoomOfInactiveCharacter();
                        if ( roomWithInactiveCharacter != nilPointer )
                        {
                                otherCharacter = roomWithInactiveCharacter->getMediator()->getActiveCharacter() ;
                        }

                        if ( otherCharacter != nilPointer && roomWithInactiveCharacter != activeRoom )
                        {
                                std::cout << "both characters are in active room \"" << activeRoom->getNameOfRoomDescriptionFile() << "\" via pure magic" << std::endl ;

                                const std::string & nameOfAnotherCharacter = otherCharacter->getLabel() ;

                                int characterX = activeCharacter->getX() ;
                                int characterY = activeCharacter->getY() ;
                                int characterZ = activeCharacter->getZ() + 2 * LayerHeight ;
                                const std::string & orientation = otherCharacter->getOrientation() ;

                                AvatarItemPtr joinedCharacter( new AvatarItem(
                                        ItemDescriptions::descriptions().getDescriptionByLabel( nameOfAnotherCharacter ),
                                        characterX, characterY, characterZ, orientation
                                ) );

                                std::string behavior = "still";
                                if ( nameOfAnotherCharacter == "head" ) behavior = "behavior of Head" ;
                                else if ( nameOfAnotherCharacter == "heels" ) behavior = "behavior of Heels" ;

                                joinedCharacter->setBehaviorOf( behavior );

                                activeRoom->addCharacterToRoom( joinedCharacter, true );
                                joinedCharacter->getBehavior()->changeActivityOfItem( activities::Activity::BeginWayInTeletransport );

                                roomWithInactiveCharacter->removeCharacterFromRoom( *otherCharacter, true );
                                mapManager.removeRoomInPlay( roomWithInactiveCharacter );
                        }
                }

                allegro::releaseKey( "j" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "l" ) )
        {
                if ( gameManager.countFreePlanets() < 5 )
                {
                        if ( activeRoom->getMediator()->findItemByLabel( "crown" ) == nilPointer )
                        {
                                const DescriptionOfItem* chapeauDescription = ItemDescriptions::descriptions().getDescriptionByLabel( "crown" );

                                int x = ( activeRoom->getLimitAt( "south" ) - activeRoom->getLimitAt( "north" ) + chapeauDescription->getWidthX() ) >> 1 ;
                                int y = ( activeRoom->getLimitAt( "west" ) - activeRoom->getLimitAt( "east" ) + chapeauDescription->getWidthY() ) >> 1 ;

                                FreeItemPtr chapeau( new FreeItem( chapeauDescription, x, y, 250, "none" ) );
                                chapeau->setBehaviorOf( "behavior of something special" );
                                activeRoom->addFreeItem( chapeau );
                        }
                }
                else
                {
                        AvatarItemPtr activeCharacter = activeRoom->getMediator()->getActiveCharacter();
                        if ( activeCharacter != nilPointer )
                        {
                                const std::string & nameOfCharacter = activeCharacter->getLabel() ;
                                const std::string & orientation = activeCharacter->getOrientation() ;
                                int teleportedX = 0;
                                int teleportedY = 95;
                                int teleportedZ = 240;

                                AvatarItemPtr teleportedCharacter( new AvatarItem(
                                        ItemDescriptions::descriptions().getDescriptionByLabel( nameOfCharacter ),
                                        teleportedX, teleportedY, teleportedZ,
                                        orientation
                                ) ) ;

                                std::string behaviorOfCharacter = "still";
                                if ( nameOfCharacter == "head" ) behaviorOfCharacter = "behavior of Head";
                                else if ( nameOfCharacter == "heels" ) behaviorOfCharacter = "behavior of Heels";
                                else if ( nameOfCharacter == "headoverheels" ) behaviorOfCharacter = "behavior of Head over Heels";

                                teleportedCharacter->setBehaviorOf( behaviorOfCharacter );

                                std::string nameOfRoomNearFinal = "blacktooth83tofreedom.xml";
                                Room* roomWithTeleportToFinalScene = mapManager.getRoomThenAddItToRoomsInPlay( nameOfRoomNearFinal, true );
                                roomWithTeleportToFinalScene->addCharacterToRoom( teleportedCharacter, true );
                                teleportedCharacter->getBehavior()->changeActivityOfItem( activities::Activity::BeginWayInTeletransport );

                                activeRoom->removeCharacterFromRoom( *activeCharacter, true );

                                mapManager.setActiveRoom( roomWithTeleportToFinalScene );
                                mapManager.removeRoomInPlay( activeRoom );

                                roomWithTeleportToFinalScene->activate();
                                activeRoom = roomWithTeleportToFinalScene;
                        }
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
        Room* activeRoom = mapManager.getActiveRoom();
        assert( activeRoom != nilPointer );
        Mediator* mediator = activeRoom->getMediator();
        assert( mediator != nilPointer );

        if ( ! this->finalRoomBuilt )
        {
                mediator->endUpdate();

                std::string arrivedCharacter = mediator->getActiveCharacter()->getOriginalLabel();

                activeRoom->removeCharacterFromRoom( * mediator->getActiveCharacter(), true );

                std::cout << "character \"" << arrivedCharacter << "\" arrived to the final room" << std::endl ;

                const DescriptionOfItem* arrived = ItemDescriptions::descriptions().getDescriptionByLabel( arrivedCharacter );

                if ( arrived != nilPointer )
                {
                        FreeItemPtr character( new FreeItem( arrived, 66, 92, Top, "south" ) );
                        activeRoom->addFreeItem( character );
                }

                activeRoom->getCamera()->instantCenterRoom ();

                // crea las coronas

                unsigned int crowns = 0;

                const DescriptionOfItem* descriptionOfChapeau = ItemDescriptions::descriptions().getDescriptionByLabel( "crown" );

                GameManager & gameManager = GameManager::getInstance() ;

                // la corona de Safari
                if ( gameManager.isFreePlanet( "safari" ) )
                {
                        FreeItemPtr chapeau( new FreeItem( descriptionOfChapeau, 66, 75, Top, "none" ) );
                        activeRoom->addFreeItem( chapeau );
                        crowns++;
                }
                // la corona de Egyptus
                if ( gameManager.isFreePlanet( "egyptus" ) )
                {
                        FreeItemPtr chapeau( new FreeItem( descriptionOfChapeau, 66, 59, Top, "none" ) );
                        activeRoom->addFreeItem( chapeau );
                        crowns++;
                }
                // la corona de Penitentiary
                if ( gameManager.isFreePlanet( "penitentiary" ) )
                {
                        FreeItemPtr chapeau( new FreeItem( descriptionOfChapeau, 65, 107, Top, "none" ) );
                        activeRoom->addFreeItem( chapeau );
                        crowns++;
                }
                // la corona de Byblos
                if ( gameManager.isFreePlanet( "byblos" ) )
                {
                        FreeItemPtr chapeau( new FreeItem( descriptionOfChapeau, 65, 123, Top, "none" ) );
                        activeRoom->addFreeItem( chapeau );
                        crowns++;
                }
                // la corona de Blacktooth
                if ( gameManager.isFreePlanet( "blacktooth" ) )
                {
                        FreeItemPtr chapeau( new FreeItem( descriptionOfChapeau, 65, 91, Top, "none" ) );
                        activeRoom->addFreeItem( chapeau );
                        crowns++;
                }

                mediator->beginUpdate();

                // it's time to count the crowns
                gameManager.inFreedomWithSoManyCrowns( crowns );

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
                                ItemDescriptions::descriptions().getDescriptionByLabel( "ball" ),
                                146, 93, LayerHeight, "none"
                        ) );
                        finalBall->setBehaviorOf( "behaivor of final ball" );
                        activeRoom->addFreeItem( finalBall );

                        finalRoomTimer->reset();
                }
        }
}

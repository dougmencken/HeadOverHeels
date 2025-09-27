
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
#include "AvatarItem.hpp"
#include "ItemDescriptions.hpp"
#include "Behavior.hpp"
#include "Bonus.hpp"

#include "UnlikelyToHappenException.hpp"


Isomot::Isomot( ) :
        view( nilPointer ),
        paused( false ),
        finalRoomBuilt( false ),
        sizeOfTileForMiniature( Miniature::the_default_size_of_tile ),
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
        delete this->view ;
        this->view = nilPointer ;

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

        // image where the isometric view is drawn
        if ( view == nilPointer )
                this->view = new Picture( GamePreferences::getScreenWidth(), GamePreferences::getScreenHeight() );
        else
                this->view->fillWithColor( Color::byName( "orange" ) );

        GameMap::getInstance().binRoomsInPlay();
}

void Isomot::beginNewGame ()
{
        prepare ();

        // bin taken bonuses
        BonusManager::getInstance().clearAbsentBonuses () ;

        GameMap::getInstance().beginNewGame( GameManager::getInstance().getHeadRoom(), GameManager::getInstance().getHeelsRoom() );

        std::cout << "playing the new game" << std::endl ;
        SoundManager::getInstance().playOgg ( "music/begin.ogg", /* loop */ false );
}

void Isomot::offRecording ()
{
        if ( GameManager::getInstance().recordingCaptures () )
                GameManager::getInstance().toggleRecordingCaptures ();
}

void Isomot::offVidasInfinitas ()
{
        if ( GameManager::getInstance().areLivesInexhaustible () )
                GameManager::getInstance().toggleInfiniteLives ();
}

void Isomot::offInviolability ()
{
        if ( GameManager::getInstance().isImmuneToCollisionsWithMortalItems () )
                GameManager::getInstance().toggleImmunityToCollisionsWithMortalItems ();
}

void Isomot::pauseMe ()
{
        IF_DEBUG( std::cout << "Isomot::pauseMe ()" << std::endl )

        if ( this->paused ) return ;

        Room * activeRoom = GameMap::getInstance().getActiveRoom() ;
        if ( activeRoom != nilPointer )
                activeRoom->deactivate() ;

        this->paused = true ;
}

void Isomot::resumeMe ()
{
        IF_DEBUG( std::cout << "Isomot::resumeMe ()" << std::endl )

        if ( ! this->paused ) return ;

        Room * activeRoom = GameMap::getInstance().getActiveRoom() ;
        if ( activeRoom != nilPointer )
                activeRoom->activate() ;

        this->paused = false ;
}

Picture* Isomot::updateMe ()
{
        if ( this->view == nilPointer ) return nilPointer ;

        const allegro::Pict& previousWhere = allegro::Pict::getWhereToDraw() ;
        allegro::Pict::setWhereToDraw( view->getAllegroPict() );

        if ( this->chequerboard == nilPointer ) {
                chequerboard = PicturePtr( new Picture( view->getWidth(), view->getHeight() ) );
                chequerboard->fillWithTransparencyChequerboard ();
        }

        const GameManager & gameManager = GameManager::getInstance() ;

        if ( this->drawOnChequerboard ) {
                allegro::bitBlit( chequerboard->getAllegroPict(), view->getAllegroPict() );
        }
        else {
                Color backgroundColor = ( ! gameManager.charactersFly() ) ? Color::blackColor() : Color::byName( "dark blue" ) ;
                view->fillWithColor( backgroundColor );
        }

        handleMagicKeys ();

        GameMap & map = GameMap::getInstance () ;

        Room* activeRoom = map.getActiveRoom() ;
        if ( activeRoom == nilPointer ) return this->view ;

        // la sala final es muy especial
        if ( activeRoom->getNameOfRoomDescriptionFile() == "finalroom.xml" )
        {
                updateFinalRoom() ;
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

        if ( gameManager.isSimpleGraphicsSet() ) {
                const Color & roomColor = Color::byName( activeRoom->getColor() );
                activeRoom->getWhereToDraw()->multiplyWithColor( roomColor );
        }

        allegro::Pict::setWhereToDraw( view->getAllegroPict() );

        allegro::drawSprite (
                activeRoom->getWhereToDraw()->getAllegroPict(),
                - cameraDeltaX, - cameraDeltaY
        );

        drawMiniature( 24, 24, this->sizeOfTileForMiniature ) ;

        // show text when the infinite lives and inviolability cheats are enabled

        if ( gameManager.areLivesInexhaustible () )
        {
                static const std::string infiniteLives = "VIDAS INFINITAS" ;
                allegro::textOut( infiniteLives, view->getWidth() - ( 8 * infiniteLives.length() ) - 16, 10, Color::whiteColor().toAllegroColor() );
        }

        if ( gameManager.isImmuneToCollisionsWithMortalItems () )
        {
                static const std::string immunity = "INVIOLABILITY" ;
                allegro::textOut( immunity, ( view->getWidth() - ( immunity.length() * 8 ) ) >> 1, 10, Color::whiteColor().toAllegroColor() );
        }

        if ( paused )
                view->multiplyWithColor( Color::byName( "gray" ) );

        allegro::Pict::setWhereToDraw( previousWhere );

        return this->view ;
}

void Isomot::drawMiniature ( int leftX, int topY, unsigned int sizeOfTile )
{
        if ( ! GameManager::getInstance().drawRoomMiniatures () ) return ;

        GameMap & map = GameMap::getInstance () ;
        Room * activeRoom = map.getActiveRoom() ;
        if ( activeRoom == nilPointer ) return ;

        bool sameRoom = true ;
        Miniature * ofThisRoom = this->miniatures.getMiniatureByName( "this" );
        if ( ofThisRoom == nilPointer ||
                        ofThisRoom->getRoom().getNameOfRoomDescriptionFile() != activeRoom->getNameOfRoomDescriptionFile()
                        || ofThisRoom->getSizeOfTile() != sizeOfTile )
        {
                ofThisRoom = new Miniature( *activeRoom, sizeOfTile, /* with room info */ true );
                ofThisRoom->setOffsetOnScreen( leftX, topY );
                this->miniatures.setMiniatureForName( "this", ofThisRoom );
                sameRoom = false ;
        }

        const std::vector< std::string > & ways = activeRoom->getConnections()->getConnectedWays () ;
        for ( unsigned int n = 0 ; n < ways.size() ; ++ n ) {
                const std::string & way = ways[ n ] ;
                const std::string & roomThere = activeRoom->getConnections()->getConnectedRoomAt( way );
                if ( ! roomThere.empty () )
                {
                        if ( ! sameRoom ) {
                                std::cout << "hey there’s a miniature connected"
                                                << ( way.find( "via" ) != std::string::npos ? " " : " on " ) << way
                                                << std::endl ;

                                Miniature * ofThatRoom = new Miniature( * map.getOrBuildRoomByFile( roomThere ) );
                                if ( ofThisRoom->connectMiniature( ofThatRoom, way ) )
                                        this->miniatures.setMiniatureForName( way, ofThatRoom );
                                /* else
                                        std::cout << "can’t connect the miniature of room " << ofThisRoom->getRoom().getNameOfRoomDescriptionFile()
                                                        << " and the miniature of adjacent to the \"" << way
                                                        << "\" room " << ofThatRoom->getRoom().getNameOfRoomDescriptionFile()
                                                        << std::endl ; */
                        }

                        Miniature * ofRoomThere = this->miniatures.getMiniatureByName( way ) ;
                        if ( ofRoomThere != nilPointer ) ofRoomThere->draw ();
                }
        }

        // draw the miniature
        if ( ofThisRoom != nilPointer ) ofThisRoom->draw() ;
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

        // [ option / alt ] & [ shift ] & [ e ] to toggle playing of room entry tunes
        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "e" ) )
        {
                gameManager.togglePlayMelodyOfScenery ();

                std::cout << "room entry tunes are " ;
                if ( gameManager.playMelodyOfScenery() ) {
                        std::cout << "on" << std::endl ;
                        playTuneForScenery( activeRoom->getScenery () );
                } else
                        std::cout << "off" << std::endl ;

                allegro::releaseKey( "e" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "s" ) )
        {
                if ( activeRoom->getMediator()->getActiveCharacter() != nilPointer )
                        gameManager.eatFish( * activeRoom->getMediator()->getActiveCharacter(), activeRoom->getNameOfRoomDescriptionFile() );

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
                                std::cout << "both characters suddenly found themselves together"
                                                << " in the active room " << activeRoom->getNameOfRoomDescriptionFile() << std::endl ;

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
                                joinedCharacter->getBehavior()->setCurrentActivity( activities::Activity::EndTeletransportation, Motion2D::rest() );

                                roomWithInactiveCharacter->removeCharacterFromRoom( *otherCharacter, true );
                                map.removeRoomInPlay( roomWithInactiveCharacter );
                        }
                }

                allegro::releaseKey( "j" );
        }

        // [ option / alt ] & [ shift ] & [ h ] for a hyperspace jump to a random room on the game map
        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "h" ) )
        {
                AvatarItemPtr activeCharacterInOldRoom = activeRoom->getMediator()->getActiveCharacter() ;
                if ( activeCharacterInOldRoom != nilPointer )
                {
                        std::vector< std::string > allRooms ;
                        map.getAllRoomFiles( allRooms );

                        const std::string & randomRoom = allRooms[ rand() % allRooms.size() ] ;
                        std::cout << "hyper-jumping to a lucky map \"" << randomRoom << "\"" << std::endl ;

                        // move the character to the top of old room
                        const int topOfRoom = Room::LayerHeight * ( Room::MaxLayers - 1 ) ;
                        activeCharacterInOldRoom->setZ( topOfRoom );
                        if ( ! gameManager.charactersFly() )
                                gameManager.setCharactersFly( true );

                        Room* newRoom = map.getOrBuildRoomByFile( randomRoom );
                        assert( newRoom != nilPointer );

                        int entryX = newRoom->getXCenterForItem( activeCharacterInOldRoom->getWidthX() );
                        int entryY = newRoom->getYCenterForItem( activeCharacterInOldRoom->getWidthY() );

                        // enter that random room
                        map.changeRoom( randomRoom, "via teleport", false, entryX, entryY, -1 );
                        activeRoom = map.getActiveRoom() ;

                        AvatarItemPtr activeCharacterInNewRoom = activeRoom->getMediator()->getActiveCharacter() ;
                        if ( activeCharacterInNewRoom == nilPointer )
                                throw UnlikelyToHappenException( "character \"" + activeCharacterInOldRoom->getOriginalKind()
                                                        + "\" can’t hyperjump to a new random room" ) ;

                        // move the character to the top of new room
                        activeCharacterInNewRoom->setZ( topOfRoom );

                        activeCharacterInNewRoom->getBehavior()->setCurrentActivity( activities::Activity::EndTeletransportation, Motion2D::rest() );
                        activeCharacterInNewRoom->getBehavior()->update() ;
                }

                allegro::releaseKey( "h" );
        }

        // [ option / alt ] & [ shift ] & [ t ] to instantly give characters their tools,
        //      horn (with box of doughnuts) and handbag
        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "t" ) )
        {
                AvatarItemPtr activeCharacter = activeRoom->getMediator()->getActiveCharacter();
                if ( activeCharacter != nilPointer ) {
                        AvatarItem & dude = * activeCharacter ;

                        if ( dude.isHead() || dude.isHeadOverHeels() ) {
                                dude.takeMagicTool( "horn" );
                                dude.addDonuts( behaviors::Bonus::DonutsPerBox );
                        }
                        if ( dude.isHeels() || dude.isHeadOverHeels() ) {
                                dude.takeMagicTool( "handbag" );
                        }

                        AvatarItemPtr otherCharacter = AvatarItemPtr( ) ;
                        Room* roomWithInactiveCharacter = map.getRoomOfInactiveCharacter() ;
                        if ( roomWithInactiveCharacter != nilPointer )
                                otherCharacter = roomWithInactiveCharacter->getMediator()->getActiveCharacter() ;
                        else
                                otherCharacter = activeRoom->getMediator()->getWaitingCharacter() ;

                        if ( otherCharacter != nilPointer ) {
                                AvatarItem & otherDude = * otherCharacter ;

                                if ( otherDude.isHeadOverHeels() )
                                        throw UnlikelyToHappenException( "and how did you get the waiting or inactive composite character?" ) ;

                                if ( otherDude.isHeels() ) {
                                        otherDude.takeMagicTool( "handbag" );
                                }
                                if ( otherDude.isHead() ) {
                                        otherDude.takeMagicTool( "horn" );
                                        otherDude.addDonuts( behaviors::Bonus::DonutsPerBox );
                                }
                        }
                }

                allegro::releaseKey( "t" );
        }

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
                                teleportedCharacter->getBehavior()->setCurrentActivity( activities::Activity::EndTeletransportation, Motion2D::rest() );

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

        if ( allegro::isShiftKeyPushed() && ( allegro::isKeyPushed( "Pad -" ) || allegro::isKeyPushed( "," ) ) )
        {
                if ( sizeOfTileForMiniature > 2 ) sizeOfTileForMiniature -- ;

                if ( allegro::isKeyPushed( "," ) ) allegro::releaseKey( "," );
                if ( allegro::isKeyPushed( "Pad -" ) ) allegro::releaseKey( "Pad -" );
        }
        if ( allegro::isShiftKeyPushed() && ( allegro::isKeyPushed( "Pad +" ) || allegro::isKeyPushed( "." ) ) )
        {
                if ( sizeOfTileForMiniature < 10 ) sizeOfTileForMiniature ++ ;

                if ( allegro::isKeyPushed( "." ) ) allegro::releaseKey( "." );
                if ( allegro::isKeyPushed( "Pad +" ) ) allegro::releaseKey( "Pad +" );
        }

        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "/" ) )
        {
                drawOnChequerboard = ! drawOnChequerboard ;
                allegro::releaseKey( "/" );
        }
}

void Isomot::playTuneForScenery ( const std::string& scenery )
{
        if ( scenery != "" && GameManager::getInstance().playMelodyOfScenery () )
                SoundManager::getInstance().playOgg ( "music/" + scenery + ".ogg", /* loop */ false );
}

void Isomot::updateFinalRoom ()
{
        Room* finalRoom = GameMap::getInstance().getActiveRoom() ;
        assert( finalRoom != nilPointer );
        Mediator* mediator = finalRoom->getMediator() ;
        assert( mediator != nilPointer );

        if ( ! this->finalRoomBuilt )
        {
                mediator->endUpdating ();

                std::string arrivedCharacter = mediator->getActiveCharacter()->getOriginalKind();

                finalRoom->removeCharacterFromRoom( * mediator->getActiveCharacter(), true );

                std::cout << "character \"" << arrivedCharacter << "\" arrived to the final room" << std::endl ;

                const DescriptionOfItem* arrived = ItemDescriptions::descriptions().getDescriptionByKind( arrivedCharacter );
                if ( arrived != nilPointer ) {
                        FreeItemPtr character( new FreeItem( *arrived, 66, 92, Room::FloorZ, "south" ) );
                        finalRoom->addFreeItem( character );
                }

                finalRoom->getCamera()->instantCenterRoom ();

                // crea las coronas

                unsigned int crowns = 0;

                const DescriptionOfItem & descriptionOfChapeau = * ItemDescriptions::descriptions().getDescriptionByKind( "crown" );

                GameManager & gameManager = GameManager::getInstance() ;

                // la corona de Safari
                if ( gameManager.isFreePlanet( "safari" ) )
                {
                        FreeItemPtr chapeau( new FreeItem( descriptionOfChapeau, 66, 75, Room::FloorZ ) );
                        finalRoom->addFreeItem( chapeau );
                        crowns ++ ;
                }
                // la corona de Egyptus
                if ( gameManager.isFreePlanet( "egyptus" ) )
                {
                        FreeItemPtr chapeau( new FreeItem( descriptionOfChapeau, 66, 59, Room::FloorZ ) );
                        finalRoom->addFreeItem( chapeau );
                        crowns ++ ;
                }
                // la corona de Penitentiary
                if ( gameManager.isFreePlanet( "penitentiary" ) )
                {
                        FreeItemPtr chapeau( new FreeItem( descriptionOfChapeau, 65, 107, Room::FloorZ ) );
                        finalRoom->addFreeItem( chapeau );
                        crowns ++ ;
                }
                // la corona de Byblos
                if ( gameManager.isFreePlanet( "byblos" ) )
                {
                        FreeItemPtr chapeau( new FreeItem( descriptionOfChapeau, 65, 123, Room::FloorZ ) );
                        finalRoom->addFreeItem( chapeau );
                        crowns ++ ;
                }
                // la corona de Blacktooth
                if ( gameManager.isFreePlanet( "blacktooth" ) )
                {
                        FreeItemPtr chapeau( new FreeItem( descriptionOfChapeau, 65, 91, Room::FloorZ ) );
                        finalRoom->addFreeItem( chapeau );
                        ++ crowns ;
                }

                mediator->beginUpdating ();

                // it's time to count the crowns
                gameManager.inFreedomWithSoManyCrowns( crowns );

                this->finalRoomBuilt = true ;
                std::cout << "final room is okay" << std::endl ;
                finalRoomTimer.go() ;

                SoundManager::getInstance().playOgg ( "music/freedom.ogg", /* loop */ true );
        }
        else
        {
                if ( finalRoomTimer.getValue() > 4.0 /* each 4 seconds */ )
                {
                        FreeItemPtr finalBall( new FreeItem (
                                * ItemDescriptions::descriptions().getDescriptionByKind( "ball" ),
                                146, 93, Room::LayerHeight, "none"
                        ) );
                        finalBall->setBehaviorOf( "behaivor of final ball" );
                        finalRoom->addFreeItem( finalBall );

                        finalRoomTimer.go() ; // restart the timer
                }
        }
}

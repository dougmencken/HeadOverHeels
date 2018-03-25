
#include "MapManager.hpp"
#include "Ism.hpp"
#include "Isomot.hpp"
#include "RoomBuilder.hpp"
#include "PlayerItem.hpp"
#include "Door.hpp"
#include "Behavior.hpp"
#include "Mediator.hpp"
#include "Camera.hpp"
#include "SoundManager.hpp"
#include "GameManager.hpp"

# include <xercesc/util/PlatformUtils.hpp>


namespace isomot
{

MapManager::MapManager( Isomot* isomot, const std::string& fileName )
        : isomot( isomot )
        , activeRoom( nilPointer )
        , fileName( fileName )
{

}

MapManager::~MapManager( )
{

}

void MapManager::loadMap ()
{
        xercesc::XMLPlatformUtils::Initialize ();

        try
        {
                // read from XML file
                std::auto_ptr< mxml::MapXML > mapXML( mxml::map( ( isomot::sharePath() + "map" + pathSeparator + fileName ).c_str () ) );

                for ( mxml::MapXML::room_const_iterator i = mapXML->room().begin (); i != mapXML->room().end (); ++i )
                {
                        MapRoomData* roomData = new MapRoomData( ( *i ).file() );

                        if ( ( *i ).north().present() )
                                roomData->setNorth( ( *i ).north().get() );             // connection on north

                        if ( ( *i ).south().present() )
                                roomData->setSouth( ( *i ).south().get() );             // connection on south

                        if ( ( *i ).east().present() )
                                roomData->setEast( ( *i ).east().get() );               // connection on east

                        if ( ( *i ).west().present() )
                                roomData->setWest( ( *i ).west().get() );               // connection on west

                        if ( ( *i ).floor().present() )
                                roomData->setFloor( ( *i ).floor().get() );             // connection on bottom

                        if ( ( *i ).roof().present() )
                                roomData->setRoof( ( *i ).roof().get() );               // connection on top

                        if ( ( *i ).teleport().present() )
                                roomData->setTeleport( ( *i ).teleport().get() );       // connection via teleport

                        if ( ( *i ).teleport2().present() )
                                roomData->setTeleportToo( ( *i ).teleport2().get() );   // connection via second teleport

                        if ( ( *i ).north_east().present() )
                                roomData->setNorthEast( ( *i ).north_east().get() );    // north-east connection

                        if ( ( *i ).north_west().present() )
                                roomData->setNorthWest( ( *i ).north_west().get() );    // north-west connection

                        if ( ( *i ).south_east().present() )
                                roomData->setSouthEast( ( *i ).south_east().get() );    // south-east connection

                        if ( ( *i ).south_west().present() )
                                roomData->setSouthWest( ( *i ).south_west().get() );    // south-west connection

                        if ( ( *i ).east_north().present() )
                                roomData->setEastNorth( ( *i ).east_north().get() );    // east-north connection

                        if ( ( *i ).east_south().present() )
                                roomData->setEastSouth( ( *i ).east_south().get() );    // east-south connection

                        if ( ( *i ).west_north().present() )
                                roomData->setWestNorth( ( *i ).west_north().get() );    // west-north connection

                        if ( ( *i ).west_south().present() )
                                roomData->setWestSouth( ( *i ).west_south().get() );    // west-south connection

                        // add data for this room to the list
                        this->theMap.push_back( roomData );
                }
        }
        catch ( const xml_schema::exception& e )
        {
                std::cerr << e << std::endl ;
        }
        catch ( const std::exception& e )
        {
                std::cerr << e.what() << std::endl ;
        }

        xercesc::XMLPlatformUtils::Terminate ();
}

void MapManager::beginNewGame( const std::string& firstRoomFileName, const std::string& secondRoomFileName )
{
        resetVisitedRooms();

        GameManager::getInstance()->setHeadLives( 8 );
        GameManager::getInstance()->setHeelsLives( 8 );

        // get data of first room
        MapRoomData* firstRoomData = findRoomData( firstRoomFileName );

        // create first room
        if ( firstRoomData != nilPointer )
        {
                std::auto_ptr< RoomBuilder > roomBuilder( new RoomBuilder( isomot->getItemDataManager() ) );
                Room* firstRoom = roomBuilder->buildRoom( isomot::sharePath() + "map" + pathSeparator + firstRoomFileName );

                if ( firstRoom != nilPointer )
                {
                        ItemData* firstPlayerData = isomot->getItemDataManager()->findDataByLabel( "head" );

                        int centerX = RoomBuilder::getXCenterOfRoom( firstPlayerData, firstRoom );
                        int centerY = RoomBuilder::getYCenterOfRoom( firstPlayerData, firstRoom );

                        // create player Head
                        roomBuilder->createPlayerInTheSameRoom( true, "head", centerX, centerY, 0, West );

                        firstRoomData->setVisited( true );

                        firstRoom->activateCharacterByLabel( "head" );

                        firstRoom->getCamera()->turnOn( firstRoom->getMediator()->getActiveCharacter(), JustWait );
                        activeRoom = firstRoom;
                        rooms.push_back( firstRoom );
                }
        }

        // get data of second room
        MapRoomData* secondRoomData = findRoomData( secondRoomFileName );

        // create second room
        if ( secondRoomData != nilPointer )
        {
                std::auto_ptr< RoomBuilder > roomBuilder( new RoomBuilder( isomot->getItemDataManager() ) );
                Room* secondRoom = roomBuilder->buildRoom( isomot::sharePath() + "map" + pathSeparator + secondRoomFileName );

                if ( secondRoom != nilPointer )
                {
                        ItemData* secondPlayerData = isomot->getItemDataManager()->findDataByLabel( "heels" );

                        int centerX = RoomBuilder::getXCenterOfRoom( secondPlayerData, secondRoom );
                        int centerY = RoomBuilder::getYCenterOfRoom( secondPlayerData, secondRoom );

                        // create player Heels
                        roomBuilder->createPlayerInTheSameRoom( true, "heels", centerX, centerY, 0, South );

                        secondRoomData->setVisited( true );

                        secondRoom->activateCharacterByLabel( "heels" );

                        secondRoom->getCamera()->turnOn( secondRoom->getMediator()->getActiveCharacter(), JustWait );
                        rooms.push_back( secondRoom );
                }
        }
}

void MapManager::beginOldGameWithCharacter( const sgxml::player& data )
{
        MapRoomData* roomData = findRoomData( data.roomFilename() );
        Room* room = nilPointer;

        if ( roomData != nilPointer )
        {
                // if there is already created room it is when room of second player is the same as of first player
                if ( this->activeRoom != nilPointer && this->activeRoom->getNameOfFileWithDataAboutRoom() == roomData->getNameOfRoomFile() )
                {
                        room = this->activeRoom;
                }
                else
                {
                        std::auto_ptr< RoomBuilder > roomBuilder( new RoomBuilder( isomot->getItemDataManager() ) );
                        room = roomBuilder->buildRoom ( isomot::sharePath() + "map" + pathSeparator + data.roomFilename() );
                }

                // place character in room
                if ( room != nilPointer )
                {
                        std::string nameOfCharacter = data.label();

                        // create player
                        std::auto_ptr< RoomBuilder > roomBuilder( new RoomBuilder( isomot->getItemDataManager() ) );
                        PlayerItem* player = roomBuilder->createPlayerInRoom(
                                                                room,
                                                                true,
                                                                nameOfCharacter,
                                                                data.x (), data.y (), data.z (),
                                                                Way( data.direction() ), Way( data.entry() ) );

                        roomData->setVisited( true );

                        Way entry( data.entry() );
                        if ( entry.toString() == "just wait" )
                        {
                                // it’s the case of resume of saved game
                                // show bubbles only for active player
                                if ( data.active() )
                                {
                                        entry = ByTeleportToo;
                                }
                        }

                        // change activity of player by way of entry
                        player->autoMoveOnEntry( Way( entry ).toString() );

                        // for active player or for other player when it is created in another room
                        // when other player is in the same room as active player then there’s no need to do anything more
                        if ( data.active() || this->activeRoom != room )
                        {
                                room->activateCharacterByLabel( nameOfCharacter );
                                room->getCamera()->turnOn( room->getMediator()->getActiveCharacter(), Way( data.entry() ) );
                                //// room->getCamera()->centerOn( room->getMediator()->getActiveCharacter () );

                                if ( data.active() )
                                {
                                        // for active player this room is active one
                                        this->activeRoom = room;
                                }

                                this->rooms.push_back( room );
                        }
                }
        }
}

void MapManager::binEveryRoom()
{
        // bin rooms
        if ( this->rooms.size () == 2 &&
                this->rooms[ 0 ]->getNameOfFileWithDataAboutRoom() == this->rooms[ 1 ]->getNameOfFileWithDataAboutRoom() )
        {
                delete this->rooms[ 0 ];
        }
        else
        {
                std::for_each( this->rooms.begin (), this->rooms.end (), DeleteObject() );
        }

        this->rooms.clear();

        this->activeRoom = 0;
}

Room* MapManager::changeRoom( const Way& wayOfExit )
{
        Room* previousRoom = this->activeRoom;
        previousRoom->setWayOfExit( "no exit" );

        // get data of previous room
        MapRoomData* previousRoomData = findRoomData( previousRoom->getNameOfFileWithDataAboutRoom() );

        Way wayOfEntry( JustWait ) ;

        // search the map for next room and get way of entry to it
        MapRoomData* nextRoomData = findRoomData( previousRoomData->findConnectedRoom( wayOfExit, &wayOfEntry ) );

        if ( nextRoomData == nilPointer )
        {
                // no room there, so continue with current one
                return previousRoom ;
        }

        activeRoom->deactivate();
        this->activeRoom = nilPointer;

        nextRoomData->adjustEntry( &wayOfEntry, previousRoomData->getNameOfRoomFile() );

        SoundManager::getInstance()->stopEverySound ();

        PlayerItem* oldItemOfRoamer = previousRoom->getMediator()->getActiveCharacter( );

        std::string nameOfRoamer = oldItemOfRoamer->getOriginalLabel() ; // current label may be "bubbles" when teleporting

        std::cout << "\"" << nameOfRoamer << "\" migrates"
                        << " from room \"" << previousRoomData->getNameOfRoomFile() << "\" with way of exit \"" << wayOfExit.toString() << "\""
                        << " to room \"" << nextRoomData->getNameOfRoomFile() << "\" with way of entry \"" << wayOfEntry.toString() << "\"" << std::endl ;

        const int exitX = oldItemOfRoamer->getX ();
        const int exitY = oldItemOfRoamer->getY ();
        const int exitZ = oldItemOfRoamer->getZ ();

        const Way exitOrientation = oldItemOfRoamer->getOrientation ();

        // get limits of room
        int northBound = previousRoom->getLimitAt( "north" );
        int eastBound = previousRoom->getLimitAt( "east" );
        int southBound = previousRoom->getLimitAt( "south" );
        int westBound = previousRoom->getLimitAt( "west" );
        // plus there’s possibility to exit and to enter room via floor, roof or teletransport

        // remove active player from previous room
        previousRoom->removePlayerFromRoom( oldItemOfRoamer, true );
        oldItemOfRoamer = nilPointer;

        if ( ! previousRoom->isAnyPlayerStillInRoom() || nameOfRoamer == "headoverheels" )
        {
                std::cout << "there’re no players left in room \"" << previousRoom->getNameOfFileWithDataAboutRoom() << "\""
                                << " so bye that room" << std::endl ;

                removeRoom( previousRoom );
                previousRoom = nilPointer;
        }

        Room* newRoom = findRoom( nextRoomData->getNameOfRoomFile() );

        if ( newRoom != nilPointer )
        {
                std::cout << "room \"" << newRoom->getNameOfFileWithDataAboutRoom() << "\" is already created" << std::endl ;
                newRoom->setWayOfExit( "no exit" );
        }
        else
        {
                std::cout << "going to create room \"" << nextRoomData->getNameOfRoomFile() << "\"" << std::endl ;

                std::auto_ptr< RoomBuilder > roomBuilder( new RoomBuilder( isomot->getItemDataManager() ) );
                newRoom = roomBuilder->buildRoom( isomot::sharePath() + "map" + pathSeparator + nextRoomData->getNameOfRoomFile() );
                rooms.push_back( newRoom );
        }

        ItemData* dataOfRoamer = isomot->getItemDataManager()->findDataByLabel( nameOfRoamer );

        // get player’s exit position in old room to calculate entry position in new room
        int entryX = exitX ;
        int entryY = exitY ;
        int entryZ = exitZ ;

        std::cout << "exit coordinates are x=" << exitX << " y=" << exitY << " z=" << exitZ << std::endl ;
        newRoom->calculateEntryCoordinates( wayOfEntry, dataOfRoamer->getWidthX(), dataOfRoamer->getWidthY(), northBound, eastBound, southBound, westBound, &entryX, &entryY, &entryZ );
        std::cout << "entry coordinates are x=" << entryX << " y=" << entryY << " z=" << entryZ << std::endl ;

        // create player
        std::auto_ptr< RoomBuilder > roomBuilder( new RoomBuilder( isomot->getItemDataManager() ) );
        if ( wayOfEntry.toString() == "via teleport" || wayOfEntry.toString() == "via second teleport" )
        {
                entryZ = Top;
        }

        PlayerItem* newItemOfRoamer = roomBuilder->createPlayerInRoom( newRoom, true, nameOfRoamer, entryX, entryY, entryZ, exitOrientation, wayOfEntry );

        if ( newItemOfRoamer != nilPointer )
        {
                newItemOfRoamer->autoMoveOnEntry( wayOfEntry.toString() );
        }

        nextRoomData->setVisited( true );

        newRoom->activateCharacterByLabel( nameOfRoamer );
        newRoom->getCamera()->turnOn( newRoom->getMediator()->getActiveCharacter(), wayOfEntry );

        activeRoom = newRoom;
        activeRoom->activate();

        return newRoom;
}

Room* MapManager::rebuildRoom()
{
        activeRoom->deactivate();
        Room* oldRoom = activeRoom;

        MapRoomData* oldRoomData = findRoomData( oldRoom->getNameOfFileWithDataAboutRoom() );

        // rebuild room by data from map
        std::auto_ptr< RoomBuilder > roomBuilder( new RoomBuilder( isomot->getItemDataManager() ) );
        Room* newRoom = roomBuilder->buildRoom ( isomot::sharePath() + "map" + pathSeparator + oldRoomData->getNameOfRoomFile() );

        std::string nameOfActivePlayer = oldRoom->getMediator()->getLabelOfActiveCharacter();
        std::string nameOfActivePlayerBeforeJoining = oldRoom->getMediator()->getLastActiveCharacterBeforeJoining();

        Way theWay = Nowhere;
        PlayerItem* alivePlayer = nilPointer;

        // for each player entered this room
        std::list < const PlayerItem * > playersOnEntry = oldRoom->getPlayersWhoEnteredRoom ();
        for ( std::list< const PlayerItem * >::const_iterator it = playersOnEntry.begin (); it != playersOnEntry.end (); )
        {
                const PlayerItem* player = *it;

                if ( player->getLabel() == "headoverheels" || GameManager::getInstance()->getLives( player->getLabel() ) > 0 )
                {
                #ifdef DEBUG
                        std::cout << "got player \"" << player->getLabel() << "\" who entered this room @ MapManager::rebuildRoom" << std::endl ;
                #endif

                        // when joined character splits, then some simple character migrates to another room
                        // and further via swap user changes to room of splitting, and loses life there
                        // then don’t rebuild room with headoverheels but with character left in this room

                        if ( player->getLabel() == "headoverheels" && rooms.size() > 1 )
                        {
                                std::cout << "some character migrated to another room, and only \"" << nameOfActivePlayer << "\" is still here" << std::endl ;

                                int playerX = player->getX();
                                int playerY = player->getY();
                                int playerZ = player->getZ();
                                Way playerOrientation = player->getOrientation();
                                Way playerEntry = player->getWayOfEntry();

                                // forget composite player
                                oldRoom->removePlayerFromRoom( oldRoom->getMediator()->getActiveCharacter(), true );

                                // create simple player
                                roomBuilder->createPlayerInRoom( oldRoom, true,
                                                                        nameOfActivePlayer,
                                                                        playerX, playerY, playerZ,
                                                                        playerOrientation, playerEntry );

                                // update list of players and rewind iterator
                                playersOnEntry = oldRoom->getPlayersWhoEnteredRoom ();
                                it = playersOnEntry.begin ();
                                continue;
                        }

                        theWay = player->getOrientation();

                        Way entry = player->getWayOfEntry();

                        // create player
                        alivePlayer = roomBuilder->createPlayerInRoom( newRoom,
                                                                        true,
                                                                        player->getLabel(),
                                                                        player->getX(), player->getY(), player->getZ(),
                                                                        theWay, entry );

                        if ( alivePlayer != nilPointer )
                                alivePlayer->autoMoveOnEntry( entry.toString() );
                }

                it++ ; // next player
        }

        // remove existing room
        removeRoom( oldRoom );

        if ( alivePlayer != nilPointer )
        {
                // add room just created
                rooms.push_back( newRoom );

                bool activePlayerIsHere = newRoom->activateCharacterByLabel( nameOfActivePlayer );

                if ( ! activePlayerIsHere )
                {
                        // case when composite player joined in this room loses life and splits back into two players
                        activePlayerIsHere = newRoom->activateCharacterByLabel( nameOfActivePlayerBeforeJoining );

                        if ( ! activePlayerIsHere ) // unlucky to happen
                        {
                                if ( newRoom->isAnyPlayerStillInRoom() )
                                {
                                        newRoom->getMediator()->setActiveCharacter( * newRoom->getPlayersYetInRoom().begin () );
                                }
                        }
                }

                newRoom->getCamera()->turnOn( newRoom->getMediator()->getActiveCharacter(), theWay );

                activeRoom = newRoom;
                activeRoom->activate();
        }
        // can’t create player, game over
        else
        {
                delete newRoom;
                activeRoom = newRoom = nilPointer;
        }

        return newRoom;
}

Room* MapManager::createRoom( const std::string& fileName )
{
        std::auto_ptr< RoomBuilder > roomBuilder( new RoomBuilder( isomot->getItemDataManager() ) );
        return roomBuilder->buildRoom( isomot::sharePath() + "map" + pathSeparator + fileName );
}

Room* MapManager::createRoomThenAddItToListOfRooms( const std::string& fileName, bool markVisited )
{
        Room* room = createRoom( fileName );
        rooms.push_back( room );

        if ( markVisited )
        {
                MapRoomData* roomData = findRoomData( fileName );
                roomData->setVisited( true );
        }

        return room;
}

Room* MapManager::swapRoom ()
{
        // swap is possible when there are more than one room
        if ( rooms.size() > 1 )
        {
                activeRoom->deactivate();

                SoundManager::getInstance()->stopEverySound ();

                std::string fileOfPreviousRoom = activeRoom->getNameOfFileWithDataAboutRoom() ;

                std::vector< Room* >::iterator i = std::find_if( rooms.begin (), rooms.end (), std::bind2nd( EqualRoom(), activeRoom->getNameOfFileWithDataAboutRoom() ) );
                // get next room to swap with
                ++i;
                // when it’s last one swap with first one
                activeRoom = ( i != rooms.end() ? ( *i ) : *rooms.begin() );

                std::cout << "swop room \"" << fileOfPreviousRoom << "\" with \"" << activeRoom->getNameOfFileWithDataAboutRoom() << "\"" << std::endl ;

                activeRoom->activate();
        }

        return activeRoom;
}

Room* MapManager::removeRoomAndSwap ()
{
        activeRoom->deactivate();
        Room* inactiveRoom = activeRoom;

        std::vector< Room* >::iterator i = std::find_if( rooms.begin (), rooms.end (), std::bind2nd( EqualRoom(), activeRoom->getNameOfFileWithDataAboutRoom() ) );
        // get next room to swap with
        ++i;
        // when it’s last one swap with first one
        activeRoom = ( i != rooms.end() ? ( *i ) : *rooms.begin() );

        // no more rooms, game over
        if ( inactiveRoom == activeRoom )
        {
                activeRoom = nilPointer;
        }

        removeRoom( inactiveRoom );

        if ( activeRoom != nilPointer )
        {
                activeRoom->activate();
        }

        return activeRoom;
}

void MapManager::removeRoom( Room* whichRoom )
{
        if ( whichRoom != nilPointer )
        {
                if ( whichRoom->isActive() )
                        whichRoom->deactivate();

                if ( whichRoom == this->activeRoom )
                        this->activeRoom = nilPointer;

                rooms.erase( std::remove_if( rooms.begin (), rooms.end (), std::bind2nd( EqualRoom(), whichRoom->getNameOfFileWithDataAboutRoom() ) ), rooms.end() );
                delete whichRoom;
        }
}

void MapManager::readVisitedSequence( sgxml::exploredRooms::visited_sequence& visitedSequence )
{
        resetVisitedRooms();

        for ( sgxml::exploredRooms::visited_const_iterator i = visitedSequence.begin (); i != visitedSequence.end (); ++i )
        {
                std::list< MapRoomData * >::iterator m = std::find_if( theMap.begin(), theMap.end(), std::bind2nd( EqualMapRoomData(), ( *i ).filename() ) );
                if ( m != theMap.end() )
                {
                        ( *m )->setVisited( true );
                }
        }
}

void MapManager::storeVisitedSequence( sgxml::exploredRooms::visited_sequence& visitedSequence )
{
        for ( std::list< MapRoomData * >::iterator i = this->theMap.begin (); i != this->theMap.end (); ++i )
        {
                if ( ( *i )->isVisited() )
                {
                        visitedSequence.push_back( sgxml::visited( ( *i )->getNameOfRoomFile() ) );
                }
        }
}

unsigned int MapManager::countVisitedRooms()
{
        unsigned int number = 0;

        for ( std::list< MapRoomData * >::iterator i = this->theMap.begin (); i != this->theMap.end (); ++i )
        {
                number += ( *i )->isVisited() ? 1 : 0;
        }

        return number;
}

void MapManager::resetVisitedRooms()
{
        for ( std::list< MapRoomData * >::iterator i = this->theMap.begin (); i != this->theMap.end (); ++i )
        {
                ( *i )->setVisited( false );
        }
}

MapRoomData* MapManager::findRoomData( const std::string& room ) const
{
        if ( room.empty() )
                return nilPointer;

        /* std::cout << "lookin’ for data about connections of room \"" << room << "\"" << std::endl ; */

        std::list< MapRoomData * >::const_iterator i = std::find_if( theMap.begin (), theMap.end (), std::bind2nd( EqualMapRoomData(), room ) );
        MapRoomData* data = ( i != theMap.end() ? *i : nilPointer );

        return data;
}

Room* MapManager::findRoom( const std::string& room ) const
{
        std::vector< Room* >::const_iterator i = std::find_if( rooms.begin (), rooms.end (), std::bind2nd( EqualRoom(), room ) );
        return ( i != rooms.end() ? *i : nilPointer );
}

Room* MapManager::getRoomOfInactivePlayer() const
{
        for ( size_t i = 0; i < this->rooms.size (); ++i )
        {
                if ( this->rooms[ i ] != this->activeRoom )
                {
                        return this->rooms[ i ];
                }
        }

        return nilPointer;
}

bool EqualMapRoomData::operator()( const MapRoomData* mapData, const std::string& room ) const
{
        return ( mapData->getNameOfRoomFile() == room );
}

bool EqualRoom::operator()( const Room* room, const std::string& nameOfRoom ) const
{
        return ( room->getNameOfFileWithDataAboutRoom() == nameOfRoom );
}

}

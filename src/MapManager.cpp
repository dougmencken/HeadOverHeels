
#include "MapManager.hpp"
#include "Ism.hpp"
#include "Isomot.hpp"
#include "RoomBuilder.hpp"
#include "PlayerItem.hpp"
#include "Door.hpp"
#include "Behavior.hpp"
#include "Mediator.hpp"
#include "Camera.hpp"
#include "Exception.hpp"
#include "SoundManager.hpp"
#include "GameManager.hpp"

# include <xercesc/util/PlatformUtils.hpp>


namespace isomot
{

MapManager::MapManager( Isomot* isomot, const std::string& fileName )
: isomot( isomot ),
  activeRoom( 0 ),
  fileName( fileName )
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
                // read from the XML file
                std::auto_ptr< mxml::MapXML > mapXML( mxml::map( ( isomot::sharePath() + "map/" + fileName ).c_str () ) );

                for ( mxml::MapXML::room_const_iterator i = mapXML->room().begin (); i != mapXML->room().end (); ++i )
                {
                        // Se crea la sala a partir del nombre de su archivo XML
                        std::auto_ptr< MapRoomData > roomData( new MapRoomData( ( *i ).file() ) );

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
                        this->mapData.push_back( *roomData.get() );
                }
        }
        catch ( const xml_schema::exception& e )
        {
                std::cerr << e << std::endl ;
        }
        catch ( const Exception& e )
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
        if ( firstRoomData != 0 )
        {
                std::auto_ptr< RoomBuilder > roomBuilder( new RoomBuilder( isomot->getItemDataManager() ) );
                Room* firstRoom = roomBuilder->buildRoom( isomot::sharePath() + "map/" + firstRoomFileName );

                if ( firstRoom != 0 )
                {
                        ItemData* firstPlayerData = isomot->getItemDataManager()->findItemByLabel( "head" );

                        int centerX = RoomBuilder::getXCenterOfRoom( firstPlayerData, firstRoom );
                        int centerY = RoomBuilder::getYCenterOfRoom( firstPlayerData, firstRoom );

                        // create player Head
                        roomBuilder->createPlayerInTheSameRoom( true, "head", centerX, centerY, 0, false, West );

                        firstRoomData->setVisited( true );

                        firstRoom->activatePlayerByName( "head" );

                        firstRoom->getCamera()->turnOn( firstRoom->getMediator()->getActivePlayer(), JustWait );
                        activeRoom = firstRoom;
                        rooms.push_back( firstRoom );
                }
        }

        // get data of second room
        MapRoomData* secondRoomData = findRoomData( secondRoomFileName );

        // create second room
        if ( secondRoomData != 0 )
        {
                std::auto_ptr< RoomBuilder > roomBuilder( new RoomBuilder( isomot->getItemDataManager() ) );
                Room* secondRoom = roomBuilder->buildRoom( isomot::sharePath() + "map/" + secondRoomFileName );

                if ( secondRoom != 0 )
                {
                        ItemData* secondPlayerData = isomot->getItemDataManager()->findItemByLabel( "heels" );

                        int centerX = RoomBuilder::getXCenterOfRoom( secondPlayerData, secondRoom );
                        int centerY = RoomBuilder::getYCenterOfRoom( secondPlayerData, secondRoom );

                        // create player Heels
                        roomBuilder->createPlayerInTheSameRoom( true, "heels", centerX, centerY, 0, false, South );

                        secondRoomData->setVisited( true );

                        secondRoom->activatePlayerByName( "heels" );

                        secondRoom->getCamera()->turnOn( secondRoom->getMediator()->getActivePlayer(), JustWait );
                        rooms.push_back( secondRoom );
                }
        }
}

void MapManager::beginOldGameWithCharacter( const sgxml::player& data )
{
        MapRoomData* roomData = findRoomData( data.roomFilename() );
        Room* room = 0;

        if ( roomData != 0 )
        {
                // if there is already created room it is when room of second player is the same as of first player
                if ( this->activeRoom != 0 && this->activeRoom->getNameOfFileWithDataAboutRoom().compare( roomData->getNameOfRoomFile() ) == 0 )
                {
                        room = this->activeRoom;
                }
                else
                {
                        std::auto_ptr< RoomBuilder > roomBuilder( new RoomBuilder( isomot->getItemDataManager() ) );
                        room = roomBuilder->buildRoom ( isomot::sharePath() + "map/" + data.roomFilename() );
                }

                // place character in room
                if ( room != 0 )
                {
                        std::string nameOfCharacter = data.label();

                        // create player
                        std::auto_ptr< RoomBuilder > roomBuilder( new RoomBuilder( isomot->getItemDataManager() ) );
                        PlayerItem* player = roomBuilder->createPlayerInRoom(
                                                                room,
                                                                true,
                                                                nameOfCharacter,
                                                                data.x (), data.y (), data.z (),
                                                                false,
                                                                Direction( data.direction() ), Direction( data.entry() ) );

                        roomData->setVisited( true );

                        Direction entry = static_cast< Direction >( data.entry() );
                        if ( entry == JustWait )
                        {
                                // it’s the case of resume of saved game
                                // show bubbles only for active player
                                if ( data.active() )
                                {
                                        entry = ByTeleportToo;
                                }
                        }

                        // change activity of player by way of entry
                        player->autoMoveOnEntry( entry );

                        // for active player or for other player when it is created in another room
                        // when other player is in the same room as active player then there’s no need to do anything more
                        if ( data.active() || this->activeRoom != room )
                        {
                                room->activatePlayerByName( nameOfCharacter );
                                room->getCamera()->turnOn( room->getMediator()->getActivePlayer(), Direction( data.entry() ) );
                                //// room->getCamera()->centerOn( room->getMediator()->getActivePlayer () );

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
                this->rooms[ 0 ]->getNameOfFileWithDataAboutRoom().compare( this->rooms[ 1 ]->getNameOfFileWithDataAboutRoom() ) == 0 )
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

Room* MapManager::changeRoom( const Direction& exit )
{
        activeRoom->deactivate();
        Room* previousRoom = this->activeRoom;
        this->activeRoom = 0;

        // get data of previous room
        MapRoomData* previousRoomData = findRoomData( previousRoom->getNameOfFileWithDataAboutRoom() );

        Direction entry ;

        // search the map for next room and get way of entry to it
        MapRoomData* nextRoomData = findRoomData( previousRoomData->findConnectedRoom( exit, &entry ) );

        if ( nextRoomData == 0 )
        {
                return 0;
        }

        std::cout << "player \"" << previousRoom->getMediator()->getActivePlayer()->getLabel() << "\" migrates"
                        << " from room \"" << previousRoomData->getNameOfRoomFile() << "\" with way of exit " << exit
                        << " to room \"" << nextRoomData->getNameOfRoomFile() << "\" with way of entry " << entry << std::endl ;

        nextRoomData->adjustEntry( &entry, previousRoomData->getNameOfRoomFile() );

        SoundManager::getInstance()->stopEverySound ();

        PlayerItem* oldItemOfRoamer = previousRoom->getMediator()->getActivePlayer( );

        std::string nameOfRoamer = oldItemOfRoamer->getLabel() ;

        const int exitX = oldItemOfRoamer->getX ();
        const int exitY = oldItemOfRoamer->getY ();
        const int exitZ = oldItemOfRoamer->getZ ();

        const Direction exitOrientation = oldItemOfRoamer->getDirection ();

        // get limits of room
        // there’s possibility to exit and to enter new room in cases when player travels through floor, roof or via teletransport
        int northBound = previousRoom->getBound( North );
        int eastBound = previousRoom->getBound( East );
        int southBound = previousRoom->getBound( South );
        int westBound = previousRoom->getBound( West );

        // if player carries some item
        bool withItem = oldItemOfRoamer->consultTakenItemImage() != 0;

        // remove active player from previous room
        previousRoom->removePlayerFromRoom( oldItemOfRoamer, true );
        oldItemOfRoamer = 0;

        if ( ! previousRoom->isAnyPlayerStillInRoom() || nameOfRoamer == "headoverheels" )
        {
                std::cout << "there’re no players left in room \"" << previousRoom->getNameOfFileWithDataAboutRoom() << "\""
                                << " so bye that room" << std::endl ;

                removeRoom( previousRoom );
                previousRoom = 0;
        }

        Room* newRoom = findRoom( nextRoomData->getNameOfRoomFile() );

        if ( newRoom != 0 )
        {
                std::cout << "room \"" << newRoom->getNameOfFileWithDataAboutRoom() << "\" is already created" << std::endl ;

                newRoom->setActive( true );
        }
        else
        {
                std::cout << "going to create room \"" << nextRoomData->getNameOfRoomFile() << "\"" << std::endl ;

                std::auto_ptr< RoomBuilder > roomBuilder( new RoomBuilder( isomot->getItemDataManager() ) );
                newRoom = roomBuilder->buildRoom( isomot::sharePath() + "map/" + nextRoomData->getNameOfRoomFile() );
                rooms.push_back( newRoom );
        }

        ItemData* dataOfRoamer = isomot->getItemDataManager()->findItemByLabel( nameOfRoamer );

        // get player’s exit position in old room to calculate entry position in new room
        int entryX = exitX ;
        int entryY = exitY ;
        int entryZ = exitZ ;

        std::cout << "exit coordinates are x=" << exitX << " y=" << exitY << " z=" << exitZ << std::endl ;
        newRoom->calculateEntryCoordinates( entry, dataOfRoamer->widthX, dataOfRoamer->widthY, northBound, eastBound, southBound, westBound, &entryX, &entryY, &entryZ );
        std::cout << "entry coordinates are x=" << entryX << " y=" << entryY << " z=" << entryZ << std::endl ;

        // create player
        std::auto_ptr< RoomBuilder > roomBuilder( new RoomBuilder( isomot->getItemDataManager() ) );
        if ( entry == ByTeleport || entry == ByTeleportToo )
        {
                entryZ = Top;
        }

        PlayerItem* newItemOfRoamer = roomBuilder->createPlayerInRoom( newRoom, true, nameOfRoamer, entryX, entryY, entryZ, withItem, exitOrientation, entry );

        newItemOfRoamer->autoMoveOnEntry( entry );

        nextRoomData->setVisited( true );

        newRoom->activatePlayerByName( nameOfRoamer );
        newRoom->getCamera()->turnOn( newRoom->getMediator()->getActivePlayer(), entry );

        activeRoom = newRoom;
        activeRoom->activate();

        return newRoom;
}

Room* MapManager::rebuildRoom()
{
        activeRoom->deactivate();

        MapRoomData* activeRoomData = findRoomData( activeRoom->getNameOfFileWithDataAboutRoom() );

        // rebuild room by data from map
        std::auto_ptr< RoomBuilder > roomBuilder( new RoomBuilder( isomot->getItemDataManager() ) );
        Room* newRoom = roomBuilder->buildRoom ( isomot::sharePath() + "map/" + activeRoomData->getNameOfRoomFile() );

        std::string nameOfActivePlayer = activeRoom->getMediator()->getActivePlayer()->getLabel();
        std::string nameOfActivePlayerBeforeJoining = activeRoom->getMediator()->getLastActivePlayerBeforeJoining();

        Direction direction = NoDirection;
        PlayerItem* alivePlayer = 0;

        // for each player entered this room
        std::list < const PlayerItem * > playersOnEntry = activeRoom->getPlayersWhoEnteredRoom ();
        for ( std::list< const PlayerItem * >::const_iterator it = playersOnEntry.begin (); it != playersOnEntry.end (); ++it )
        {
                const PlayerItem* player = *it;

                if ( player->getLabel() == "headoverheels" || GameManager::getInstance()->getLives( player->getLabel() ) > 0 )
                {
                        std::cout << "got player \"" << player->getLabel() << "\" who entered this room @ MapManager::rebuildRoom" << std::endl ;

                        // when joined character splits, then some simple character migrates to another room
                        // further via swap user changes to room of splitting, and loses life there
                        // so room rebuilds as fresh one with headoverheels in it
                        // and as surprise that simple character is still in another room
                        if ( player->getLabel() == "headoverheels" && rooms.size() > 1 )
                        {
                                // get that another room
                                std::vector< Room* >::iterator ri = std::find_if( rooms.begin (), rooms.end (), std::bind2nd( EqualRoom(), activeRoom->getNameOfFileWithDataAboutRoom() ) );
                                ++ri ;
                                Room* roomToForget = ( ri != rooms.end() ? ( *ri ) : *rooms.begin() );

                                // and forget it
                                removeRoom( roomToForget );
                        }

                        direction = player->getDirection();

                        Direction entry = player->getDirectionOfEntry();

                        // create player
                        alivePlayer = roomBuilder->createPlayerInRoom( newRoom,
                                                                        true,
                                                                        player->getLabel(),
                                                                        player->getX(), player->getY(), player->getZ(),
                                                                        false, direction, entry );

                        alivePlayer->autoMoveOnEntry( entry );
                }
        }

        // remove existing room
        removeRoom( activeRoom );

        if ( alivePlayer != 0 )
        {
                // add room just created
                rooms.push_back( newRoom );

                bool activePlayerIsHere = newRoom->activatePlayerByName( nameOfActivePlayer );

                if ( ! activePlayerIsHere )
                {
                        // case when composite player joined in this room loses life and splits back into two players
                        activePlayerIsHere = newRoom->activatePlayerByName( nameOfActivePlayerBeforeJoining );

                        if ( ! activePlayerIsHere ) // unlucky to happen
                        {
                                if ( newRoom->isAnyPlayerStillInRoom() )
                                {
                                        newRoom->getMediator()->setActivePlayer( * newRoom->getPlayersYetInRoom().begin () );
                                }
                        }
                }

                newRoom->getCamera()->turnOn( newRoom->getMediator()->getActivePlayer(), direction );

                activeRoom = newRoom;
                activeRoom->activate();
        }
        // can’t create player, game over
        else
        {
                delete newRoom;
                activeRoom = newRoom = 0;
        }

        return newRoom;
}

Room* MapManager::createRoom( const std::string& fileName )
{
        std::auto_ptr< RoomBuilder > roomBuilder( new RoomBuilder( isomot->getItemDataManager() ) );
        return roomBuilder->buildRoom( isomot::sharePath() + "map/" + fileName );
}

Room* MapManager::createRoomThenAddItToListOfRooms( const std::string& fileName )
{
        Room* room = createRoom( fileName );
        rooms.push_back( room );
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
                activeRoom = 0;
        }

        removeRoom( inactiveRoom );

        if ( activeRoom != 0 )
        {
                activeRoom->activate();
        }

        return activeRoom;
}

void MapManager::removeRoom( Room* whichRoom )
{
        if ( whichRoom != 0 )
        {
                if ( whichRoom->isActive() )
                        whichRoom->deactivate();

                if ( whichRoom == this->activeRoom )
                        this->activeRoom = 0;

                rooms.erase( std::remove_if( rooms.begin (), rooms.end (), std::bind2nd( EqualRoom(), whichRoom->getNameOfFileWithDataAboutRoom() ) ), rooms.end() );
                delete whichRoom;
        }
}

void MapManager::readVisitedSequence( sgxml::exploredRooms::visited_sequence& visitedSequence )
{
        resetVisitedRooms();

        for ( sgxml::exploredRooms::visited_const_iterator i = visitedSequence.begin (); i != visitedSequence.end (); ++i )
        {
                std::list< MapRoomData >::iterator m = std::find_if( mapData.begin(), mapData.end(), std::bind2nd( EqualMapRoomData(), ( *i ).filename() ) );
                if ( m != mapData.end() )
                {
                        ( *m ).setVisited( true );
                }
        }
}

void MapManager::storeVisitedSequence( sgxml::exploredRooms::visited_sequence& visitedSequence )
{
        for ( std::list< MapRoomData >::iterator i = this->mapData.begin (); i != this->mapData.end (); ++i )
        {
                if ( ( *i ).isVisited() )
                {
                        visitedSequence.push_back( sgxml::visited( ( *i ).getNameOfRoomFile() ) );
                }
        }
}

unsigned int MapManager::countVisitedRooms()
{
        unsigned int number = 0;

        for ( std::list< MapRoomData >::iterator i = this->mapData.begin (); i != this->mapData.end (); ++i )
        {
                number += ( *i ).isVisited() ? 1 : 0;
        }

        return number;
}

void MapManager::resetVisitedRooms()
{
        for ( std::list< MapRoomData >::iterator i = this->mapData.begin (); i != this->mapData.end (); ++i )
        {
                ( *i ).setVisited( false );
        }
}

MapRoomData* MapManager::findRoomData( const std::string& room )
{
        std::list< MapRoomData >::iterator i = std::find_if( mapData.begin (), mapData.end (), std::bind2nd( EqualMapRoomData(), room ) );
        MapRoomData* data = ( i != mapData.end() ? static_cast< MapRoomData* >( &( *i ) ) : 0 );

        return data;
}

Room* MapManager::findRoom( const std::string& room )
{
        std::vector< Room* >::iterator i = std::find_if( rooms.begin (), rooms.end (), std::bind2nd( EqualRoom(), room ) );
        return ( i != rooms.end() ? ( *i ) : 0 );
}

Room* MapManager::getRoomOfInactivePlayer()
{
        Room* room = 0;

        for ( size_t i = 0; i < this->rooms.size (); ++i )
        {
                if ( this->rooms[ i ] != this->activeRoom )
                        room = this->rooms[ i ];
        }

        return room;
}

bool EqualMapRoomData::operator()( const MapRoomData& mapData, const std::string& room ) const
{
        return ( mapData.getNameOfRoomFile().compare( room ) == 0 );
}

bool EqualRoom::operator()( Room* room, const std::string& nameOfRoom ) const
{
        return ( room->getNameOfFileWithDataAboutRoom().compare( nameOfRoom ) == 0 );
}

}

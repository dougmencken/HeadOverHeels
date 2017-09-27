
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
        // get data of first room
        MapRoomData* firstRoomData = findRoomData( firstRoomFileName );

        // create first room
        if ( firstRoomData != 0 )
        {
                std::auto_ptr< RoomBuilder > roomBuilder( new RoomBuilder( isomot->getItemDataManager() ) );
                Room* firstRoom = roomBuilder->buildRoom( isomot::sharePath() + "map/" + firstRoomFileName );

                if ( firstRoom != 0 )
                {
                        // initial position of player
                        ItemData* firstPlayerData = isomot->getItemDataManager()->findItemByLabel( "head" );

                        int centerX = RoomBuilder::getXCenterOfRoom( firstPlayerData, firstRoom );
                        int centerY = RoomBuilder::getYCenterOfRoom( firstPlayerData, firstRoom );

                        // store initial position of player in room’s data
                        PlayerInitialPosition firstPlayerPosition( "head" );
                        firstPlayerPosition.assignPosition( JustWait, centerX, centerY, 0, West );
                        firstRoomData->addPlayerPosition( firstPlayerPosition );

                        // create player
                        roomBuilder->createPlayerInTheSameRoom( "head", "behavior of Head", centerX, centerY, 0, West, false );

                        firstRoom->activatePlayerByName( "head" );
                        firstRoom->getCamera()->turnOn( firstRoom->getMediator()->getActivePlayer(), JustWait );
                        activeRoom = firstRoom;
                        firstRoomData->setNameOfActivePlayer( "head" );
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
                        // initial position of player
                        ItemData* secondPlayerData = isomot->getItemDataManager()->findItemByLabel( "heels" );

                        int centerX = RoomBuilder::getXCenterOfRoom( secondPlayerData, secondRoom );
                        int centerY = RoomBuilder::getYCenterOfRoom( secondPlayerData, secondRoom );

                        // store initial position of player in room’s data
                        PlayerInitialPosition secondPlayerPosition( "heels" );
                        secondPlayerPosition.assignPosition( JustWait, centerX, centerY, 0, South );
                        secondRoomData->addPlayerPosition( secondPlayerPosition);

                        // create player
                        roomBuilder->createPlayerInTheSameRoom( "heels", "behavior of Heels", centerX, centerY, 0, South, false );

                        secondRoom->activatePlayerByName( "heels" );
                        secondRoom->getCamera()->turnOn( secondRoom->getMediator()->getActivePlayer(), JustWait );
                        secondRoomData->setNameOfActivePlayer( "heels" );
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
                if ( this->activeRoom != 0 && this->activeRoom->getNameOfFileWithDataAboutRoom().compare( roomData->getRoom() ) == 0 )
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
                        std::string behavior;

                        if ( nameOfCharacter == "head" )
                                behavior = "behavior of Head";
                        else if ( nameOfCharacter == "heels" )
                                behavior = "behavior of Heels";
                        else if ( nameOfCharacter == "headoverheels" )
                                behavior = "behavior of Head over Heels";

                        // store initial position of player in room’s data
                        PlayerInitialPosition playerPosition( nameOfCharacter );
                        playerPosition.assignPosition( Direction( data.entry() ), data.x (), data.y (), data.z (), Direction( data.direction() ) );
                        roomData->addPlayerPosition( playerPosition );

                        // create player
                        std::auto_ptr< RoomBuilder > roomBuilder( new RoomBuilder( isomot->getItemDataManager() ) );
                        PlayerItem* player = roomBuilder->createPlayerInRoom( room, nameOfCharacter, behavior, data.x (), data.y (), data.z (), Direction( data.direction() ), false );

                        // change activity of player by way of entry
                        switch ( Direction( data.entry() ) )
                        {
                                case North:
                                case Northeast:
                                case Northwest:
                                        player->getBehavior()->changeActivityOfItem( AutoMoveSouth );
                                        break;

                                case South:
                                case Southeast:
                                case Southwest:
                                        player->getBehavior()->changeActivityOfItem( AutoMoveNorth );
                                        break;

                                case East:
                                case Eastnorth:
                                case Eastsouth:
                                        player->getBehavior()->changeActivityOfItem( AutoMoveWest );
                                        break;

                                case West:
                                case Westnorth:
                                case Westsouth:
                                        player->getBehavior()->changeActivityOfItem( AutoMoveEast );
                                        break;

                                case ByTeleport:
                                case ByTeleportToo:
                                        player->getBehavior()->changeActivityOfItem( BeginWayInTeletransport );
                                        break;

                                case JustWait:
                                        // it’s the case of resume of saved game
                                        // show bubbles only for active player
                                        if ( data.active() )
                                        {
                                                player->getBehavior()->changeActivityOfItem( BeginWayInTeletransport );
                                        }
                                        break;

                                default:
                                        ;
                        }

                        // for active player or for other player when it is created in another room
                        // when other player is in the same room as active player then there’s no need to do anything more
                        if ( data.active() || this->activeRoom != room )
                        {
                                room->activatePlayerByName( nameOfCharacter );
                                room->getCamera()->turnOn( room->getMediator()->getActivePlayer(), Direction( data.entry() ) );
                                /////room->getCamera()->centerOn( room->getMediator()->getActivePlayer () );

                                if ( data.active() )
                                {
                                        // for active player this room is active one
                                        this->activeRoom = room;
                                }

                                roomData->setNameOfActivePlayer( nameOfCharacter );
                                this->rooms.push_back( room );
                        }
                }
        }
}

void MapManager::reset()
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

        // reset data of room
        std::for_each( this->mapData.begin (), this->mapData.end (), std::mem_fun_ref( &MapRoomData::reset ) );
}

Room* MapManager::changeRoom( const Direction& exit )
{
        activeRoom->deactivate();

        SoundManager::getInstance()->stopEverySound ();

        // data of previous room
        MapRoomData* previousRoomData = findRoomData( activeRoom->getNameOfFileWithDataAboutRoom() );

        // bin position of player in previous room
        std::string nameOfActivePlayer = previousRoomData->getNameOfActivePlayer();
        previousRoomData->removePlayerPosition( nameOfActivePlayer );

        // get player’s position in new room
        PlayerItem* player = static_cast< PlayerItem* >( activeRoom->getMediator()->findItemByLabel( nameOfActivePlayer ) );
        PlayerInitialPosition exitPosition( nameOfActivePlayer );
        exitPosition.assignPosition( player->getExit(), player->getX(), player->getY(), player->getZ(), player->getOrientation() );
        std::string behaviorOfPlayer = player->getBehavior()->getNameOfBehavior ();

        // if player carries some item
        bool withItem = player->consultTakenItemImage() != 0;

        // get limits of room
        // there’s possibility to exit and to enter new room in cases when player travels through floor, roof or teleport
        int northBound = activeRoom->getBound( North );
        int eastBound = activeRoom->getBound( East );
        int southBound = activeRoom->getBound( South );
        int westBound = activeRoom->getBound( West );

        // when there’re no players left in the room
        if ( ! previousRoomData->remainPlayers() || nameOfActivePlayer == "headoverheels" )
        {
                // bin previous room
                rooms.erase( std::remove_if( rooms.begin (), rooms.end (), std::bind2nd( EqualRoom(), activeRoom->getNameOfFileWithDataAboutRoom() ) ), rooms.end() );
                previousRoomData->clearPlayersPosition();
                delete activeRoom;
        }
        // otherwise remove active player and select new active player in abandoned room
        else
        {
                activeRoom->removePlayer( player );
                previousRoomData->removePlayerPosition( player->getLabel() );
        }

        // search the map for the next room and get the way to entry
        Direction entry;
        MapRoomData* nextRoomData = findRoomData( previousRoomData->findConnectedRoom( exit, &entry ) );
        assert( nextRoomData != 0 );
        nextRoomData->adjustEntry( &entry, previousRoomData->getRoom() );

        bool playerPresent = false;

        Room* newRoom = 0;

        if ( ! nextRoomData->remainPlayers() )
        {
                std::auto_ptr< RoomBuilder > roomBuilder( new RoomBuilder( isomot->getItemDataManager() ) );
                newRoom = roomBuilder->buildRoom( isomot::sharePath() + "map/" + nextRoomData->getRoom() );
                rooms.push_back( newRoom );
        }
        // if already exists
        else
        {
                newRoom = findRoom( nextRoomData->getRoom() );
                newRoom->setActive( true );
                playerPresent = true;
        }

        ItemData* playerData = isomot->getItemDataManager()->findItemByLabel( nameOfActivePlayer );

        // get player’s exit position in old room to calculate entry position in new room
        int x = exitPosition.getX ();
        int y = exitPosition.getY ();
        int z = exitPosition.getZ ();
        newRoom->calculateEntryCoordinates( entry, playerData->widthX, playerData->widthY, northBound, eastBound, southBound, westBound, &x, &y, &z );

        // set entry position of player
        PlayerInitialPosition playerPosition( nameOfActivePlayer );
        playerPosition.assignPosition( entry, x, y, z, exitPosition.getOrientation() );

        // if there is player already, get its data
        // for case when way of entry of both players is the same
        if ( playerPresent )
        {
                PlayerItem* player = newRoom->getMediator()->getActivePlayer();
                PlayerInitialPosition playerPresentPosition( player->getLabel() );
                playerPresentPosition.assignPosition( player->getExit(), player->getX(), player->getY(), player->getZ(), player->getOrientation() );
                nextRoomData->addPlayerPosition( playerPosition, playerPresentPosition );
        }
        else
        {
                nextRoomData->addPlayerPosition( playerPosition );
        }

        // create player
        std::auto_ptr< RoomBuilder > roomBuilder( new RoomBuilder( isomot->getItemDataManager() ) );
        if ( entry == ByTeleport || entry == ByTeleportToo )
        {
                z = Top;
        }
        player = roomBuilder->createPlayerInRoom( newRoom, nameOfActivePlayer, behaviorOfPlayer, x, y, z, exitPosition.getOrientation(), withItem );

        // change activity of player by way of entry
        switch ( entry )
        {
                case North:
                case Northeast:
                case Northwest:
                        player->getBehavior()->changeActivityOfItem( AutoMoveSouth );
                        break;

                case South:
                case Southeast:
                case Southwest:
                        player->getBehavior()->changeActivityOfItem( AutoMoveNorth );
                        break;

                case East:
                case Eastnorth:
                case Eastsouth:
                        player->getBehavior()->changeActivityOfItem( AutoMoveWest );
                        break;

                case West:
                case Westnorth:
                case Westsouth:
                        player->getBehavior()->changeActivityOfItem( AutoMoveEast );
                        break;

                case ByTeleport:
                case ByTeleportToo:
                        player->getBehavior()->changeActivityOfItem( BeginWayInTeletransport );
                        break;

                case Up:
                        player->getBehavior()->changeActivityOfItem( Fall );
                        break;

                default:
                        ;
        }

        nextRoomData->setNameOfActivePlayer( nameOfActivePlayer );
        newRoom->activatePlayerByName( nameOfActivePlayer );
        newRoom->getCamera()->turnOn( newRoom->getMediator()->getActivePlayer(), entry );

        activeRoom = newRoom;
        activeRoom->activate();

        return newRoom;
}

Room* MapManager::restartRoom()
{
        activeRoom->deactivate();

        MapRoomData* activeRoomData = findRoomData( activeRoom->getNameOfFileWithDataAboutRoom() );

        std::auto_ptr< RoomBuilder > roomBuilder( new RoomBuilder( isomot->getItemDataManager() ) );
        Room* newRoom = roomBuilder->buildRoom ( isomot::sharePath() + "map/" + activeRoomData->getRoom() );

        // initial position of all players in room
        std::list< PlayerInitialPosition > playersPosition = activeRoomData->getPlayersPosition();

        std::string nameOfActivePlayer = "in~restart";
        std::string playerBehavior = "still";
        Direction entry = NoDirection;
        PlayerItem* player = 0;

        // for each player in room
        for ( std::list< PlayerInitialPosition >::iterator i = playersPosition.begin (); i != playersPosition.end (); ++i )
        {
                player = dynamic_cast< PlayerItem* >( activeRoom->getMediator()->findItemByLabel( ( *i ).getPlayer() ) );

                // if there is player in room
                if ( player != 0 )
                {
                        playerBehavior = player->getBehavior()->getNameOfBehavior ();
                }
                // when there are no players in room but are known to have entered
                // it is the case when simple player enters room with another player already in this room
                else
                {
                        if ( ( *i ).getPlayer() == "head" )
                                playerBehavior = "behavior of Head";
                        else if ( ( *i ).getPlayer() == "heels" )
                                playerBehavior = "behavior of Heels";
                        else if ( ( *i ).getPlayer() == "headoverheels" )
                                playerBehavior = "behavior of Head over Heels";
                }

                if ( activeRoomData->getNameOfActivePlayer() == ( *i ).getPlayer() || player == 0 )
                {
                        // if there’s no active player then get its name from initial position
                        nameOfActivePlayer = player != 0 ? activeRoomData->getNameOfActivePlayer() : ( *i ).getPlayer();

                        // initial position
                        int x = ( *i ).getX();
                        int y = ( *i ).getY();
                        int z = ( *i ).getZ();

                        // create player
                        player = roomBuilder->createPlayerInRoom( newRoom, ( *i ).getPlayer(), playerBehavior, x, y, z, ( *i ).getOrientation(), false );

                        // Si se ha podido crear al jugador, la partida continua. De lo contrario quiere decir que ha terminado
                        if ( player != 0 )
                        {
                                entry = ( *i ).getEntry();

                                // change player when it passed from the composite head~over~heels to a simple one
                                if ( player->getLabel() != nameOfActivePlayer )
                                {
                                        nameOfActivePlayer = player->getLabel();

                                        // update initial positions on change of player
                                        PlayerInitialPosition newPlayerInitialPosition( nameOfActivePlayer );
                                        newPlayerInitialPosition.assignPosition( entry, ( *i ).getX(), ( *i ).getY(), ( *i ).getZ(), ( *i ).getOrientation() );
                                        i = playersPosition.erase( i );
                                        i = playersPosition.insert( i, newPlayerInitialPosition );
                                }

                                switch ( entry )
                                {
                                        case North:
                                        case Northeast:
                                        case Northwest:
                                                player->getBehavior()->changeActivityOfItem( AutoMoveSouth );
                                                break;

                                        case South:
                                        case Southeast:
                                        case Southwest:
                                                player->getBehavior()->changeActivityOfItem( AutoMoveNorth );
                                                break;

                                        case East:
                                        case Eastnorth:
                                        case Eastsouth:
                                                player->getBehavior()->changeActivityOfItem( AutoMoveWest );
                                                break;

                                        case West:
                                        case Westnorth:
                                        case Westsouth:
                                                player->getBehavior()->changeActivityOfItem( AutoMoveEast );
                                                break;

                                        case ByTeleport:
                                        case ByTeleportToo:
                                                player->getBehavior()->changeActivityOfItem( BeginWayInTeletransport );
                                                break;

                                        default:
                                                ;
                                }
                        }
                }
                // Si no es el jugador activo entonces el jugador reaparecerá en la posición que tenía en el
                // momento que entró en la sala
                else
                {
                        PlayerInitialPosition* position = activeRoomData->findPlayerPosition( ( *i ).getPlayer() );
                        player = roomBuilder->createPlayerInRoom(
                                newRoom,
                                ( *i ).getPlayer(),
                                playerBehavior,
                                position->getX(), position->getY(), position->getZ(), position->getOrientation(),
                                false
                        );

                        // change activity of player by way of entry
                        switch ( ( *i ).getEntry() )
                        {
                                case North:
                                case Northeast:
                                case Northwest:
                                        player->getBehavior()->changeActivityOfItem( AutoMoveSouth );
                                        break;

                                case South:
                                case Southeast:
                                case Southwest:
                                        player->getBehavior()->changeActivityOfItem( AutoMoveNorth );
                                        break;

                                case East:
                                case Eastnorth:
                                case Eastsouth:
                                        player->getBehavior()->changeActivityOfItem( AutoMoveWest );
                                        break;

                                case West:
                                case Westnorth:
                                case Westsouth:
                                        player->getBehavior()->changeActivityOfItem( AutoMoveEast );
                                        break;

                                case ByTeleport:
                                case ByTeleportToo:
                                        player->getBehavior()->changeActivityOfItem( BeginWayInTeletransport );
                                        break;

                                default:
                                        ;
                        }
                }
        }

        // remove existing room
        rooms.erase( std::remove_if( rooms.begin (), rooms.end (), std::bind2nd( EqualRoom(), activeRoom->getNameOfFileWithDataAboutRoom() ) ), rooms.end() );
        delete activeRoom;

        if ( player != 0 )
        {
                // add room just created
                rooms.push_back( newRoom );

                activeRoomData->setNameOfActivePlayer( nameOfActivePlayer );
                newRoom->activatePlayerByName( nameOfActivePlayer );
                newRoom->getCamera()->turnOn( newRoom->getMediator()->getActivePlayer(), entry );

                activeRoom = newRoom;
                activeRoom->activate();
        }
        // could not create player, game over
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
                SoundManager::getInstance()->stopEverySound ();

                activeRoom->deactivate();

                std::vector< Room* >::iterator i = std::find_if( rooms.begin (), rooms.end (), std::bind2nd( EqualRoom(), activeRoom->getNameOfFileWithDataAboutRoom() ) );
                // get next room to swap with
                ++i;
                // when it’s last one swap with first one
                activeRoom = ( i != rooms.end() ? ( *i ) : *rooms.begin() );

                activeRoom->activate();
        }

        return activeRoom;
}

Room* MapManager::removeRoomAndSwap ()
{
        activeRoom->deactivate();
        Room* inactiveRoom = activeRoom;

        MapRoomData* activeRoomData = findRoomData( activeRoom->getNameOfFileWithDataAboutRoom() );
        activeRoomData->removePlayerPosition( activeRoomData->getNameOfActivePlayer() );

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

void MapManager::updateActivePlayer()
{
        MapRoomData* activeRoomData = findRoomData( activeRoom->getNameOfFileWithDataAboutRoom () );
        activeRoomData->setNameOfActivePlayer( activeRoom->getMediator()->getActivePlayer()->getLabel() );
}

void MapManager::loadVisitedSequence( sgxml::exploredRooms::visited_sequence& visitedSequence )
{
        for ( sgxml::exploredRooms::visited_const_iterator i = visitedSequence.begin (); i != visitedSequence.end (); ++i )
        {
                std::list< MapRoomData >::iterator m = std::find_if( mapData.begin(), mapData.end(), std::bind2nd( EqualMapRoomData(), ( *i ).filename() ) );
                if ( m != mapData.end() )
                {
                        static_cast< MapRoomData* >( &( *m ) )->setVisited( true );
                }
        }
}

void MapManager::saveVisitedSequence( sgxml::exploredRooms::visited_sequence& visitedSequence )
{
        for ( std::list< MapRoomData >::iterator i = this->mapData.begin (); i != this->mapData.end (); ++i )
        {
                if ( ( *i ).isVisited() )
                {
                        visitedSequence.push_back( sgxml::visited( ( *i ).getRoom() ) );
                }
        }
}

unsigned short MapManager::countVisitedRooms()
{
        unsigned short number = 0;

        for ( std::list< MapRoomData >::iterator i = this->mapData.begin (); i != this->mapData.end (); ++i )
        {
                number += ( *i ).isVisited() ? 1 : 0;
        }

        return number;
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

PlayerInitialPosition* MapManager::findPlayerPosition( const std::string& room, const std::string& name )
{
        return this->findRoomData( room )->findPlayerPosition( name );
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
        return ( mapData.getRoom().compare( room ) == 0 );
}

bool EqualRoom::operator()( Room* room, const std::string& nameOfRoom ) const
{
        return ( room->getNameOfFileWithDataAboutRoom().compare( nameOfRoom ) == 0 );
}

}

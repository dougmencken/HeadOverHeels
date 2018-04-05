
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

#include <tinyxml2.h>


namespace isomot
{

MapManager::MapManager( Isomot* isomot )
        : isomot( isomot )
        , activeRoom( nilPointer )
{

}

MapManager::~MapManager( )
{

}

void MapManager::loadMap ( const std::string& fileName )
{
        // read from XML file
        tinyxml2::XMLDocument mapXml;
        tinyxml2::XMLError result = mapXml.LoadFile( fileName.c_str () );
        if ( result != tinyxml2::XML_SUCCESS )
        {
                std::cerr << "can’t load file \"" << fileName << "\" with map of game" << std::endl ;
                return;
        }

        tinyxml2::XMLElement* root = mapXml.FirstChildElement( "map" );

        for ( tinyxml2::XMLElement* room = root->FirstChildElement( "room" ) ;
                        room != nilPointer ;
                        room = room->NextSiblingElement( "room" ) )
        {
                std::string roomFile = room->Attribute( "file" );
                /* std::cout << "got room \"" << roomFile << "\" of map" << std::endl ; */
                MapRoomData* roomData = new MapRoomData( roomFile );

                tinyxml2::XMLElement* north = room->FirstChildElement( "north" ) ;
                tinyxml2::XMLElement* south = room->FirstChildElement( "south" ) ;
                tinyxml2::XMLElement* east = room->FirstChildElement( "east" ) ;
                tinyxml2::XMLElement* west = room->FirstChildElement( "west" ) ;

                tinyxml2::XMLElement* floor = room->FirstChildElement( "floor" ) ;
                tinyxml2::XMLElement* roof = room->FirstChildElement( "roof" ) ;
                tinyxml2::XMLElement* teleport = room->FirstChildElement( "teleport" ) ;
                tinyxml2::XMLElement* teleport2 = room->FirstChildElement( "teleport2" ) ;

                tinyxml2::XMLElement* northeast = room->FirstChildElement( "northeast" ) ;
                tinyxml2::XMLElement* northwest = room->FirstChildElement( "northwest" ) ;
                tinyxml2::XMLElement* southeast = room->FirstChildElement( "southeast" ) ;
                tinyxml2::XMLElement* southwest = room->FirstChildElement( "southwest" ) ;
                tinyxml2::XMLElement* eastnorth = room->FirstChildElement( "eastnorth" ) ;
                tinyxml2::XMLElement* eastsouth = room->FirstChildElement( "eastsouth" ) ;
                tinyxml2::XMLElement* westnorth = room->FirstChildElement( "westnorth" ) ;
                tinyxml2::XMLElement* westsouth = room->FirstChildElement( "westsouth" ) ;

                if ( north != nilPointer ) // connection on north
                        roomData->setNorth( north->FirstChild()->ToText()->Value() );

                if ( south != nilPointer ) // connection on south
                        roomData->setSouth( south->FirstChild()->ToText()->Value() );

                if ( east != nilPointer ) // connection on east
                        roomData->setEast( east->FirstChild()->ToText()->Value() );

                if ( west != nilPointer ) // connection on west
                        roomData->setWest( west->FirstChild()->ToText()->Value() );

                if ( floor != nilPointer ) // connection on bottom
                        roomData->setFloor( floor->FirstChild()->ToText()->Value() );

                if ( roof != nilPointer ) // connection on top
                        roomData->setRoof( roof->FirstChild()->ToText()->Value() );

                if ( teleport != nilPointer ) // connection via teleport
                        roomData->setTeleport( teleport->FirstChild()->ToText()->Value() );

                if ( teleport2 != nilPointer ) // connection via second teleport
                        roomData->setTeleportToo( teleport2->FirstChild()->ToText()->Value() );

                if ( northeast != nilPointer ) // north-east connection
                        roomData->setNorthEast( northeast->FirstChild()->ToText()->Value() );

                if ( northwest != nilPointer ) // north-west connection
                        roomData->setNorthWest( northwest->FirstChild()->ToText()->Value() );

                if ( southeast != nilPointer ) // south-east connection
                        roomData->setSouthEast( southeast->FirstChild()->ToText()->Value() );

                if ( southwest != nilPointer ) // south-west connection
                        roomData->setSouthWest( southwest->FirstChild()->ToText()->Value() );

                if ( eastnorth != nilPointer ) // east-north connection
                        roomData->setEastNorth( eastnorth->FirstChild()->ToText()->Value() );

                if ( eastsouth != nilPointer ) // east-south connection
                        roomData->setEastSouth( eastsouth->FirstChild()->ToText()->Value() );

                if ( westnorth != nilPointer ) // west-north connection
                        roomData->setWestNorth( westnorth->FirstChild()->ToText()->Value() );

                if ( westsouth != nilPointer ) // west-south connection
                        roomData->setWestSouth( westsouth->FirstChild()->ToText()->Value() );

                // add data for this room to the list
                this->theMap.push_back( roomData );
        }

        std::cout << "read map of rooms from " << fileName << std::endl ;
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
                Room* firstRoom = RoomBuilder::buildRoom( isomot::sharePath() + "map" + pathSeparator + firstRoomFileName );

                if ( firstRoom != nilPointer )
                {
                        ItemData* firstPlayerData = GameManager::getInstance()->getItemDataManager()->findDataByLabel( "head" );

                        int centerX = RoomBuilder::getXCenterOfRoom( firstPlayerData, firstRoom );
                        int centerY = RoomBuilder::getYCenterOfRoom( firstPlayerData, firstRoom );

                        // create player Head
                        RoomBuilder::createPlayerInRoom( firstRoom, "head", true, centerX, centerY, 0, West );

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
                Room* secondRoom = RoomBuilder::buildRoom( isomot::sharePath() + "map" + pathSeparator + secondRoomFileName );

                if ( secondRoom != nilPointer )
                {
                        ItemData* secondPlayerData = GameManager::getInstance()->getItemDataManager()->findDataByLabel( "heels" );

                        int centerX = RoomBuilder::getXCenterOfRoom( secondPlayerData, secondRoom );
                        int centerY = RoomBuilder::getYCenterOfRoom( secondPlayerData, secondRoom );

                        // create player Heels
                        RoomBuilder::createPlayerInRoom( secondRoom, "heels", true, centerX, centerY, 0, South );

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
                        room = RoomBuilder::buildRoom ( isomot::sharePath() + "map" + pathSeparator + data.roomFilename() );
                }

                // place character in room
                if ( room != nilPointer )
                {
                        std::string nameOfCharacter = data.label();

                        // create player
                        PlayerItem* player = RoomBuilder::createPlayerInRoom(
                                                        room,
                                                        nameOfCharacter,
                                                        true,
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
        std::for_each( this->rooms.begin (), this->rooms.end (), DeleteObject() );

        this->rooms.clear();

        this->activeRoom = nilPointer ;
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

        Room* newRoom = findRoomByFile( nextRoomData->getNameOfRoomFile() );

        if ( newRoom != nilPointer )
        {
                std::cout << "room \"" << newRoom->getNameOfFileWithDataAboutRoom() << "\" is already created" << std::endl ;
                newRoom->setWayOfExit( "no exit" );
        }
        else
        {
                std::cout << "going to create room \"" << nextRoomData->getNameOfRoomFile() << "\"" << std::endl ;

                newRoom = RoomBuilder::buildRoom( isomot::sharePath() + "map" + pathSeparator + nextRoomData->getNameOfRoomFile() );
                rooms.push_back( newRoom );
        }

        ItemData* dataOfRoamer = GameManager::getInstance()->getItemDataManager()->findDataByLabel( nameOfRoamer );

        // get player’s exit position in old room to calculate entry position in new room
        int entryX = exitX ;
        int entryY = exitY ;
        int entryZ = exitZ ;

        std::cout << "exit coordinates are x=" << exitX << " y=" << exitY << " z=" << exitZ << std::endl ;
        newRoom->calculateEntryCoordinates( wayOfEntry, dataOfRoamer->getWidthX(), dataOfRoamer->getWidthY(), northBound, eastBound, southBound, westBound, &entryX, &entryY, &entryZ );
        std::cout << "entry coordinates are x=" << entryX << " y=" << entryY << " z=" << entryZ << std::endl ;

        // create player

        if ( wayOfEntry.toString() == "via teleport" || wayOfEntry.toString() == "via second teleport" )
        {
                entryZ = Top;
        }

        PlayerItem* newItemOfRoamer = RoomBuilder::createPlayerInRoom( newRoom, nameOfRoamer, true, entryX, entryY, entryZ, exitOrientation, wayOfEntry );

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

        // rebuild room
        Room* newRoom = RoomBuilder::buildRoom ( isomot::sharePath() + "map" + pathSeparator + oldRoomData->getNameOfRoomFile() );

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
                                RoomBuilder::createPlayerInRoom( oldRoom,
                                                                 nameOfActivePlayer, true,
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
                        alivePlayer = RoomBuilder::createPlayerInRoom( newRoom,
                                                                       player->getLabel(),
                                                                       true,
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
        return RoomBuilder::buildRoom( isomot::sharePath() + "map" + pathSeparator + fileName );
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

                std::list< Room* >::iterator ri = rooms.begin ();
                while ( ri != rooms.end () )
                {
                        if ( *ri == activeRoom ) break;
                        ++ ri;
                }

                // get next room to swap with
                ++ ri;
                // when it’s last one swap with first one
                activeRoom = ( ri != rooms.end() ? *ri : *rooms.begin() );

                std::cout << "swop room \"" << fileOfPreviousRoom << "\" with \"" << activeRoom->getNameOfFileWithDataAboutRoom() << "\"" << std::endl ;

                activeRoom->activate();
        }

        return activeRoom;
}

Room* MapManager::removeRoomAndSwap ()
{
        activeRoom->deactivate();

        Room* inactiveRoom = activeRoom;

        std::list< Room* >::iterator ri = rooms.begin ();
        while ( ri != rooms.end () )
        {
                if ( *ri == activeRoom ) break;
                ++ ri;
        }

        // get next room to swap with
        ++ ri;
        // when it’s last one swap with first one
        activeRoom = ( ri != rooms.end() ? *ri : *rooms.begin() );

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

                rooms.remove( whichRoom );

                delete whichRoom;
        }
}

void MapManager::readVisitedSequence( sgxml::exploredRooms::visited_sequence& visitedSequence )
{
        resetVisitedRooms();

        for ( sgxml::exploredRooms::visited_const_iterator i = visitedSequence.begin () ; i != visitedSequence.end () ; ++i )
        {
                for ( std::list< MapRoomData * >::iterator m = theMap.begin() ; m != theMap.end() ; ++ m )
                {
                        if ( ( *m )->getNameOfRoomFile() == ( *i ).filename() )
                        {
                                ( *m )->setVisited( true );
                                break;
                        }
                }
        }
}

void MapManager::storeVisitedSequence( sgxml::exploredRooms::visited_sequence& visitedSequence )
{
        for ( std::list< MapRoomData * >::iterator i = theMap.begin () ; i != theMap.end () ; ++ i )
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

        for ( std::list< MapRoomData * >::iterator i = theMap.begin () ; i != theMap.end () ; ++ i )
        {
                number += ( *i )->isVisited() ? 1 : 0;
        }

        return number;
}

void MapManager::resetVisitedRooms()
{
        for ( std::list< MapRoomData * >::iterator i = theMap.begin () ; i != theMap.end () ; ++ i )
        {
                ( *i )->setVisited( false );
        }
}

MapRoomData* MapManager::findRoomData( const std::string& roomFile ) const
{
        if ( roomFile.empty() ) return nilPointer;

        for ( std::list< MapRoomData * >::const_iterator m = theMap.begin () ; m != theMap.end () ; ++ m )
        {
                if ( ( *m )->getNameOfRoomFile() == roomFile )
                {
                        return *m ;
                }
        }

        return nilPointer ;
}

Room* MapManager::findRoomByFile( const std::string& roomFile ) const
{
        for ( std::list< Room* >::const_iterator ri = rooms.begin () ; ri != rooms.end () ; ++ ri )
        {
                if ( ( *ri )->getNameOfFileWithDataAboutRoom() == roomFile )
                {
                        return *ri ;
                }
        }

        return nilPointer ;
}

Room* MapManager::getRoomOfInactivePlayer() const
{
        for ( std::list< Room* >::const_iterator ri = rooms.begin () ; ri != rooms.end () ; ++ ri )
        {
                if ( *ri != this->activeRoom )
                {
                        return *ri ;
                }
        }

        return nilPointer ;
}

}

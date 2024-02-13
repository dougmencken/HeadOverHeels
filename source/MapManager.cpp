
#include "MapManager.hpp"

#include "Isomot.hpp"
#include "RoomBuilder.hpp"
#include "AvatarItem.hpp"
#include "Door.hpp"
#include "Behavior.hpp"
#include "Mediator.hpp"
#include "Camera.hpp"
#include "SoundManager.hpp"
#include "GameManager.hpp"
#include "GamePreferences.hpp"
#include "ItemDescriptions.hpp"

#include "ospaths.hpp"

#include <tinyxml2.h>

#define GENERATE_ROOM_DESCRIPTIONS      0


/* static */ MapManager MapManager::instance ;

bool MapManager::buildEveryRoomAtOnce = false ;


/* static */
MapManager & MapManager::getInstance ()
{
        return MapManager::instance ;
}

MapManager::~MapManager( )
{
        clear() ;
}

/* private */ void MapManager::clear ()
{
        forgetVisitedRooms () ;

        binRoomsInPlay() ;

        for ( std::map< std::string, Room * >::const_iterator ri = gameRooms.begin () ; ri != gameRooms.end () ; ++ ri )
        {
                delete ri->second ;
        }
        gameRooms.clear() ;

        for ( std::map< std::string, ConnectedRooms * >::const_iterator ci = linksBetweenRooms.begin () ; ci != linksBetweenRooms.end () ; ++ ci )
        {
                delete ci->second ;
        }
        linksBetweenRooms.clear() ;
}

void MapManager::readMap ( const std::string& fileName )
{
        clear();

        tinyxml2::XMLDocument mapXml;
        tinyxml2::XMLError result = mapXml.LoadFile( fileName.c_str () );
        if ( result != tinyxml2::XML_SUCCESS )
        {
                std::cerr << "can’t load file \"" << fileName << "\" with the map of the game" << std::endl ;
                return;
        }

# if defined( GENERATE_ROOM_DESCRIPTIONS ) && GENERATE_ROOM_DESCRIPTIONS

        std::string pathToRooms = ospaths::homePath () + "rooms" + ospaths::pathSeparator () ;

        if ( ! ospaths::folderExists( pathToRooms ) )
                mkdir( pathToRooms.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH );

#endif

        tinyxml2::XMLElement* root = mapXml.FirstChildElement( "map" );

        unsigned int howManyRooms = 0 ;
        for ( tinyxml2::XMLElement* room = root->FirstChildElement( "room" ) ;
                        room != nilPointer ;
                        room = room->NextSiblingElement( "room" ) )
        {
                ++ howManyRooms ;
        }
        std::cout << "the map of the game consists of the " << howManyRooms << " rooms" << std::endl ;

        unsigned int roomNth = 0 ;
        for ( tinyxml2::XMLElement* room = root->FirstChildElement( "room" ) ;
                        room != nilPointer ;
                        room = room->NextSiblingElement( "room" ) )
        {
                ++ roomNth ;

                tinyxml2::XMLElement* north = room->FirstChildElement( "north" ) ;
                tinyxml2::XMLElement* south = room->FirstChildElement( "south" ) ;
                tinyxml2::XMLElement* east = room->FirstChildElement( "east" ) ;
                tinyxml2::XMLElement* west = room->FirstChildElement( "west" ) ;

                tinyxml2::XMLElement* below = room->FirstChildElement( "below" ) ;
                tinyxml2::XMLElement* above = room->FirstChildElement( "above" ) ;
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

                ConnectedRooms * connections = new ConnectedRooms() ;

                if ( north != nilPointer )
                        connections->setRoomAtNorth( north->FirstChild()->ToText()->Value () );

                if ( south != nilPointer )
                        connections->setRoomAtSouth( south->FirstChild()->ToText()->Value () );

                if ( east != nilPointer )
                        connections->setRoomAtEast( east->FirstChild()->ToText()->Value () );

                if ( west != nilPointer )
                        connections->setRoomAtWest( west->FirstChild()->ToText()->Value () );

                if ( below != nilPointer )
                        connections->setRoomBelow( below->FirstChild()->ToText()->Value () );

                if ( above != nilPointer )
                        connections->setRoomAbove( above->FirstChild()->ToText()->Value () );

                if ( teleport != nilPointer )
                        connections->setRoomToTeleport( teleport->FirstChild()->ToText()->Value () );

                if ( teleport2 != nilPointer )
                        connections->setRoomToTeleportToo( teleport2->FirstChild()->ToText()->Value () );

                if ( northeast != nilPointer )
                        connections->setRoomAtNorthEast( northeast->FirstChild()->ToText()->Value () );

                if ( northwest != nilPointer )
                        connections->setRoomAtNorthWest( northwest->FirstChild()->ToText()->Value () );

                if ( southeast != nilPointer )
                        connections->setRoomAtSouthEast( southeast->FirstChild()->ToText()->Value () );

                if ( southwest != nilPointer )
                        connections->setRoomAtSouthWest( southwest->FirstChild()->ToText()->Value () );

                if ( eastnorth != nilPointer )
                        connections->setRoomAtEastNorth( eastnorth->FirstChild()->ToText()->Value () );

                if ( eastsouth != nilPointer )
                        connections->setRoomAtEastSouth( eastsouth->FirstChild()->ToText()->Value () );

                if ( westnorth != nilPointer )
                        connections->setRoomAtWestNorth( westnorth->FirstChild()->ToText()->Value () );

                if ( westsouth != nilPointer )
                        connections->setRoomAtWestSouth( westsouth->FirstChild()->ToText()->Value () );

                std::string roomFile = room->Attribute( "file" );

                linksBetweenRooms[ roomFile ] = connections ;

                if ( MapManager::buildEveryRoomAtOnce )
                {
                        // letters of allegro’s font are 8 x 7
                        const unsigned short widthOfChar = 8 ;
                        const unsigned short heightOfChar = 7 ;
                        const unsigned short interligne = 8 ;

                        const unsigned short offsetY = ( ( heightOfChar + interligne ) << 1 ) + 10 ;
                        const unsigned short atY = GamePreferences::getScreenHeight() - offsetY ;

                        std::string roomInRooms = "building " + util::toStringWithOrdinalSuffix( roomNth ) + " room out of " + util::number2string( howManyRooms ) ;

                        allegro::Pict::resetWhereToDraw() ;

                        allegro::fillRect( 0, atY - 4, GamePreferences::getScreenWidth(), atY,
                                           Color::blackColor().toAllegroColor() );

                        autouniqueptr< allegro::Pict > stripe( allegro::Pict::newPict( GamePreferences::getScreenWidth(), heightOfChar + interligne ) );

                        allegro::Pict::setWhereToDraw( *stripe ) ;

                        unsigned int textX = ( GamePreferences::getScreenWidth() - roomInRooms.length() * widthOfChar ) >> 1 ;
                        stripe->clearToColor( Color::byName( "blue" ).toAllegroColor() );
                        allegro::textOut( roomInRooms, textX, ( interligne >> 1 ) + 1, Color::byName( "yellow" ).toAllegroColor() );
                        allegro::bitBlit( *stripe, allegro::Pict::theScreen(), 0, atY );

                        textX = ( GamePreferences::getScreenWidth() - roomFile.length() * widthOfChar ) >> 1 ;
                        stripe->clearToColor( Color::byName( "blue" ).toAllegroColor() );
                        allegro::textOut( roomFile, textX, ( interligne >> 1 ) - 1, Color::byName( "green" ).toAllegroColor() );
                        allegro::bitBlit( *stripe, allegro::Pict::theScreen(), 0, atY + ( heightOfChar + interligne ) );

                        allegro::Pict::resetWhereToDraw() ;

                        allegro::fillRect( 0, atY + ( ( heightOfChar + interligne ) << 1 ),
                                           GamePreferences::getScreenWidth(), atY + ( ( heightOfChar + interligne ) << 1 ) + 4,
                                           Color::blackColor().toAllegroColor() );

                        allegro::update();

                        Room* theRoom = RoomBuilder::buildRoom( ospaths::sharePath() + "map" + ospaths::pathSeparator() + roomFile );
                        theRoom->setConnections( connections );
                        gameRooms[ roomFile ] = theRoom ;

                # if defined( GENERATE_ROOM_DESCRIPTIONS ) && GENERATE_ROOM_DESCRIPTIONS
                        // write the description of room
                        std::string saveTo = pathToRooms + theRoom->getNameOfRoomDescriptionFile ();
                        std::cout << "saving the description of room as \"" << saveTo << "\"" << std::endl ;
                        theRoom->saveAsXML( saveTo );
                # endif
                }
                else
                {
                        gameRooms[ roomFile ] = nilPointer ;
                }
        }

        std::cout << "read map of rooms from " << fileName << std::endl ;
}

void MapManager::beginNewGame( const std::string & headRoom, const std::string & heelsRoom )
{
        std::cout << "MapManager::beginNewGame( \"" << headRoom << "\", \"" << heelsRoom << "\" )" << std::endl ;

        clear () ;

        GameManager & gameManager = GameManager::getInstance () ;

        // reset the number of lives and other game's data
        gameManager.getGameInfo().resetForANewGame () ;

        if ( linksBetweenRooms.empty() )
                readMap( ospaths::sharePath() + "map" + ospaths::pathSeparator() + "map.xml" );

        // head’s room

        if ( linksBetweenRooms.find( headRoom ) != linksBetweenRooms.end () )
        {
                Room* firstRoom = getOrBuildRoomByFile( headRoom );

                if ( firstRoom != nilPointer )
                {
                        addRoomInPlay( firstRoom );

                        const DescriptionOfItem* headData = ItemDescriptions::descriptions ().getDescriptionByKind( "head" );

                        int centerX = firstRoom->getXCenterForItem( headData );
                        int centerY = firstRoom->getYCenterForItem( headData );

                        if ( headRoom != heelsRoom )
                        {
                                // create character Head
                                RoomBuilder::createCharacterInRoom( firstRoom, "head", true, centerX, centerY, 0, "west" );
                                firstRoom->activateCharacterByName( "head" );
                        }
                        else
                        {
                                // create character Head over Heels
                                RoomBuilder::createCharacterInRoom( firstRoom, "headoverheels", true, centerX, centerY, 0, "west" );
                                firstRoom->activateCharacterByName( "headoverheels" );
                        }

                        addRoomAsVisited( firstRoom->getNameOfRoomDescriptionFile () ) ;

                        activeRoom = firstRoom;
                }
        }
        else
        {
                std::cerr << "room \"" << headRoom << "\" doesn’t exist" << std::endl ;
        }

        if ( headRoom != heelsRoom )
        {
                // heels’ room

                if ( linksBetweenRooms.find( heelsRoom ) != linksBetweenRooms.end () )
                {
                        Room* secondRoom = getOrBuildRoomByFile( heelsRoom );

                        if ( secondRoom != nilPointer )
                        {
                                addRoomInPlay( secondRoom );

                                const DescriptionOfItem* heelsData = ItemDescriptions::descriptions ().getDescriptionByKind( "heels" );

                                int centerX = secondRoom->getXCenterForItem( heelsData );
                                int centerY = secondRoom->getYCenterForItem( heelsData );

                                // create character Heels
                                RoomBuilder::createCharacterInRoom( secondRoom, "heels", true, centerX, centerY, 0, "south" );
                                secondRoom->activateCharacterByName( "heels" );

                                addRoomAsVisited( secondRoom->getNameOfRoomDescriptionFile () ) ;
                        }
                }
                else
                {
                        std::cerr << "room \"" << heelsRoom << "\" doesn’t exist" << std::endl ;
                }
        }

        if ( activeRoom != nilPointer ) activeRoom->activate();
}

void MapManager::beginOldGameWithCharacter( const std::string & roomFile, const std::string & characterName,
                                            int x, int y, int z,
                                            const std::string & direction, const std::string & entry,
                                            bool active )
{
        std::cout << "MapManager::beginOldGameWithCharacter( \""
                        << roomFile << "\", \"" << characterName << "\", "
                        << x << ", " << y << ", " << z << ", \""
                        << direction << "\", \"" << entry << "\", "
                        << ( active ? "true" : "false" ) << " )"
                        << std::endl ;

        Room * room = nilPointer ;

        // if there is already created room, it happens when
        // the room of the second character is the same as of the first character
        if ( activeRoom != nilPointer && activeRoom->getNameOfRoomDescriptionFile() == roomFile )
        {
                room = activeRoom;
        }
        else
        {
                room = getOrBuildRoomByFile( roomFile );
        }

        // place character in room
        if ( room != nilPointer )
        {
                addRoomInPlay( room );

                // create character
                AvatarItemPtr newCharacter = RoomBuilder::createCharacterInRoom (
                                                                room, characterName, true,
                                                                x, y, z, direction, entry );

                addRoomAsVisited( room->getNameOfRoomDescriptionFile () ) ;

                std::string realEntry = entry ;

                if ( realEntry == "just wait" )
                {
                        // it’s the case of resume of saved game
                        // show bubbles only for active character
                        if ( active )
                        {
                                realEntry = "via second teleport";
                        }
                }

                room->activateCharacterByName( characterName );

                newCharacter->autoMoveOnEntry( realEntry );

                // when other character is in the same room as active character then there’s no need to do anything more
                if ( active )
                {
                        activeRoom = room;
                        activeRoom->activate();
                }
        }
}

Room* MapManager::rebuildRoom( Room* room )
{
        if ( room == nilPointer ) return nilPointer ;

        bool isActive = room->isActive();
        room->deactivate();

        std::string fileOfRoom = room->getNameOfRoomDescriptionFile() ;

        // rebuild room
        Room* newRoom = RoomBuilder::buildRoom( ospaths::sharePath() + "map" + ospaths::pathSeparator() + fileOfRoom );
        if ( newRoom == nilPointer ) return nilPointer ;
        newRoom->setConnections( room->getConnections() );

        if ( isRoomInPlay( room ) )
        {
                std::string nameOfActiveCharacter = room->getMediator()->getNameOfActiveCharacter();
                std::string nameOfActiveCharacterBeforeJoining = room->getMediator()->getLastActiveCharacterBeforeJoining();

                std::string theWay( "nowhere" );
                AvatarItemPtr aliveCharacter ;

                // for each character entered this room
                std::vector< AvatarItemPtr > charactersOnEntry = room->getCharactersWhoEnteredRoom ();

        //#ifdef DEBUG
                size_t howManyCharactersEntered = charactersOnEntry.size () ;
                std::cout << "there " ;
                if ( howManyCharactersEntered == 1 ) std::cout << "is " ;
                else                                 std::cout << "are " ;
                std::cout << howManyCharactersEntered << " character" ;
                if ( howManyCharactersEntered != 1 ) std::cout << "s" ;
                std::cout << " who entered room \"" << fileOfRoom << "\"" << std::endl ;
        //#endif

                for ( unsigned int i = 0 ; i < charactersOnEntry.size () ; )
                {
                        const AvatarItemPtr character = charactersOnEntry[ i ];

                        if ( character == nilPointer )
                                std::cerr << "**nil** character among those who entered room \"" << fileOfRoom << "\""
                                                << " @ MapManager::rebuildRoom" << std::endl ;
                        else
                        if ( character->getKind() == "headoverheels" || character->getLives() > 0 )
                        {
                        //#ifdef DEBUG
                                std::cout << "got character \"" << character->getKind() << "\" who entered room \"" << fileOfRoom << "\""
                                                << " @ MapManager::rebuildRoom" << std::endl ;
                        //#endif

                                // when the joined character splits, and then some simple character migrates to another room
                                // and further the player swaps back to the room of splitting, and loses a life there
                                // then don’t rebuild the room with both headoverheels together with the simple character

                                if ( character->getKind() == "headoverheels" && roomsInPlay.size() > 1 )
                                {
                                        std::cout << "some character migrated to another room, and only \"" << nameOfActiveCharacter << "\" is still here" << std::endl ;

                                        // forget the composite character
                                        room->removeCharacterFromRoom( * room->getMediator()->getActiveCharacter(), true );

                                        int characterX = character->getX();
                                        int characterY = character->getY();
                                        int characterZ = character->getZ();
                                        const std::string & characterOrientation = character->getOrientation() ;
                                        const std::string & characterEntry = character->getWayOfEntry() ;

                                        // create the simple character
                                        RoomBuilder::createCharacterInRoom( room,
                                                                            nameOfActiveCharacter, true,
                                                                            characterX, characterY, characterZ,
                                                                            characterOrientation, characterEntry );

                                        // update the list of characters and rewind the loop
                                        charactersOnEntry = room->getCharactersWhoEnteredRoom ();
                                        i = 0 ;
                                        continue ;
                                }

                                theWay = character->getOrientation() ;

                                std::string entry = character->getWayOfEntry() ;

                                // create character
                                aliveCharacter = RoomBuilder::createCharacterInRoom( newRoom,
                                                                                     character->getKind(),
                                                                                     true,
                                                                                     character->getX(), character->getY(), character->getZ(),
                                                                                     theWay, entry );

                                if ( aliveCharacter != nilPointer )
                                        aliveCharacter->autoMoveOnEntry( entry );
                        }

                        ++ i ; // next character
                }

                roomsInPlay.erase( std::remove( roomsInPlay.begin (), roomsInPlay.end (), room ), roomsInPlay.end() );

                if ( aliveCharacter != nilPointer )
                {
                        addRoomInPlay( newRoom );

                        bool characterIsHere = newRoom->activateCharacterByName( nameOfActiveCharacter );

                        if ( ! characterIsHere )
                        {
                                // the case when the composite character was merged in this room
                                // and then loses a life and splits back into the two simple characters
                                characterIsHere = newRoom->activateCharacterByName( nameOfActiveCharacterBeforeJoining );

                                if ( ! characterIsHere ) // unlucky to happen
                                {
                                        if ( newRoom->isAnyCharacterStillInRoom() )
                                        {
                                                newRoom->getMediator()->setActiveCharacter( * newRoom->getCharactersYetInRoom().begin () );
                                        }
                                }
                        }
                }
                // can’t create character, game over
                else
                {
                        delete newRoom ;
                        newRoom = nilPointer ;
                }
        }

        if ( isActive )
        {
                activeRoom = newRoom;
                if ( activeRoom != nilPointer ) activeRoom->activate();
        }

        gameRooms[ fileOfRoom ] = newRoom ;
        delete room ;

        return newRoom;
}

Room* MapManager::changeRoom()
{
        return changeRoom( activeRoom->getMediator()->getActiveCharacter()->getWayOfExit() );
}

Room* MapManager::changeRoom( const std::string& wayOfExit )
{
        Room* previousRoom = this->activeRoom ;

        std::string fileOfPreviousRoom = previousRoom->getNameOfRoomDescriptionFile() ;
        const ConnectedRooms * previousRoomLinks = previousRoom->getConnections() ;

        // look for the next room
        std::string fileOfNextRoom = previousRoomLinks->getConnectedRoomAt( wayOfExit );
        if ( fileOfNextRoom.empty() )
        {       // no room there, so continue with the current one
                return previousRoom ;
        }

        activeRoom->deactivate();
        this->activeRoom = nilPointer;

        SoundManager::getInstance().stopEverySound ();

        const AvatarItem & oldItemOfRoamer = * previousRoom->getMediator()->getActiveCharacter( );

        std::string nameOfRoamer = oldItemOfRoamer.getOriginalKind (); // the original, because the current kind may be "bubbles" when teleporting
        const DescriptionOfItem * descriptionOfRoamer = ItemDescriptions::descriptions ().getDescriptionByKind( nameOfRoamer ) ;

        const int exitX = oldItemOfRoamer.getX ();
        const int exitY = oldItemOfRoamer.getY ();
        const int exitZ = oldItemOfRoamer.getZ ();

        // get the limits of the previous room
        int previousNorthBound = previousRoom->getLimitAt( "north" );
        int previousEastBound = previousRoom->getLimitAt( "east" );
        int previousSouthBound = previousRoom->getLimitAt( "south" );
        int previousWestBound = previousRoom->getLimitAt( "west" );

        const std::string exitOrientation = oldItemOfRoamer.getOrientation() ;

        // remove the active character from the previous room
        previousRoom->removeCharacterFromRoom( oldItemOfRoamer, true );

        if ( ! previousRoom->isAnyCharacterStillInRoom() || nameOfRoamer == "headoverheels" )
        {
                std::cout << "there’re no characters left in room \"" << fileOfPreviousRoom << "\""
                                << " thus bye that room" << std::endl ;

                removeRoomInPlay( previousRoom );
        }

        Room* newRoom = getOrBuildRoomByFile( fileOfNextRoom );
        assert( newRoom != nilPointer );

        std::string wayOfEntry = Way::exitToEntry( wayOfExit ) ;

        if ( ! newRoom->isSingleRoom () )
                wayOfEntry = newRoom->getConnections()->clarifyTheWayOfEntryToABigRoom( wayOfEntry, fileOfPreviousRoom );

        std::cout << "\"" << nameOfRoamer << "\" migrates"
                        << " from the room \"" << fileOfPreviousRoom << "\" at \"" << wayOfExit << "\""
                        << " to the room \"" << fileOfNextRoom << "\" at \"" << wayOfEntry << "\"" << std::endl ;

        addRoomInPlay( newRoom );

        // calculate the entry location in the new room

        std::cout << "the exit coordinates from room \"" << fileOfPreviousRoom << "\" are"
                        << " x=" << exitX << " y=" << exitY << " z=" << exitZ << std::endl ;

        // initially the coordinates of exit from the previous room
        int entryX = exitX ;
        int entryY = exitY ;
        int entryZ = exitZ ;

        bool okayToEnter = newRoom->calculateEntryCoordinates (
                wayOfEntry,
                descriptionOfRoamer->getWidthX(), descriptionOfRoamer->getWidthY(),
                previousNorthBound, previousEastBound, previousSouthBound, previousWestBound,
                &entryX, &entryY, &entryZ
        ) ;
        if ( ! okayToEnter ) {
                std::cout << "coordinates"
                                << " x=" << entryX << " y=" << entryY << " z=" << entryZ
                                << " are not okay to enter room \"" << fileOfNextRoom << "\"" << std::endl ;
        } else {
                std::cout << "the entry coordinates to room \"" << fileOfNextRoom << "\" are"
                                << " x=" << entryX << " y=" << entryY << " z=" << entryZ << std::endl ;
        }

        // create character

        if ( wayOfEntry == "via teleport" || wayOfEntry == "via second teleport" )
                entryZ = Isomot::FloorZ ;

        // no taken item in new room
        GameManager::getInstance().emptyHandbag();

        AvatarItemPtr newItemOfRoamer = RoomBuilder::createCharacterInRoom( newRoom, nameOfRoamer, true,
                                                                            entryX, entryY, entryZ,
                                                                            exitOrientation, wayOfEntry );
        if ( newItemOfRoamer != nilPointer )
                newItemOfRoamer->autoMoveOnEntry( wayOfEntry );

        addRoomAsVisited( newRoom->getNameOfRoomDescriptionFile () ) ;

        newRoom->activateCharacterByName( nameOfRoamer );

        activeRoom = newRoom;
        activeRoom->activate();

        return newRoom;
}

Room* MapManager::getRoomThenAddItToRoomsInPlay( const std::string& roomFile, bool markVisited )
{
        Room* room = getOrBuildRoomByFile( roomFile );

        if ( room != nilPointer )
        {
                if ( markVisited )
                        addRoomAsVisited( room->getNameOfRoomDescriptionFile () ) ;

                addRoomInPlay( room );
        }

        return room;
}

Room* MapManager::swapRoom ()
{
        // swap is possible when there are more than one room
        if ( roomsInPlay.size() > 1 )
        {
                activeRoom->deactivate();

                SoundManager::getInstance().stopEverySound ();

                std::string fileOfPreviousRoom = activeRoom->getNameOfRoomDescriptionFile() ;

                std::vector< Room* >::iterator ri = roomsInPlay.begin ();
                while ( ri != roomsInPlay.end () )
                {
                        if ( *ri == activeRoom ) break;
                        ++ ri;
                }

                // get next room to swap with
                ++ ri;
                // when it’s last one swap with first one
                activeRoom = ( ri != roomsInPlay.end () ? *ri : *roomsInPlay.begin () );

                std::cout << "swop room \"" << fileOfPreviousRoom << "\" with \"" << activeRoom->getNameOfRoomDescriptionFile() << "\"" << std::endl ;

                activeRoom->activate();
        }

        return activeRoom;
}

Room* MapManager::noLivesSwap ()
{
        activeRoom->deactivate();

        Room* inactiveRoom = activeRoom;

        std::vector< Room* >::iterator ri = roomsInPlay.begin ();
        while ( ri != roomsInPlay.end () )
        {
                if ( *ri == activeRoom ) break;
                ++ ri;
        }

        // get next room to swap with
        ++ ri;
        // when it’s last one swap with first one
        activeRoom = ( ri != roomsInPlay.end () ? *ri : *roomsInPlay.begin () );

        // no more rooms, game over
        if ( inactiveRoom == activeRoom )
        {
                activeRoom = nilPointer;
        }

        removeRoomInPlay( inactiveRoom );

        if ( activeRoom != nilPointer )
        {
                activeRoom->activate();
        }

        return activeRoom;
}

void MapManager::addRoomInPlay( Room* whichRoom )
{
        if ( whichRoom == nilPointer ) return ;

        std::string roomFile = whichRoom->getNameOfRoomDescriptionFile();

        if ( isRoomInPlay( whichRoom ) || findRoomInPlayByFile( roomFile ) != nilPointer )
        {
                std::cout << "room \"" << roomFile << "\" is already in play" << std::endl ;
                return ;
        }

        roomsInPlay.push_back( whichRoom );
}

void MapManager::removeRoomInPlay( Room* whichRoom )
{
        if ( whichRoom == nilPointer ) return ;

        whichRoom->deactivate();

        if ( whichRoom == this->activeRoom )
                this->activeRoom = nilPointer;

        roomsInPlay.erase( std::remove( roomsInPlay.begin (), roomsInPlay.end (), whichRoom ), roomsInPlay.end() );

        gameRooms[ whichRoom->getNameOfRoomDescriptionFile() ] = nilPointer ;
        delete whichRoom ;
}

void MapManager::binRoomsInPlay()
{
        while ( ! roomsInPlay.empty () )
        {
                removeRoomInPlay( roomsInPlay.back () ) ;
        }

        this->activeRoom = nilPointer ;
}

Room* MapManager::getRoomOfInactiveCharacter () const
{
        for ( std::vector< Room* >::const_iterator ri = roomsInPlay.begin () ; ri != roomsInPlay.end () ; ++ ri )
        {
                if ( *ri != this->activeRoom )
                {
                        return *ri ;
                }
        }

        return nilPointer ;
}

bool MapManager::isRoomInPlay( const Room* room ) const
{
        for ( std::vector< Room* >::const_iterator ri = roomsInPlay.begin () ; ri != roomsInPlay.end () ; ++ ri )
        {
                if ( *ri == room ) return true ;
        }

        return false ;
}

Room* MapManager::findRoomInPlayByFile( const std::string& roomFile ) const
{
        for ( std::vector< Room* >::const_iterator ri = roomsInPlay.begin () ; ri != roomsInPlay.end () ; ++ ri )
        {
                if ( *ri != nilPointer && ( *ri )->getNameOfRoomDescriptionFile() == roomFile )
                {
                        return *ri ;
                }
        }

        return nilPointer ;
}

Room* MapManager::findRoomByFile( const std::string& roomFile ) const
{
        for ( std::map< std::string, Room * >::const_iterator ri = gameRooms.begin () ; ri != gameRooms.end () ; ++ ri )
        {
                if ( ri->first == roomFile )
                {
                        return ri->second ;
                }
        }

        return nilPointer ;
}

Room* MapManager::getOrBuildRoomByFile( const std::string& roomFile )
{
        if ( gameRooms.empty() || linksBetweenRooms.empty() )
                readMap( ospaths::sharePath() + "map" + ospaths::pathSeparator() + "map.xml" );

        if ( gameRooms.find( roomFile ) != gameRooms.end () )
        {
                if ( gameRooms[ roomFile ] == nilPointer )
                {
                        Room* theRoom = RoomBuilder::buildRoom( ospaths::sharePath() + "map" + ospaths::pathSeparator() + roomFile );
                        if ( theRoom != nilPointer )
                        {
                                theRoom->setConnections( linksBetweenRooms[ roomFile ] );
                                gameRooms[ roomFile ] = theRoom ;
                        }
                }

                return gameRooms[ roomFile ] ;
        }

        return nilPointer ;
}

void MapManager::parseVisitedRooms( const std::vector< std::string >& visitedRooms )
{
        forgetVisitedRooms () ;

        for ( std::vector< std::string >::const_iterator vi = visitedRooms.begin () ; vi != visitedRooms.end () ; ++ vi )
        {
                Room* visitedRoom = getOrBuildRoomByFile( *vi );

                if ( visitedRoom != nilPointer )
                        addRoomAsVisited( visitedRoom->getNameOfRoomDescriptionFile () ) ;
        }
}

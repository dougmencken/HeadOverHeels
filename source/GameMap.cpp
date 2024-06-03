
#include "GameMap.hpp"

#include "RoomMaker.hpp"
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

/*
 * if 1 and buildEveryRoomAtOnce is true, readMap will re-generate
 * the XML descriptions of rooms and write them to home-path/rooms
 */
#define GENERATE_ROOM_DESCRIPTIONS      1


/* static */ GameMap GameMap::instance ;

bool GameMap::buildEveryRoomAtOnce = false ;


/* static */
GameMap & GameMap::getInstance ()
{
        return GameMap::instance ;
}

GameMap::~GameMap( )
{
        clear() ;
}

/* private */ void GameMap::clear ()
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

void GameMap::readMap ( const std::string& fileName )
{
        clear();

        tinyxml2::XMLDocument mapXml ;
        tinyxml2::XMLError result = mapXml.LoadFile( fileName.c_str () );
        if ( result != tinyxml2::XML_SUCCESS ) {
                std::cerr << "can’t load file \"" << fileName << "\" with the map of the game" << std::endl ;
                return;
        }

# if defined( GENERATE_ROOM_DESCRIPTIONS ) && GENERATE_ROOM_DESCRIPTIONS

        std::string pathToRooms = ospaths::homePath () + "rooms" ;
        ospaths::makeFolder( pathToRooms );

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

                std::string fileOfRoom = room->Attribute( "file" );

                ConnectedRooms * connections = new ConnectedRooms() ;

                static const std::string howLinked[ 16 ] = { "north", "east", "south", "west",
                                                                "above", "below", "teleport", "teleport2",
                                                                "northeast", "northwest", "southeast", "southwest",
                                                                "eastnorth", "eastsouth", "westnorth", "westsouth" } ;

                for ( unsigned int h = 0; h < 16; ++ h ) {
                        tinyxml2::XMLElement* linkedRoom = room->FirstChildElement( howLinked[ h ].c_str () ) ;
                        if ( linkedRoom != nilPointer )
                                connections->setConnectedRoomAt( howLinked[ h ], linkedRoom->FirstChild()->ToText()->Value() );
                }

                linksBetweenRooms[ fileOfRoom ] = connections ;

                if ( GameMap::buildEveryRoomAtOnce )
                {
                        // the letters of allegro’s font are 8 x 7
                        const unsigned short widthOfChar = 8 ;
                        const unsigned short heightOfChar = 7 ;
                        const unsigned short interligne = 8 ;

                        const unsigned short offsetY = ( ( heightOfChar + interligne ) << 1 ) + 10 ;
                        const unsigned short atY = GamePreferences::getScreenHeight() - offsetY ;

                        std::string roomInRooms = "making " + util::toStringWithOrdinalSuffix( roomNth )
                                                                + " room out of " + util::number2string( howManyRooms ) ;

                        allegro::Pict::resetWhereToDraw() ;

                        allegro::fillRect( 0, atY - 4, GamePreferences::getScreenWidth(), atY,
                                           Color::blackColor().toAllegroColor() );

                        autouniqueptr< allegro::Pict > stripe( allegro::Pict::newPict( GamePreferences::getScreenWidth(), heightOfChar + interligne ) );

                        allegro::Pict::setWhereToDraw( *stripe ) ;

                        unsigned int textX = ( GamePreferences::getScreenWidth() - roomInRooms.length() * widthOfChar ) >> 1 ;
                        stripe->clearToColor( Color::byName( "blue" ).toAllegroColor() );
                        allegro::textOut( roomInRooms, textX, ( interligne >> 1 ) + 1, Color::byName( "yellow" ).toAllegroColor() );
                        allegro::bitBlit( *stripe, allegro::Pict::theScreen(), 0, atY );

                        textX = ( GamePreferences::getScreenWidth() - fileOfRoom.length() * widthOfChar ) >> 1 ;
                        stripe->clearToColor( Color::byName( "blue" ).toAllegroColor() );
                        allegro::textOut( fileOfRoom, textX, ( interligne >> 1 ) - 1, Color::byName( "green" ).toAllegroColor() );
                        allegro::bitBlit( *stripe, allegro::Pict::theScreen(), 0, atY + ( heightOfChar + interligne ) );

                        allegro::Pict::resetWhereToDraw() ;

                        allegro::fillRect( 0, atY + ( ( heightOfChar + interligne ) << 1 ),
                                           GamePreferences::getScreenWidth(), atY + ( ( heightOfChar + interligne ) << 1 ) + 4,
                                           Color::blackColor().toAllegroColor() );

                        allegro::update();

                        Room* theRoom = RoomMaker::makeRoom( fileOfRoom );
                        theRoom->setConnections( connections );
                        gameRooms[ fileOfRoom ] = theRoom ;

                # if defined( GENERATE_ROOM_DESCRIPTIONS ) && GENERATE_ROOM_DESCRIPTIONS
                        // write the description of room
                        std::string saveTo = ospaths::pathToFile( pathToRooms, theRoom->getNameOfRoomDescriptionFile () );
                        std::cout << "saving the description of room as \"" << saveTo << "\"" << std::endl ;
                        theRoom->saveAsXML( saveTo );
                # endif
                }
                else
                        gameRooms[ fileOfRoom ] = nilPointer ;
        }

        std::cout << "read the map from " << fileName << std::endl ;
}

void GameMap::beginNewGame( const std::string & headRoom, const std::string & heelsRoom )
{
        std::cout << "GameMap::beginNewGame( \"" << headRoom << "\", \"" << heelsRoom << "\" )" << std::endl ;

        clear () ;

        GameManager & gameManager = GameManager::getInstance () ;

        // reset the number of lives and other info
        gameManager.getGameInfo().resetForANewGame () ;

        if ( linksBetweenRooms.empty() )
                readMap( ospaths::pathToFile( ospaths::sharePath() + "map", "map.xml" ) );

        // head’s room

        if ( linksBetweenRooms.find( headRoom ) != linksBetweenRooms.end () )
        {
                Room* firstRoom = getOrBuildRoomByFile( headRoom );

                if ( firstRoom != nilPointer )
                {
                        addRoomInPlay( firstRoom );

                        const DescriptionOfItem & headData = * ItemDescriptions::descriptions ().getDescriptionByKind( "head" );

                        int centerX = firstRoom->getXCenterForItem( headData.getWidthX() );
                        int centerY = firstRoom->getYCenterForItem( headData.getWidthY() );

                        if ( headRoom != heelsRoom )
                        {
                                // create character Head
                                firstRoom->placeCharacterInRoom( "head", true, centerX, centerY, 0, "west" );
                        }
                        else
                        {
                                // create character Head over Heels
                                firstRoom->placeCharacterInRoom( "headoverheels", true, centerX, centerY, 0, "west" );
                        }

                        addRoomAsVisited( firstRoom->getNameOfRoomDescriptionFile () ) ;

                        this->activeRoom = firstRoom ;
                }
        }
        else
                std::cerr << "room \"" << headRoom << "\" doesn’t exist" << std::endl ;

        if ( headRoom != heelsRoom )
        {
                // heels’ room

                if ( linksBetweenRooms.find( heelsRoom ) != linksBetweenRooms.end () )
                {
                        Room* secondRoom = getOrBuildRoomByFile( heelsRoom );

                        if ( secondRoom != nilPointer )
                        {
                                addRoomInPlay( secondRoom );

                                const DescriptionOfItem & heelsData = * ItemDescriptions::descriptions ().getDescriptionByKind( "heels" );

                                int centerX = secondRoom->getXCenterForItem( heelsData.getWidthX() );
                                int centerY = secondRoom->getYCenterForItem( heelsData.getWidthY() );

                                // create character Heels
                                secondRoom->placeCharacterInRoom( "heels", true, centerX, centerY, 0, "south" );
                                //////secondRoom->getMediator()->activateCharacterByName( "heels" );

                                addRoomAsVisited( secondRoom->getNameOfRoomDescriptionFile () ) ;
                        }
                }
                else
                        std::cerr << "room \"" << heelsRoom << "\" doesn’t exist" << std::endl ;
        }

        if ( this->activeRoom != nilPointer ) this->activeRoom->activate() ;
}

void GameMap::beginOldGameWithCharacter( const std::string & roomFile, const std::string & characterName,
                                            int x, int y, int z, const std::string & heading,
                                            bool activeCharacter )
{
        std::cout << "GameMap::beginOldGameWithCharacter( \""
                        << roomFile << "\", \"" << characterName << "\", "
                        << x << ", " << y << ", " << z << ", \"" << heading << "\", "
                        << ( activeCharacter ? "true" : "false" ) << " )"
                        << std::endl ;

        Room * room = nilPointer ;

        if ( this->activeRoom != nilPointer && this->activeRoom->getNameOfRoomDescriptionFile() == roomFile )
                // the room is already created and is active... are both characters in the same room?
                room = this->activeRoom ;
        else
                room = getOrBuildRoomByFile( roomFile );

        // place the character in the room
        if ( room != nilPointer )
        {
                addRoomInPlay( room );

                room->placeCharacterInRoom( characterName, true, x, y, z, heading );

                addRoomAsVisited( room->getNameOfRoomDescriptionFile () ) ;

                if ( activeCharacter ) {
                        // show bubbles only for the active character when resuming a saved game
                        /////// don’t show bubbles for whosoever, they are unseen due to the planets screen anyway
                        /////////newCharacter->setWayOfEntry( "via second teleport" );

                        setActiveRoom( room );
                }
        }
}

void GameMap::rebuildRoom( Room* room )
{
        if ( room == nilPointer ) return ;

        bool wasActive = room->isActive ();
        room->deactivate() ;
        if ( room == this->activeRoom ) this->activeRoom = nilPointer ;

        std::string fileOfRoom = room->getNameOfRoomDescriptionFile() ;

        Room* newRoom = RoomMaker::makeRoom( fileOfRoom );
        assert( newRoom != nilPointer );

        newRoom->setConnections( room->getConnections() );

        if ( isRoomInPlay( room ) )
        {
                bool characterInTheNewRoom = false ;

                const std::string & nameOfActiveCharacter = room->getMediator()->getNameOfActiveCharacter() ;
                const std::string & nameOfActiveCharacterBeforeJoining = room->getMediator()->getLastActiveCharacterBeforeJoining() ;

                // for each character entered this room
                std::vector< AvatarItemPtr > charactersOnEntry = room->getCharactersWhoEnteredRoom ();

        ///#ifdef DEBUG
                size_t howManyCharactersEntered = charactersOnEntry.size () ;
                std::cout << "there " ;
                if ( howManyCharactersEntered == 1 ) std::cout << "is " ;
                else                                 std::cout << "are " ;
                std::cout << howManyCharactersEntered << " character" ;
                if ( howManyCharactersEntered != 1 ) std::cout << "s" ;
                std::cout << " who entered room \"" << fileOfRoom << "\"" << std::endl ;
        ///#endif

                for ( unsigned int i = 0 ; i < charactersOnEntry.size () ; )
                {
                        const AvatarItemPtr character = charactersOnEntry[ i ];
                        assert( character != nilPointer );
                        /** std::cerr << "**nil** " << util::toStringWithOrdinalSuffix( i ) << " character"
                                                << " among those who entered room \"" << fileOfRoom << "\""
                                                << " @ GameMap::rebuildRoom" << std::endl ;
                        continue ; **/

                        if ( character->getKind() == "headoverheels" || character->getLives() > 0 )
                        {
                                std::cout << "character \"" << character->getKind() << "\" entered room \"" << fileOfRoom << "\""
                                                << " @ GameMap::rebuildRoom" << std::endl ;

                                // when the joined character splits, and then some simple character migrates to another room
                                // and further the player swaps back to the room of splitting, and loses a life there
                                // then don’t rebuild the room with both headoverheels together with the simple character

                                if ( character->getKind() == "headoverheels" && roomsInPlay.size() > 1 )
                                {
                                        std::cout << "some character migrated to another room, and only \"" << nameOfActiveCharacter << "\" is still here" << std::endl ;

                                        // forget the composite character ---??? not sure why the old room’s active character is composite ???//////
                                        room->removeCharacterFromRoom( * room->getMediator()->getActiveCharacter(), true );

                                        // create the simple character in the old room
                                        room->placeCharacterInRoom( nameOfActiveCharacter, true,
                                                                        character->getX(), character->getY(), character->getZ(),
                                                                        character->getHeading(), character->getWayOfEntry() );

                                        // update the list of characters and rewind the loop
                                        charactersOnEntry = room->getCharactersWhoEnteredRoom ();
                                        i = 0 ;
                                        continue ;
                                }

                                // place character in the new room
                                characterInTheNewRoom = newRoom->placeCharacterInRoom( character->getKind(), true,
                                                                                        character->getX(), character->getY(), character->getZ(),
                                                                                        character->getHeading(), character->getWayOfEntry() );
                        }

                        ++ i ; // next character
                }

                removeRoomInPlay( room );

                if ( characterInTheNewRoom )
                {
                        addRoomInPlay( newRoom );

                        bool characterIsHere = newRoom->getMediator()->activateCharacterByName( nameOfActiveCharacter );

                        if ( ! characterIsHere ) {
                                // the case when the composite character was merged in this room
                                // and then loses a life and splits back into the two simple characters
                                newRoom->getMediator()->activateCharacterByName( nameOfActiveCharacterBeforeJoining );
                        }
                }
                // can’t create character, game over
                else
                {
                        delete newRoom ;
                        newRoom = nilPointer ;
                }
        }

        if ( wasActive )
                setActiveRoom( newRoom );

        replaceRoomForFile( fileOfRoom, newRoom );
}

Room* GameMap::changeRoom ()
{
        assert( getActiveRoom()->getMediator()->getActiveCharacter() != nilPointer );
        return changeRoom( getActiveRoom()->getMediator()->getActiveCharacter()->getWayOfExit() );
}

Room* GameMap::changeRoom( const std::string & wayOfExit )
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

        std::string nameOfRoamer = oldItemOfRoamer.getOriginalKind (); // the original kind, because the current one may be "bubbles" when teleporting
        const DescriptionOfItem * descriptionOfRoamer = ItemDescriptions::descriptions ().getDescriptionByKind( nameOfRoamer ) ;

        const int exitX = oldItemOfRoamer.getX ();
        const int exitY = oldItemOfRoamer.getY ();
        const int exitZ = oldItemOfRoamer.getZ ();

        // get the limits of the previous room
        int previousNorthBound = previousRoom->getLimitAt( "north" );
        int previousEastBound = previousRoom->getLimitAt( "east" );
        int previousSouthBound = previousRoom->getLimitAt( "south" );
        int previousWestBound = previousRoom->getLimitAt( "west" );

        const std::string exitOrientation( oldItemOfRoamer.getHeading() );

        Room* newRoom = getOrBuildRoomByFile( fileOfNextRoom );
        assert( newRoom != nilPointer );

        std::string wayOfEntry( Way::exitToEntry( wayOfExit ) );

        if ( ! newRoom->isSingleRoom () )
                wayOfEntry = newRoom->getConnections()->clarifyTheWayOfEntryToABigRoom( wayOfEntry, fileOfPreviousRoom );

        std::cout << "\"" << nameOfRoamer << "\" migrates"
                        << " from the room \"" << fileOfPreviousRoom << "\" at \"" << wayOfExit << "\""
                        << " to the room \"" << fileOfNextRoom << "\" at \"" << wayOfEntry << "\"" << std::endl ;

        // remove the active character from the previous room
        previousRoom->removeCharacterFromRoom( oldItemOfRoamer, true );

        if ( ! previousRoom->isAnyCharacterStillInRoom() || nameOfRoamer == "headoverheels" )
        {
                std::cout << "there’re no characters left in room \"" << fileOfPreviousRoom << "\""
                                << " thus bye that room" << std::endl ;

                removeRoomInPlay( previousRoom );
        }

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
        if ( ! okayToEnter )
                std::cout << "coordinates"
                                << " x=" << entryX << " y=" << entryY << " z=" << entryZ
                                << " are not okay to enter room \"" << fileOfNextRoom << "\"" << std::endl ;
        else
                std::cout << "the entry coordinates to room \"" << fileOfNextRoom << "\" are"
                                << " x=" << entryX << " y=" << entryY << " z=" << entryZ << std::endl ;

        // create character

        if ( wayOfEntry == "via teleport" || wayOfEntry == "via second teleport" )
                entryZ = Room::FloorZ ;

        // no taken item in new room
        GameManager::getInstance().emptyHandbag();

        newRoom->placeCharacterInRoom( nameOfRoamer, true, entryX, entryY, entryZ, exitOrientation, "" /*///// wayOfEntry //////*/ );

        addRoomAsVisited( newRoom->getNameOfRoomDescriptionFile () ) ;

        newRoom->getMediator()->activateCharacterByName( nameOfRoamer );

        activeRoom = newRoom;
        activeRoom->activate();

        return newRoom;
}

Room* GameMap::getRoomThenAddItToRoomsInPlay( const std::string& roomFile, bool markVisited )
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

Room* GameMap::swapRoom ()
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

void GameMap::noLivesSwap ()
{
        activeRoom->deactivate();

        Room* inactiveRoom = activeRoom;

        std::vector< Room* >::iterator ri = roomsInPlay.begin ();
        while ( ri != roomsInPlay.end () )
        {
                if ( *ri == activeRoom ) break;
                ++ ri;
        }

        // get the next room to swap with
        ++ ri ;
        // the last one is swapped with the first one
        activeRoom = ( ri != roomsInPlay.end () ? *ri : *roomsInPlay.begin () );

        // no more rooms
        if ( inactiveRoom == activeRoom ) activeRoom = nilPointer;

        removeRoomInPlay( inactiveRoom );

        if ( activeRoom != nilPointer ) activeRoom->activate() ;
}

void GameMap::addRoomInPlay( Room* whichRoom )
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

void GameMap::removeRoomInPlay( Room* whichRoom )
{
        if ( whichRoom == nilPointer ) return ;

        whichRoom->deactivate();

        if ( whichRoom == this->activeRoom )
                this->activeRoom = nilPointer;

        roomsInPlay.erase( std::remove( roomsInPlay.begin (), roomsInPlay.end (), whichRoom ), roomsInPlay.end() );

        replaceRoomForFile( whichRoom->getNameOfRoomDescriptionFile(), nilPointer );
}

void GameMap::binRoomsInPlay()
{
        while ( ! roomsInPlay.empty () )
                removeRoomInPlay( roomsInPlay.back () ) ;

        this->activeRoom = nilPointer ;
}

Room* GameMap::getRoomOfInactiveCharacter () const
{
        for ( std::vector< Room* >::const_iterator ri = roomsInPlay.begin () ; ri != roomsInPlay.end () ; ++ ri )
        {
                if ( *ri != this->activeRoom ) return *ri ;
        }

        return nilPointer ;
}

bool GameMap::isRoomInPlay( const Room* room ) const
{
        for ( std::vector< Room* >::const_iterator ri = roomsInPlay.begin () ; ri != roomsInPlay.end () ; ++ ri )
        {
                if ( *ri == room ) return true ;
        }

        return false ;
}

Room* GameMap::findRoomInPlayByFile( const std::string& roomFile ) const
{
        for ( std::vector< Room* >::const_iterator ri = roomsInPlay.begin () ; ri != roomsInPlay.end () ; ++ ri )
        {
                if ( *ri != nilPointer && ( *ri )->getNameOfRoomDescriptionFile() == roomFile )
                        return *ri ;
        }

        return nilPointer ;
}

Room* GameMap::findRoomByFile( const std::string& roomFile ) const
{
        for ( std::map< std::string, Room * >::const_iterator ri = gameRooms.begin () ; ri != gameRooms.end () ; ++ ri )
        {
                if ( ri->first == roomFile ) return ri->second ;
        }

        return nilPointer ;
}

Room* GameMap::getOrBuildRoomByFile( const std::string & roomFile )
{
        if ( gameRooms.empty() || linksBetweenRooms.empty() )
                readMap( ospaths::pathToFile( ospaths::sharePath() + "map", "map.xml" ) );

        if ( gameRooms.find( roomFile ) != gameRooms.end () )
        {
                if ( gameRooms[ roomFile ] == nilPointer )
                {
                        Room* theRoom = RoomMaker::makeRoom( roomFile );
                        if ( theRoom != nilPointer ) {
                                theRoom->setConnections( linksBetweenRooms[ roomFile ] );
                                gameRooms[ roomFile ] = theRoom ;
                        }
                }

                return gameRooms[ roomFile ] ;
        }

        return nilPointer ;
}

void GameMap::replaceRoomForFile( const std::string & roomFile, Room * room )
{
        Room * previousRoom = findRoomByFile( roomFile );
        if ( previousRoom != nilPointer ) previousRoom->deactivate() ;

        gameRooms[ roomFile ] = room ;

        delete previousRoom ;
}

void GameMap::parseVisitedRooms( const std::vector< std::string >& visitedRooms )
{
        forgetVisitedRooms () ;

        for ( std::vector< std::string >::const_iterator vi = visitedRooms.begin () ; vi != visitedRooms.end () ; ++ vi )
        {
                Room* visitedRoom = getOrBuildRoomByFile( *vi );

                if ( visitedRoom != nilPointer )
                        addRoomAsVisited( visitedRoom->getNameOfRoomDescriptionFile () ) ;
        }
}

void GameMap::getAllRoomFiles ( std::vector< std::string > & whereToCollect )
{
        whereToCollect.reserve( this->gameRooms.size() );

        for ( std::map< std::string, Room * >::const_iterator i = this->gameRooms.begin(), e = this->gameRooms.end() ; i != e ; ++ i )
        {
                whereToCollect.push_back( i->first );
        }
}

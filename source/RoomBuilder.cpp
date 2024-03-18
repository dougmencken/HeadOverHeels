
#include "RoomBuilder.hpp"

#include "MapManager.hpp"
#include "Room.hpp"
#include "Behavior.hpp"
#include "Elevator.hpp"
#include "BonusManager.hpp"
#include "GameManager.hpp"
#include "PoolOfPictures.hpp"
#include "ItemDescriptions.hpp"

#include "ospaths.hpp"

#define MAKE_PARTIAL_FLOOR_TILES        0
#define ALL_DOORLINE_FLOOR_TILES        0


/* static */
Room* RoomBuilder::buildRoom ( const std::string& roomFile )
{
        tinyxml2::XMLDocument roomXml ;
        tinyxml2::XMLError result = roomXml.LoadFile( roomFile.c_str () );
        if ( result != tinyxml2::XML_SUCCESS )
        {
                std::cerr << "can’t read the room file " << roomFile.c_str () << std::endl ;
                return nilPointer;
        }

        tinyxml2::XMLElement* xmlRoot = roomXml.FirstChildElement( "room" );

        const char* sceneryOfRoom = xmlRoot->Attribute( "scenery" );
        std::string scenery( ( sceneryOfRoom != nilPointer ) ? sceneryOfRoom : "" );

        tinyxml2::XMLElement* xTilesElement = xmlRoot->FirstChildElement( "xTiles" ) ;
        tinyxml2::XMLElement* yTilesElement = xmlRoot->FirstChildElement( "yTiles" ) ;

        unsigned int xTiles = std::atoi( xTilesElement->FirstChild()->ToText()->Value() );
        unsigned int yTiles = std::atoi( yTilesElement->FirstChild()->ToText()->Value() );

        std::string roomColor = "white" ;
        tinyxml2::XMLElement* colorElement = xmlRoot->FirstChildElement( "color" ) ;
        if ( colorElement != nilPointer ) roomColor = colorElement->FirstChild()->ToText()->Value() ;

        tinyxml2::XMLElement* floorKind = xmlRoot->FirstChildElement( "floorKind" ) ;
        if ( floorKind == nilPointer ) floorKind = xmlRoot->FirstChildElement( "floorType" ) ;
        std::string kindOfFloor = floorKind->FirstChild()->ToText()->Value() ;
        if ( kindOfFloor == "none" ) kindOfFloor = "absent" ;

        std::string roomName = roomFile;
        const char* fromLastSlash = std::strrchr( roomFile.c_str (), ospaths::pathSeparator()[ 0 ] );
        if ( fromLastSlash != nilPointer )
                roomName = std::string( fromLastSlash + 1 );

        if ( MapManager::getInstance().findRoomByFile( roomName ) == nilPointer )
                std::cout << "building new" ;
        else
                std::cout << "rebuilding" ;

        std::cout << " room \"" << roomName << "\"" << std::endl ;

        Room * theRoom = new Room (
                roomName,
                scenery,
                xTiles, yTiles,
                kindOfFloor
        );

        if ( theRoom == nilPointer )
        {
                std::cerr << "can’t create room \"" << roomFile << "\"" << std::endl ;
                return nilPointer;
        }

#if defined( DEBUG ) && DEBUG
        std::cout << "the color of room is " << Color::byName( roomColor ).toString () << std::endl ;
#endif
        theRoom->setColor( roomColor );

        // add items to room

        std::set< std::pair< int, int > > wallsXY ; // the pieces of walls

        tinyxml2::XMLElement* items = xmlRoot->FirstChildElement( "items" );

        for ( tinyxml2::XMLElement* item = items->FirstChildElement( "item" ) ;
                        item != nilPointer ;
                        item = item->NextSiblingElement( "item" ) )
        {
                int gridCellX = std::atoi( item->Attribute( "x" ) );
                int gridCellY = std::atoi( item->Attribute( "y" ) );
                int gridCellZ = std::atoi( item->Attribute( "z" ) );

                tinyxml2::XMLElement* itemClass = item->FirstChildElement( "class" );
                assert( itemClass != nilPointer );
                std::string whichClass = itemClass->FirstChild()->ToText()->Value() ;

                if ( whichClass == "door" ) // is it a door
                {
                        Door* door = buildDoor( item );

                        if ( door != nilPointer )
                                theRoom->addDoor( door );
                        else
                                std::cout << "oops, can’t build a door at cell " << gridCellX << ", " << gridCellY << ", " << gridCellZ << std::endl ;
                }
                else if ( whichClass == "griditem" ) // is it a grid item
                {
                        std::string kind = item->FirstChildElement( "kind" )->FirstChild()->ToText()->Value();
                        int wallX = std::atoi( item->Attribute( "x" ) );
                        int wallY = std::atoi( item->Attribute( "y" ) );

                        if ( kind.find( "wall" ) != std::string::npos &&
                                ( ( wallY == 0 && kind.find( "-x" ) != std::string::npos ) || ( wallX == 0 && kind.find( "-y" ) != std::string::npos ) ) )
                        {
                                bool onX = ( wallY == 0 && kind.find( "-x" ) != std::string::npos );
                                int index = ( onX ? wallX : wallY );

                                std::cout << "converting grid item \"" << kind << "\" to wall on " << ( onX ? "x:" : "y:" ) << index
                                                << " of room \"" << theRoom->getNameOfRoomDescriptionFile() << "\"" << std::endl ;

                                std::string fileWithPicture = kind + ".png";
                                std::string gfxSet = GameManager::getInstance().getChosenGraphicsSet() ;

                                autouniqueptr< allegro::Pict > picture( allegro::Pict::fromPNGFile (
                                        ospaths::pathToFile( ospaths::sharePath() + gfxSet, fileWithPicture )
                                ) );

                                if ( picture->isNotNil() )
                                {
                                        wallsXY.insert( std::pair< int, int >( onX ? index : 0, onX ? 0 : index ) );

                                        Picture* imageOfWall = new Picture( *picture );
                                        imageOfWall->setName( fileWithPicture );

                                        WallPiece* wallSegment = new WallPiece( onX, index, imageOfWall );

                                        theRoom->addWallPiece( wallSegment );
                                }
                                else
                                {
                                        std::cerr << "picture \"" << fileWithPicture << "\" from \"" << gfxSet << "\" is absent" << std::endl ;
                                }
                        }
                        else
                        {
                                GridItemPtr gridItem = buildGridItem( item, theRoom );

                                if ( gridItem != nilPointer )
                                {
                                        theRoom->addGridItem( gridItem );

                                        if ( gridItem->isSegmentOfWallAlongX () || gridItem->isSegmentOfWallAlongY () )
                                                wallsXY.insert( std::pair< int, int >( gridItem->getCellX(), gridItem->getCellY() ) );
                                }
                                else
                                        std::cout << "oops, can’t build a grid item at cell "
                                                        << gridCellX << ", " << gridCellY << ", " << gridCellZ << std::endl ;
                        }
                }
                else if ( whichClass == "freeitem" ) // is it a free item
                {
                        FreeItemPtr freeItem = buildFreeItem( item, theRoom );

                        if ( freeItem != nilPointer )
                                theRoom->addFreeItem( freeItem );
                        else // there may be bonus item already taken and thus absent
                                std::cout << "free item at cell " << gridCellX << ", " << gridCellY << ", " << gridCellZ << " is absent" << std::endl ;
                }
        }

        // build the floor
        buildFloor( theRoom, xmlRoot );

        // add walls

        tinyxml2::XMLElement* walls = xmlRoot->FirstChildElement( "walls" );
        if ( walls != nilPointer )
        {
                for ( tinyxml2::XMLElement* wall = walls->FirstChildElement( "wall" ) ;
                                wall != nilPointer ;
                                wall = wall->NextSiblingElement( "wall" ) )
                {
                        WallPiece* wallSegment = buildWallPiece( wall );

                        if ( wallSegment != nilPointer )
                        {
                                if ( wallSegment->isAlongX () )
                                        wallsXY.insert( std::pair< int, int >( wallSegment->getPosition(), 0 ) );
                                else
                                        wallsXY.insert( std::pair< int, int >( 0, wallSegment->getPosition() ) );

                                theRoom->addWallPiece( wallSegment );
                        }
                }
        }

        theRoom->convertWallsNearDoors() ;
        theRoom->calculateBounds ();

        std::cout << "built room \"" << theRoom->getNameOfRoomDescriptionFile() << "\"" << std::endl ;

        return theRoom ;
}

/* static */
AvatarItemPtr RoomBuilder::createCharacterInRoom( Room * room,
                                                  const std::string & nameOfCharacter,
                                                  bool justEntered,
                                                  int x, int y, int z,
                                                  const std::string & heading, const std::string & wayOfEntry )
{
        if ( room == nilPointer ) return AvatarItemPtr () ;

        GameInfo & gameInfo = GameManager::getInstance().getGameInfo () ;

        std::string nameOfCharacterToCreate( "none" );

        if ( gameInfo.getLivesByName( nameOfCharacter ) > 0 )
        {
                nameOfCharacterToCreate = nameOfCharacter ;
        }
        else
        if ( nameOfCharacter == "headoverheels" )
        {
                // when the composite character ran out of lives, check if any of the simple characters survived
                if ( gameInfo.getLivesByName( "head" ) > 0 )
                        nameOfCharacterToCreate = "head" ;
                else
                if ( gameInfo.getLivesByName( "heels" ) > 0 )
                        nameOfCharacterToCreate = "heels" ;
        }
        else
        {
                if ( gameInfo.getLivesByName( "head" ) == 0 && gameInfo.getLivesByName( "heels" ) == 0 )
                        nameOfCharacterToCreate = "game over" ;
        }

        const DescriptionOfItem * itemDescription = ItemDescriptions::descriptions().getDescriptionByKind( nameOfCharacterToCreate );

        if ( ( nameOfCharacterToCreate == "headoverheels" || nameOfCharacterToCreate == "head" || nameOfCharacterToCreate == "heels" )
                        && itemDescription != nilPointer )
        {
                if ( gameInfo.getLivesByName( nameOfCharacterToCreate ) > 0 ) {
                        // there are lives left, create the character
                        AvatarItemPtr character( new AvatarItem( *itemDescription, x, y, z, heading ) );
                        assert( character != nilPointer );

                        // automove on entry
                        if ( wayOfEntry.empty() ) {
                                // perhaps the character walks through a door
                                const std::map< std::string, Door* > & doors = room->getDoors() ;
                                for ( std::map< std::string, Door* >::const_iterator di = doors.begin() ; di != doors.end() ; ++ di ) {
                                        Door* door = di->second ;
                                        if ( door != nilPointer && door->isUnderDoor( *character ) ) {
                                                character->setWayOfEntry( di->first );
                                                break ;
                                        }
                                }
                        } else
                                character->setWayOfEntry( wayOfEntry );

                        room->addCharacterToRoom( character, justEntered );

                        return character ;
                }
        }

        return AvatarItemPtr() ;
}

/* static */
void RoomBuilder::buildFloor( Room * room, tinyxml2::XMLElement * xmlRootElement )
{
        assert( room != nilPointer );
        assert( xmlRootElement != nilPointer );

        Door* eastDoor = room->getDoorAt( "east" );
        Door* southDoor = room->getDoorAt( "south" );
        Door* northDoor = room->getDoorAt( "north" );
        Door* westDoor = room->getDoorAt( "west" );

        Door* eastnorthDoor = room->getDoorAt( "eastnorth" );
        Door* eastsouthDoor = room->getDoorAt( "eastsouth" );
        Door* southeastDoor = room->getDoorAt( "southeast" );
        Door* southwestDoor = room->getDoorAt( "southwest" );
        Door* northeastDoor = room->getDoorAt( "northeast" );
        Door* northwestDoor = room->getDoorAt( "northwest" );
        Door* westnorthDoor = room->getDoorAt( "westnorth" );
        Door* westsouthDoor = room->getDoorAt( "westsouth" );

        // read about tiles with no floor in a triple room

        std::set< std::pair< int, int > > tilesWithoutFloor ;

        for ( tinyxml2::XMLElement* nofloor = xmlRootElement->FirstChildElement( "nofloor" ) ;
                        nofloor != nilPointer ;
                        nofloor = nofloor->NextSiblingElement( "nofloor" ) )
        {
                int noFloorX = std::atoi( nofloor->Attribute( "x" ) ) ;
                int noFloorY = std::atoi( nofloor->Attribute( "y" ) ) ;
                std::pair< int, int > noFloorXY( noFloorX, noFloorY ) ;

                tilesWithoutFloor.insert( noFloorXY );
        }

        if ( ! tilesWithoutFloor.empty () )
                room->setTilesWithoutFloor( tilesWithoutFloor );

#if defined( DEBUG ) && DEBUG
        std::cout << "building floor in room \"" << room->getNameOfRoomDescriptionFile() << "\"" ;
#endif

        PoolOfPictures floorImages ;

        if ( room->getScenery() != "" )
        {
                // build the floor automatically

                const std::string sceneryPrefix = room->getScenery() + "-" ;

                int lastTileX = room->getTilesX() - 1 ;
                int lastTileY = room->getTilesY() - 1 ;

                for ( int tileX = 0 ; tileX <= lastTileX ; ++ tileX ) {
                        for ( int tileY = 0 ; tileY <= lastTileY ; ++ tileY )
                        {
                                bool addTile = true ;

                                std::string suffixOfNotFullTile ;

                                if ( ( southDoor != nilPointer && westDoor != nilPointer &&
                                                southDoor->getCellX() == tileX && westDoor->getCellY() == tileY ) ||
                                        ( southwestDoor != nilPointer && westsouthDoor != nilPointer &&
                                                southwestDoor->getCellX() == tileX && westsouthDoor->getCellY() == tileY ) ||
                                        ( southeastDoor != nilPointer && westsouthDoor != nilPointer &&
                                                southeastDoor->getCellX() == tileX && westsouthDoor->getCellY() == tileY ) ||
                                        ( southwestDoor != nilPointer && westnorthDoor != nilPointer &&
                                                southwestDoor->getCellX() == tileX && westnorthDoor->getCellY() == tileY ) ||
                                        ( southeastDoor != nilPointer && westnorthDoor != nilPointer &&
                                                southeastDoor->getCellX() == tileX && westnorthDoor->getCellY() == tileY ) )
                                {
                                        suffixOfNotFullTile = "sw" ;
                                }
                                else if ( ( southDoor != nilPointer && eastDoor != nilPointer &&
                                                southDoor->getCellX() == tileX && eastDoor->getCellY() == tileY &&
                                                        tileX == lastTileX && tileY == 0 )
                                        ||
                                        ( southwestDoor != nilPointer && eastsouthDoor != nilPointer &&
                                                southwestDoor->getCellX() == tileX && eastsouthDoor->getCellY() == tileY &&
                                                        tileX == lastTileX && tileY == 0 )
                                        ||
                                        ( southeastDoor != nilPointer && eastsouthDoor != nilPointer &&
                                                southeastDoor->getCellX() == tileX && eastsouthDoor->getCellY() == tileY &&
                                                        tileX == lastTileX && tileY == 0 )
                                        ||
                                        ( southwestDoor != nilPointer && eastnorthDoor != nilPointer &&
                                                southwestDoor->getCellX() == tileX && eastnorthDoor->getCellY() == tileY &&
                                                        tileX == lastTileX && tileY == 0 )
                                        ||
                                        ( southeastDoor != nilPointer && eastnorthDoor != nilPointer &&
                                                southeastDoor->getCellX() == tileX && eastnorthDoor->getCellY() == tileY &&
                                                        tileX == lastTileX && tileY == 0 ) )
                                {
                                        // no floor tile for the “ se ” corner
                                        continue ;
                                }
                                else if ( ( northDoor != nilPointer && westDoor != nilPointer &&
                                                northDoor->getCellX() == tileX && westDoor->getCellY() == tileY &&
                                                        tileX == 0 && tileY == lastTileY )
                                        ||
                                        ( northwestDoor != nilPointer && westsouthDoor != nilPointer &&
                                                northwestDoor->getCellX() == tileX && westsouthDoor->getCellY() == tileY &&
                                                        tileX == 0 && tileY == lastTileY )
                                        ||
                                        ( northeastDoor != nilPointer && westsouthDoor != nilPointer &&
                                                northeastDoor->getCellX() == tileX && westsouthDoor->getCellY() == tileY &&
                                                        tileX == 0 && tileY == lastTileY )
                                        ||
                                        ( northwestDoor != nilPointer && westnorthDoor != nilPointer &&
                                                northwestDoor->getCellX() == tileX && westnorthDoor->getCellY() == tileY &&
                                                        tileX == 0 && tileY == lastTileY )
                                        ||
                                        ( northeastDoor != nilPointer && westnorthDoor != nilPointer &&
                                                northeastDoor->getCellX() == tileX && westnorthDoor->getCellY() == tileY &&
                                                        tileX == 0 && tileY == lastTileY ) )
                                {
                                        // no floor tile for the “ nw ” corner
                                        continue ;
                                }
                                else if ( ( westDoor != nilPointer && westDoor->getCellY() == tileY )
                                                || ( westnorthDoor != nilPointer && westnorthDoor->getCellY() == tileY )
                                                        || ( westsouthDoor != nilPointer && westsouthDoor->getCellY() == tileY ) )
                                {
                                        suffixOfNotFullTile = "west" ;
                                }
                                else if ( ( southDoor != nilPointer && southDoor->getCellX() == tileX )
                                                || ( southeastDoor != nilPointer && southeastDoor->getCellX() == tileX )
                                                        || ( southwestDoor != nilPointer && southwestDoor->getCellX() == tileX ) )
                                {
                                        suffixOfNotFullTile = "south" ;
                                }
                                else if ( ( eastDoor != nilPointer && eastDoor->getCellY() == tileY )
                                                || ( eastnorthDoor != nilPointer && eastnorthDoor->getCellY() == tileY )
                                                        || ( eastsouthDoor != nilPointer && eastsouthDoor->getCellY() == tileY ) )
                                {
                                        bool isNotFull = true ;

                                        if ( tileY > 0 ) {
                                                // the tile is not full only if the tile further east is without floor
                                                std::pair< int, int > easterTileXY( tileX, tileY - 1 );
                                                isNotFull = ( tilesWithoutFloor.find( easterTileXY ) != tilesWithoutFloor.end() );
                                        }

                                        if ( isNotFull )
                                        {
                                                suffixOfNotFullTile = "east" ;

                        #if !defined( ALL_DOORLINE_FLOOR_TILES ) || ALL_DOORLINE_FLOOR_TILES == 0
                                                //
                                                // add floor tiles only under a door
                                                //
                                                if ( ( eastDoor != nilPointer && tileX != eastDoor->getCellX() && tileX != eastDoor->getCellX() + 1 )
                                                        || ( eastnorthDoor != nilPointer && eastnorthDoor->getCellY() == tileY
                                                                && tileX != eastnorthDoor->getCellX() && tileX != eastnorthDoor->getCellX() + 1 )
                                                        || ( eastsouthDoor != nilPointer && eastsouthDoor->getCellY() == tileY
                                                                && tileX != eastsouthDoor->getCellX() && tileX != eastsouthDoor->getCellX() + 1 ) )
                                                {
                                                        addTile = false ;
                                                }
                        #endif
                                        }
                                }
                                else if ( ( northDoor != nilPointer && northDoor->getCellX() == tileX )
                                                || ( northeastDoor != nilPointer && northeastDoor->getCellX() == tileX )
                                                        || ( northwestDoor != nilPointer && northwestDoor->getCellX() == tileX ) )
                                {
                                        bool isNotFull = true ;

                                        if ( tileX > 0 ) {
                                                // the tile is not full only if the tile further north is without floor
                                                std::pair< int, int > northerTileXY( tileX - 1, tileY );
                                                isNotFull = ( tilesWithoutFloor.find( northerTileXY ) != tilesWithoutFloor.end() );
                                        }

                                        if ( isNotFull )
                                        {
                                                suffixOfNotFullTile = "north" ;

                        #if !defined( ALL_DOORLINE_FLOOR_TILES ) || ALL_DOORLINE_FLOOR_TILES == 0
                                                //
                                                // add floor tiles only under a door
                                                //
                                                if ( ( northDoor != nilPointer && tileY != northDoor->getCellY() && tileY != northDoor->getCellY() + 1 )
                                                        || ( northeastDoor != nilPointer && northeastDoor->getCellX() == tileX
                                                                && tileY != northeastDoor->getCellY() && tileY != northeastDoor->getCellY() + 1 )
                                                        || ( northwestDoor != nilPointer && northwestDoor->getCellX() == tileX
                                                                && tileY != northwestDoor->getCellY() && tileY != northwestDoor->getCellY() + 1 ) )
                                                {
                                                        addTile = false ;
                                                }
                        #endif
                                        }
                                }

                                std::string fileOfFullTile = sceneryPrefix + "floor" ;

                                if ( room->getKindOfFloor() == "mortal" )
                                {
                                        fileOfFullTile = sceneryPrefix + "mortalfloor" ;
                                }
                                else if ( room->getKindOfFloor() == "absent" )
                                {
                                        if ( tileX == lastTileX && tileY == lastTileY )
                                                suffixOfNotFullTile = "sw" ;
                                        else if ( tileX == 0 || tileX == lastTileX )
                                                suffixOfNotFullTile = "x" ;
                                        else if ( tileY == 0 || tileY == lastTileY )
                                                suffixOfNotFullTile = "y" ;
                                        else
                                                continue ;

                                        fileOfFullTile = sceneryPrefix + "nofloor" ;
                                }

                                std::string fileOfTile = suffixOfNotFullTile.empty() ? fileOfFullTile : fileOfFullTile + "-" + suffixOfNotFullTile ;
                                std::string nameOfPicture = fileOfTile ;
                                fileOfTile += ".png" ;
                                fileOfFullTile += ".png" ;

                                std::pair< int, int > tileXY( tileX, tileY );

                                addTile = addTile && ( tilesWithoutFloor.find( tileXY ) == tilesWithoutFloor.end() );

                                if ( addTile )
                                {
                                # if defined( DEBUG ) && DEBUG
                                        std::cout << " tile@" << tileX << "," << tileY ;
                                # endif

                                        if ( ! PoolOfPictures::isPictureThere( fileOfTile ) )
                                                std::cout << "picture \"" << fileOfTile << "\" is *not* there :(" << std::endl ;

                                        if ( floorImages.getOrLoadAndGet( fileOfTile ) == nilPointer )
                                        {
                                                if ( suffixOfNotFullTile.empty () )
                                                {
                                                        floorImages.makePicture( fileOfTile, 64, 40 ) ;
                                                }
                                                else
                                                {
                                                        bool darken = GameManager::getInstance().getCastShadows() ;
                                                        if ( GameManager::getInstance().getChosenGraphicsSet() == "gfx.2003"
                                                                || GameManager::getInstance().getChosenGraphicsSet() == "gfx.riderx" ) darken = false ;

                                                        const PicturePtr& imageOfFullTile = floorImages.getOrLoadAndGetOrMakeAndGet( fileOfFullTile, 64, 40 );
                                                        PicturePtr imageOfPartialTile = FloorTile::fullTileToPartialTile( * imageOfFullTile, suffixOfNotFullTile, darken );
                                                        floorImages.putPicture( fileOfTile, imageOfPartialTile );
                                                }
                                        }

                                        if ( floorImages.hasPicture( fileOfTile ) )
                                        {
                                                const PicturePtr & image = floorImages.getPicture( fileOfTile ) ;
                                                image->setName( nameOfPicture );

                                                if ( image->getHeight() < 40 )
                                                        image->expandOrCropTo( image->getWidth(), 40 );

                                                room->addFloorTile( new FloorTile( tileX, tileY, * image ) );

                                        # if defined( MAKE_PARTIAL_FLOOR_TILES ) && MAKE_PARTIAL_FLOOR_TILES
                                                if ( suffixOfNotFullTile.empty() )
                                                {
                                                        bool darken = GameManager::getInstance().getCastShadows() ;
                                                        if ( GameManager::getInstance().getChosenGraphicsSet() == "gfx.2003" ||
                                                                GameManager::getInstance().getChosenGraphicsSet() == "gfx.riderx" ) darken = false ;

                                                        FloorTile::fullTileToPartialTile( * image, "sw", darken )->saveAsPNG( ospaths::homePath() );
                                                        FloorTile::fullTileToPartialTile( * image, "se", darken )->saveAsPNG( ospaths::homePath() );
                                                        FloorTile::fullTileToPartialTile( * image, "ne", darken )->saveAsPNG( ospaths::homePath() );
                                                        FloorTile::fullTileToPartialTile( * image, "nw", darken )->saveAsPNG( ospaths::homePath() );
                                                        FloorTile::fullTileToPartialTile( * image, "north", darken )->saveAsPNG( ospaths::homePath() );
                                                        FloorTile::fullTileToPartialTile( * image, "south", darken )->saveAsPNG( ospaths::homePath() );
                                                        FloorTile::fullTileToPartialTile( * image, "east", darken )->saveAsPNG( ospaths::homePath() );
                                                        FloorTile::fullTileToPartialTile( * image, "west", darken )->saveAsPNG( ospaths::homePath() );
                                                        FloorTile::fullTileToPartialTile( * image, "full", false )->saveAsPNG( ospaths::homePath() );
                                                        FloorTile::fullTileToPartialTile( * image, "darkfull", true )->saveAsPNG( ospaths::homePath() );
                                                }
                                        # endif
                                        }
                                }
                        }
                }
        }
        else
        {
                tinyxml2::XMLElement* floor = xmlRootElement->FirstChildElement( "floor" );
                if ( floor != nilPointer )
                {
                        for ( tinyxml2::XMLElement* tile = floor->FirstChildElement( "tile" ) ;
                                        tile != nilPointer ;
                                        tile = tile->NextSiblingElement( "tile" ) )
                        {
                                tinyxml2::XMLElement* x = tile->FirstChildElement( "x" );
                                tinyxml2::XMLElement* y = tile->FirstChildElement( "y" );
                                tinyxml2::XMLElement* picture = tile->FirstChildElement( "picture" );

                                int tileX = std::atoi( x->FirstChild()->ToText()->Value() );
                                int tileY = std::atoi( y->FirstChild()->ToText()->Value() );
                                const std::string & imageFile = picture->FirstChild()->ToText()->Value();

                                const PicturePtr & image = floorImages.getOrLoadAndGet( imageFile ) ;

                                if ( image != nilPointer )
                                        room->addFloorTile( new FloorTile( tileX, tileY, * image ) );
                        }
                }
        }

#if defined( DEBUG ) && DEBUG
        std::cout << std::endl ;
#endif
}

/* static */
WallPiece * RoomBuilder::buildWallPiece( tinyxml2::XMLElement* wall )
{
        std::string xy;
        if ( wall->Attribute( "on" ) != nilPointer )
        {
                xy = wall->Attribute( "on" );
        }
        else
        {
                tinyxml2::XMLElement* axis = wall->FirstChildElement( "axis" );
                if ( axis != nilPointer )
                        xy = axis->FirstChild()->ToText()->Value();
        }

        tinyxml2::XMLElement* position = wall->FirstChildElement( "position" );
        if ( position == nilPointer ) position = wall->FirstChildElement( "index" );

        tinyxml2::XMLElement* picture = wall->FirstChildElement( "picture" );
        std::string pictureString = picture->FirstChild()->ToText()->Value();

        autouniqueptr< allegro::Pict > image( allegro::Pict::fromPNGFile (
                ospaths::pathToFile( ospaths::sharePath() + GameManager::getInstance().getChosenGraphicsSet(), pictureString )
        ) );

        if ( image->isNotNil() )
        {
                Picture* imageOfWallPiece = new Picture( *image );
                imageOfWallPiece->setName( pictureString );

                return new WallPiece( xy == "x" ? true : false, std::atoi( position->FirstChild()->ToText()->Value() ), imageOfWallPiece );
        }
        else
                std::cerr << "there’s no picture \"" << pictureString << "\"" << std::endl ;

        return nilPointer;
}

/* static */
GridItemPtr RoomBuilder::buildGridItem( tinyxml2::XMLElement* item, Room* room )
{
        ( void ) room ;

        std::string kind = item->FirstChildElement( "kind" )->FirstChild()->ToText()->Value();
        const DescriptionOfItem * itemDescription = ItemDescriptions::descriptions ().getDescriptionByKind( kind );

        if ( itemDescription != nilPointer )
        {
                int gridCellX = std::atoi( item->Attribute( "x" ) );
                int gridCellY = std::atoi( item->Attribute( "y" ) );
                int gridCellZ = std::atoi( item->Attribute( "z" ) );
                int freeZ = ( gridCellZ > Room::FloorZ ) ? gridCellZ * Room::LayerHeight : Room::FloorZ ;

                std::string heading ;
                tinyxml2::XMLElement* orientation = item->FirstChildElement( "orientation" );
                if ( orientation != nilPointer )
                        heading = orientation->FirstChild()->ToText()->Value();

                //// for backward compatibility
                if ( heading == "none" || heading == "nowhere" ) heading = "" ;

                GridItemPtr gridItem( new GridItem( *itemDescription, gridCellX, gridCellY, freeZ, heading ) );

                std::string behaviorOfItem ;
                tinyxml2::XMLElement* behavior = item->FirstChildElement( "behavior" );
                if ( behavior != nilPointer && behavior->FirstChild() != nilPointer ) {
                        behaviorOfItem = behavior->FirstChild()->ToText()->Value() ;
                        gridItem->setBehaviorOf( behaviorOfItem );
                }

                return gridItem ;
        }

        return GridItemPtr () ;
}

/* static */
FreeItemPtr RoomBuilder::buildFreeItem( tinyxml2::XMLElement* item, Room* room )
{
        std::string kind = item->FirstChildElement( "kind" )->FirstChild()->ToText()->Value();
        const DescriptionOfItem * itemDescription = ItemDescriptions::descriptions ().getDescriptionByKind( kind );

        if ( itemDescription != nilPointer )
        {
                int gridCellX = std::atoi( item->Attribute( "x" ) );
                int gridCellY = std::atoi( item->Attribute( "y" ) );
                int gridCellZ = std::atoi( item->Attribute( "z" ) );

                // convert the location of cell to the free coordinates
                unsigned int oneTileLong = room->getSizeOfOneTile ();
                int freeX = gridCellX * oneTileLong + ( ( oneTileLong - itemDescription->getWidthX() ) >> 1 );
                int freeY = ( gridCellY + 1 ) * oneTileLong - ( ( oneTileLong - itemDescription->getWidthY() ) >> 1 ) - 1 ;
                int freeZ = ( gridCellZ > Room::FloorZ ) ? gridCellZ * Room::LayerHeight : Room::FloorZ ;

                // don’t create an item if it's a bonus that disappears once when taken
                if ( BonusManager::getInstance().isAbsent( BonusInRoom( itemDescription->getKind (), room->getNameOfRoomDescriptionFile() ) ) )
                        return FreeItemPtr () ;

                std::string heading ;
                tinyxml2::XMLElement* orientation = item->FirstChildElement( "orientation" );
                if ( orientation != nilPointer )
                        heading = orientation->FirstChild()->ToText()->Value();

                //// for backward compatibility
                if ( heading == "none" || heading == "nowhere" ) heading = "" ;

                FreeItemPtr freeItem( new FreeItem( *itemDescription, freeX, freeY, freeZ, heading ) );
                freeItem->setInitialCellLocation( gridCellX, gridCellY, gridCellZ );

                std::string behaviorOfItem = "still";
                tinyxml2::XMLElement* behavior = item->FirstChildElement( "behavior" );
                if ( behavior != nilPointer )
                        behaviorOfItem = behavior->FirstChild()->ToText()->Value() ;

                freeItem->setBehaviorOf( behaviorOfItem );

                // more data for the behavior of elevator
                if ( behaviorOfItem == "behavior of elevator" )
                {
                        behaviors::Elevator* behaviorOfElevator = dynamic_cast< behaviors::Elevator* >( freeItem->getBehavior().get () );

                        tinyxml2::XMLElement* extra = item->FirstChildElement( "extra" );
                        if ( extra != nilPointer )
                        {
                                int data[ 3 ];
                                int foundEntries = 0;

                                for ( ; extra != nilPointer ; extra = extra->NextSiblingElement( "extra" ) )
                                {
                                        data[ foundEntries++ ] = std::atoi( extra->FirstChild()->ToText()->Value() );
                                        if ( foundEntries == 3 ) break ;
                                }

                                // three entries are needed
                                if ( foundEntries == 3 )
                                {
                                        behaviorOfElevator->setTop( data[ 0 ] );
                                        behaviorOfElevator->setBottom( data[ 1 ] );
                                        behaviorOfElevator->setAscent( data[ 2 ] != 0 );
                                }
                        }
                        else
                        {
                                tinyxml2::XMLElement* top = item->FirstChildElement( "top" );
                                tinyxml2::XMLElement* bottom = item->FirstChildElement( "bottom" );
                                tinyxml2::XMLElement* ascent = item->FirstChildElement( "ascent" );
                                std::string ascentString = ascent->FirstChild()->ToText()->Value();

                                behaviorOfElevator->setTop( std::atoi( top->FirstChild()->ToText()->Value() ) );
                                behaviorOfElevator->setBottom( std::atoi( bottom->FirstChild()->ToText()->Value() ) );
                                behaviorOfElevator->setAscent( ascentString != "false" );
                        }
                }

                return freeItem ;
        }

        return FreeItemPtr () ;
}

/* static */
Door* RoomBuilder::buildDoor( tinyxml2::XMLElement* item )
{
        std::string kind = item->FirstChildElement( "kind" )->FirstChild()->ToText()->Value();

        int gridCellX = std::atoi( item->Attribute( "x" ) );
        int gridCellY = std::atoi( item->Attribute( "y" ) );
        int gridCellZ = std::atoi( item->Attribute( "z" ) );

        // "z" can't be below the floor, that's less than Room::FloorZ = -1
        int freeZ = ( gridCellZ > Room::FloorZ ) ? gridCellZ * Room::LayerHeight : Room::FloorZ ;

        std::string doorOrientation ;
        tinyxml2::XMLElement* orientation = item->FirstChildElement( "orientation" );
        if ( orientation != nilPointer )
                doorOrientation = orientation->FirstChild()->ToText()->Value() ;
        else {
                // the door's kind is %scenery%-door-%at%
                size_t doorInKind = kind.find( "door-" );
                if ( doorInKind != std::string::npos )
                        doorOrientation = kind.substr( doorInKind + 5 );
        }

        return new Door( kind, gridCellX, gridCellY, freeZ, doorOrientation );
}

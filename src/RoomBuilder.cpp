
#include "RoomBuilder.hpp"
#include "Isomot.hpp"
#include "MapManager.hpp"
#include "Room.hpp"
#include "Behavior.hpp"
#include "Elevator.hpp"
#include "BonusManager.hpp"
#include "GameManager.hpp"
#include "PoolOfPictures.hpp"

#define MAKE_PARTIAL_FLOOR_TILES        0


namespace iso
{

/* static */
Room* RoomBuilder::buildRoom ( const std::string& roomFile )
{
        tinyxml2::XMLDocument roomXml;
        tinyxml2::XMLError result = roomXml.LoadFile( roomFile.c_str () );
        if ( result != tinyxml2::XML_SUCCESS )
        {
                std::cerr << "can’t read room file " << roomFile.c_str () << std::endl ;
                return nilPointer;
        }

        tinyxml2::XMLElement* root = roomXml.FirstChildElement( "room" );

        std::string scenery = "" ;

        if ( root->Attribute( "scenery" ) != nilPointer )
        {
                scenery = root->Attribute( "scenery" ) ;
        }
        else
        {
                tinyxml2::XMLElement* sceneryElement = root->FirstChildElement( "scenery" ) ;

                if ( sceneryElement != nilPointer && sceneryElement->FirstChild() != nilPointer )
                        scenery = sceneryElement->FirstChild()->ToText()->Value() ;
        }

        if ( scenery == "themoon" ) scenery = "moon" ;

        unsigned int tileSize = 16 ;

        if ( root->Attribute( "tileSize" ) != nilPointer )
        {
                tileSize = std::atoi( root->Attribute( "tileSize" ) );
        }
        else
        {
                tinyxml2::XMLElement* tileSizeElement = root->FirstChildElement( "tileSize" ) ;

                if ( tileSizeElement != nilPointer && tileSizeElement->FirstChild() != nilPointer )
                        tileSize = std::atoi( tileSizeElement->FirstChild()->ToText()->Value() );
        }

        tinyxml2::XMLElement* xTilesElement = root->FirstChildElement( "xTiles" ) ;
        tinyxml2::XMLElement* yTilesElement = root->FirstChildElement( "yTiles" ) ;

        unsigned int xTiles = std::atoi( xTilesElement->FirstChild()->ToText()->Value() );
        unsigned int yTiles = std::atoi( yTilesElement->FirstChild()->ToText()->Value() );

        std::string roomColor = "white" ;
        tinyxml2::XMLElement* colorElement = root->FirstChildElement( "color" ) ;
        if ( colorElement != nilPointer ) roomColor = colorElement->FirstChild()->ToText()->Value() ;

        tinyxml2::XMLElement* floorKind = root->FirstChildElement( "floorKind" ) ;
        if ( floorKind == nilPointer ) floorKind = root->FirstChildElement( "floorType" ) ;
        std::string kindOfFloor = floorKind->FirstChild()->ToText()->Value() ;
        if ( kindOfFloor == "none" ) kindOfFloor = "absent" ;

        std::string roomName = roomFile;
        const char* fromLastSlash = std::strrchr( roomFile.c_str (), util::pathSeparator()[ 0 ] );
        if ( fromLastSlash != nilPointer )
                roomName = std::string( fromLastSlash + 1 );

        if ( GameManager::getInstance().getIsomot().getMapManager().findRoomByFile( roomName ) == nilPointer )
                std::cout << "building new" ;
        else
                std::cout << "rebuilding" ;

        std::cout << " room \"" << roomName << "\"" ;
        std::cout << " using data from " << roomFile.c_str () << std::endl ;

        // create room with initial parameters like scenery, dimensions, type of floor
        Room * theRoom = new Room (
                roomName,
                scenery,
                xTiles, yTiles,
                tileSize,
                kindOfFloor
        );

        if ( theRoom == nilPointer )
        {
                std::cerr << "can’t create room \"" << roomFile << "\"" << std::endl ;
                return nilPointer;
        }

#if defined( DEBUG ) && DEBUG
        std::cout << "color of room is " << Color::byName( roomColor ).toString () << std::endl ;
#endif
        theRoom->setColor( roomColor );

        // need to know about presence of doors
        // to calculate coordinates of room’s origin in isometric space
        bool hasNorthDoor = false;
        bool hasEastDoor = false;
        bool hasSouthDoor = false;
        bool hasWestDoor = false;

        tinyxml2::XMLElement* items = root->FirstChildElement( "items" );

        // scroll through list of items to look for doors
        for ( tinyxml2::XMLElement* item = items->FirstChildElement( "item" ) ;
                        item != nilPointer ;
                        item = item->NextSiblingElement( "item" ) )
        {
                tinyxml2::XMLElement* kindOfItem = item->FirstChildElement( "kind" );
                if ( kindOfItem == nilPointer ) kindOfItem = item->FirstChildElement( "type" );
                std::string kind = kindOfItem->FirstChild()->ToText()->Value() ;

                if ( kind == "door" )
                {
                        tinyxml2::XMLElement* orientation = item->FirstChildElement( "orientation" );
                        if ( orientation == nilPointer ) orientation = item->FirstChildElement( "direction" ) ;
                        std::string theWay = orientation->FirstChild()->ToText()->Value();

                        if ( theWay == "north" || theWay == "northeast" || theWay == "northwest" )
                                hasNorthDoor = true;
                        else if ( theWay == "east" || theWay == "eastnorth" || theWay == "eastsouth" )
                                hasEastDoor = true;
                        else if ( theWay == "south" || theWay == "southeast" || theWay == "southwest" )
                                hasSouthDoor = true;
                        else if ( theWay == "west" || theWay == "westnorth" || theWay == "westsouth" )
                                hasWestDoor = true;
                }
        }

        // with knowledge about doors, calculate coordinates of origin
        theRoom->calculateCoordinatesOfOrigin( hasNorthDoor, hasEastDoor, hasSouthDoor, hasWestDoor );

        // for “ triple ” room there’s more data
        tinyxml2::XMLElement* tripleRoomData = root->FirstChildElement( "triple-room-data" );
        if ( tripleRoomData != nilPointer )
        {
                tinyxml2::XMLElement* boundX = tripleRoomData->FirstChildElement( "bound-x" );
                tinyxml2::XMLElement* boundY = tripleRoomData->FirstChildElement( "bound-y" );

                if ( boundX != nilPointer && boundY != nilPointer )
                {
                        theRoom->assignTripleRoomBounds( std::atoi( boundX->Attribute( "minimum" ) ), std::atoi( boundX->Attribute( "maximum" ) ),
                                                         std::atoi( boundY->Attribute( "minimum" ) ), std::atoi( boundY->Attribute( "maximum" ) ) );
                }
        }

        // build walls without doors

        std::vector< std::pair< int, int > > wallsXY ;

        tinyxml2::XMLElement* walls = root->FirstChildElement( "walls" );
        if ( walls != nilPointer )
        {
                for ( tinyxml2::XMLElement* wall = walls->FirstChildElement( "wall" ) ;
                                wall != nilPointer ;
                                wall = wall->NextSiblingElement( "wall" ) )
                {
                        Wall* wallSegment = buildWall( wall, iso::GameManager::getInstance().getChosenGraphicSet() );

                        if ( wallSegment != nilPointer )
                        {
                                if ( wallSegment->isOnX() )
                                        wallsXY.push_back( std::pair< int, int >( wallSegment->getPosition(), 0 ) );
                                else
                                        wallsXY.push_back( std::pair< int, int >( 0, wallSegment->getPosition() ) );

                                theRoom->addWall( wallSegment );
                        }
                }
        }

        // add items to room

        items = root->FirstChildElement( "items" );

        for ( tinyxml2::XMLElement* item = items->FirstChildElement( "item" ) ;
                        item != nilPointer ;
                        item = item->NextSiblingElement( "item" ) )
        {
                int itemX = std::atoi( item->Attribute( "x" ) );
                int itemY = std::atoi( item->Attribute( "y" ) );
                int itemZ = std::atoi( item->Attribute( "z" ) );

                tinyxml2::XMLElement* kindOfItem = item->FirstChildElement( "kind" );
                if ( kindOfItem == nilPointer ) kindOfItem = item->FirstChildElement( "type" );
                std::string kind = kindOfItem->FirstChild()->ToText()->Value() ;

                // it’s a door
                if ( kind == "door" )
                {
                        Door* door = buildDoor( item );

                        if ( door != nilPointer )
                                theRoom->addDoor( door );
                        else
                                std::cout << "oops, can’t build a door with coordinates " << itemX << ", " << itemY << ", " << itemZ << std::endl ;
                }
                // it’s a grid item
                else if ( kind == "griditem" )
                {
                        std::string label = item->FirstChildElement( "label" )->FirstChild()->ToText()->Value();
                        int wallX = std::atoi( item->Attribute( "x" ) );
                        int wallY = std::atoi( item->Attribute( "y" ) );

                        if ( ( label.find( "wall" ) != std::string::npos ) &&
                                ( ( wallY == 0 && label.find( "-x" ) != std::string::npos ) || ( wallX == 0 && label.find( "-y" ) != std::string::npos ) ) )
                        {
                                bool onX = ( wallY == 0 && label.find( "-x" ) != std::string::npos );
                                int index = ( onX ? wallX : wallY );

                                std::cout << "converting grid item \"" << label << "\" to wall on " << ( onX ? "x:" : "y:" ) << index
                                                << " of room \"" << theRoom->getNameOfFileWithDataAboutRoom() << "\"" << std::endl ;

                                std::string fileWithPicture = label + ".png";
                                std::string gfxSet = iso::GameManager::getInstance().getChosenGraphicSet();

                                autouniqueptr< allegro::Pict > picture( allegro::Pict::fromPNGFile (
                                        iso::pathToFile( iso::sharePath() + gfxSet, fileWithPicture )
                                ) );

                                if ( picture->isNotNil() )
                                {
                                        wallsXY.push_back( std::pair< int, int >( onX ? index : 0, onX ? 0 : index ) );

                                        Picture* imageOfWall = new Picture( *picture );
                                        imageOfWall->setName( fileWithPicture );

                                        Wall* wallSegment = new Wall( onX, index, imageOfWall );

                                        theRoom->addWall( wallSegment );
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

                                        if ( gridItem->isSegmentOfWallOnX () || gridItem->isSegmentOfWallOnY () )
                                                wallsXY.push_back( std::pair< int, int >( gridItem->getCellX(), gridItem->getCellY() ) );
                                }
                                else
                                        std::cout << "oops, can’t build a grid item with coordinates " << itemX << ", " << itemY << ", " << itemZ << std::endl ;
                        }
                }
                // it is a free item
                else if ( kind == "freeitem" )
                {
                        FreeItemPtr freeItem = buildFreeItem( item, theRoom );

                        if ( freeItem != nilPointer )
                                theRoom->addFreeItem( freeItem );
                        else // there may be bonus item already taken and thus absent
                                std::cout << "free item with coordinates " << itemX << ", " << itemY << ", " << itemZ << " is absent" << std::endl ;
                }
        }

        Door* eastDoor = theRoom->getDoorAt( "east" );
        Door* southDoor = theRoom->getDoorAt( "south" );
        Door* northDoor = theRoom->getDoorAt( "north" );
        Door* westDoor = theRoom->getDoorAt( "west" );

        Door* eastnorthDoor = theRoom->getDoorAt( "eastnorth" );
        Door* eastsouthDoor = theRoom->getDoorAt( "eastsouth" );
        Door* southeastDoor = theRoom->getDoorAt( "southeast" );
        Door* southwestDoor = theRoom->getDoorAt( "southwest" );
        Door* northeastDoor = theRoom->getDoorAt( "northeast" );
        Door* northwestDoor = theRoom->getDoorAt( "northwest" );
        Door* westnorthDoor = theRoom->getDoorAt( "westnorth" );
        Door* westsouthDoor = theRoom->getDoorAt( "westsouth" );

        // when room has floor and has no door at north and~or east, then north and~or east wall is outside room

        if ( theRoom->hasFloor() )
        {
                std::vector< std::pair< int, int > > outsideWalls ;

                for ( std::vector< std::pair< int, int > >::const_iterator it = wallsXY.begin () ; it != wallsXY.end () ; ++ it )
                {
                        if ( northDoor == nilPointer && northeastDoor == nilPointer && northwestDoor == nilPointer )
                                if ( it->first == 0 )
                                        outsideWalls.push_back( *it );

                        if ( eastDoor == nilPointer && eastnorthDoor == nilPointer && eastsouthDoor == nilPointer )
                                if ( it->second == 0 )
                                        outsideWalls.push_back( *it );
                }

                while ( ! outsideWalls.empty () )
                {
                        std::vector< std::pair< int, int > >::iterator binMe = std::find( wallsXY.begin (), wallsXY.end (), outsideWalls.back () ) ;
                        if ( binMe != wallsXY.end () /* 0,0 may be pushed twice */ ) wallsXY.erase( binMe );
                        outsideWalls.pop_back();
                }
        }

        // read about tiles with no floor

        std::vector< std::pair< int, int > > tilesWithoutFloor ;

        for ( tinyxml2::XMLElement* nofloor = root->FirstChildElement( "nofloor" ) ;
                        nofloor != nilPointer ;
                        nofloor = nofloor->NextSiblingElement( "nofloor" ) )
        {
                int noFloorX = std::atoi( nofloor->Attribute( "x" ) ) ;
                int noFloorY = std::atoi( nofloor->Attribute( "y" ) ) ;
                std::pair< int, int > noFloorXY( noFloorX, noFloorY ) ;

                if ( std::find( tilesWithoutFloor.begin (), tilesWithoutFloor.end (), noFloorXY ) == tilesWithoutFloor.end () &&
                        std::find( wallsXY.begin (), wallsXY.end (), noFloorXY ) == wallsXY.end () )
                {
                        tilesWithoutFloor.push_back( noFloorXY );
                }
        }

        if ( ! tilesWithoutFloor.empty () )
        {
                std::sort( tilesWithoutFloor.begin (), tilesWithoutFloor.end () );
                theRoom->setTilesWithoutFloor( tilesWithoutFloor );
        }

#if defined( DEBUG ) && DEBUG
        std::cout << "building floor in room \"" << theRoom->getNameOfFileWithDataAboutRoom() << "\"" ;
#endif

        PoolOfPictures floorImages ;

        if ( scenery != "" )
        {
                // build floor automatically

                std::string sceneryPrefix = scenery + "-" ;

                int lastTileX = xTiles - 1 ;
                int lastTileY = yTiles - 1 ;

                for ( int tileX = 0 ; tileX <= lastTileX ; ++ tileX )
                {
                        for ( int tileY = 0 ; tileY <= lastTileY ; ++ tileY )
                        {
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
                                        // no floor tile for “ se ” corner
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
                                        // no floor tile for “ nw ” corner
                                        continue ;
                                }
                                else if ( ( eastDoor != nilPointer && eastDoor->getCellY() == tileY &&
                                                /* is under door */ ( eastDoor->getCellX() == tileX || eastDoor->getCellX() + 1 == tileX ) ) ||
                                        ( eastnorthDoor != nilPointer && eastnorthDoor->getCellY() == tileY &&
                                                /* is under door */ ( eastnorthDoor->getCellX() == tileX || eastnorthDoor->getCellX() + 1 == tileX ) ) ||
                                        ( eastsouthDoor != nilPointer && eastsouthDoor->getCellY() == tileY &&
                                                /* is under door */ ( eastsouthDoor->getCellX() == tileX || eastsouthDoor->getCellX() + 1 == tileX ) ) )
                                {
                                        suffixOfNotFullTile = "east" ;
                                }
                                else if ( ( westDoor != nilPointer && westDoor->getCellY() == tileY ) ||
                                        ( westnorthDoor != nilPointer && westnorthDoor->getCellY() == tileY ) ||
                                        ( westsouthDoor != nilPointer && westsouthDoor->getCellY() == tileY ) )
                                {
                                        suffixOfNotFullTile = "west" ;
                                }
                                else if ( ( northDoor != nilPointer && northDoor->getCellX() == tileX &&
                                                /* is under door */ ( northDoor->getCellY() == tileY || northDoor->getCellY() + 1 == tileY ) ) ||
                                        ( northeastDoor != nilPointer && northeastDoor->getCellX() == tileX &&
                                                /* is under door */ ( northeastDoor->getCellY() == tileY || northeastDoor->getCellY() + 1 == tileY ) ) ||
                                        ( northwestDoor != nilPointer && northwestDoor->getCellX() == tileX &&
                                                /* is under door */ ( northwestDoor->getCellY() == tileY || northwestDoor->getCellY() + 1 == tileY ) ) )
                                {
                                        suffixOfNotFullTile = "north" ;
                                }
                                else if ( ( southDoor != nilPointer && southDoor->getCellX() == tileX ) ||
                                        ( southeastDoor != nilPointer && southeastDoor->getCellX() == tileX ) ||
                                        ( southwestDoor != nilPointer && southwestDoor->getCellX() == tileX ) )
                                {
                                        suffixOfNotFullTile = "south" ;
                                }

                                std::string fileOfFullTile = sceneryPrefix + "floor" ;

                                if ( kindOfFloor == "mortal" )
                                {
                                        fileOfFullTile = sceneryPrefix + "mortalfloor" ;
                                }
                                else if ( kindOfFloor == "absent" )
                                {
                                        if ( tileX == lastTileX && tileY == lastTileY )
                                                suffixOfNotFullTile = "sw" ;
                                        else if ( tileX == 0 )
                                                suffixOfNotFullTile = "north" ;
                                        else if ( tileY == 0 )
                                                suffixOfNotFullTile = "east" ;
                                        else if ( tileX == lastTileX )
                                                suffixOfNotFullTile = "south" ;
                                        else if ( tileY == lastTileY )
                                                suffixOfNotFullTile = "west" ;
                                        else
                                                continue ;

                                        fileOfFullTile = sceneryPrefix + "nofloor" ;
                                }

                                std::string fileOfTile = suffixOfNotFullTile.empty() ? fileOfFullTile : fileOfFullTile + "-" + suffixOfNotFullTile ;
                                std::string nameOfPicture = fileOfTile ;
                                fileOfTile += ".png" ;
                                fileOfFullTile += ".png" ;

                                std::pair< int, int > tileXY( tileX, tileY );

                                if ( std::find( tilesWithoutFloor.begin (), tilesWithoutFloor.end (), tileXY ) == tilesWithoutFloor.end () &&
                                        ( std::find( wallsXY.begin (), wallsXY.end (), tileXY ) == wallsXY.end () || ! theRoom->hasFloor() ) )
                                {
                                # if defined( DEBUG ) && DEBUG
                                        std::cout << " tile@" << tileX << "," << tileY ;
                                # endif

                                        if ( floorImages.getOrLoadAndGet( fileOfTile ) == nilPointer )
                                        {
                                                if ( suffixOfNotFullTile.empty () )
                                                {
                                                        floorImages.makePicture( fileOfTile, 64, 40 ) ;
                                                }
                                                else
                                                {
                                                        bool darken = iso::GameManager::getInstance().getCastShadows() ;
                                                        if ( iso::GameManager::getInstance().getChosenGraphicSet() == "gfx.2003" ||
                                                                iso::GameManager::getInstance().getChosenGraphicSet() == "gfx.riderx" ) darken = false ;

                                                        const PicturePtr& imageOfFullTile = floorImages.getOrLoadAndGetOrMakeAndGet( fileOfFullTile, 64, 40 );
                                                        PicturePtr imageOfPartialTile = FloorTile::fullTileToPartialTile( * imageOfFullTile, suffixOfNotFullTile, darken );
                                                        floorImages.putPicture( fileOfTile, imageOfPartialTile );
                                                }
                                        }

                                        if ( floorImages.getPicture( fileOfTile ) != nilPointer )
                                        {
                                                const PicturePtr& image = floorImages.getPicture( fileOfTile ) ;
                                                image->setName( nameOfPicture );

                                                if ( image->getHeight() < 40 )
                                                        image->expandOrCropTo( image->getWidth(), 40 );

                                                theRoom->addFloorTile( new FloorTile( tileX, tileY, * image ) );

                                        # if defined( MAKE_PARTIAL_FLOOR_TILES ) && MAKE_PARTIAL_FLOOR_TILES
                                                if ( suffixOfNotFullTile.empty() )
                                                {
                                                        bool darken = iso::GameManager::getInstance().getCastShadows() ;
                                                        if ( iso::GameManager::getInstance().getChosenGraphicSet() == "gfx.2003" ||
                                                                iso::GameManager::getInstance().getChosenGraphicSet() == "gfx.riderx" ) darken = false ;

                                                        FloorTile::fullTileToPartialTile( * image, "sw", darken )->saveAsPNG( iso::homePath() );
                                                        FloorTile::fullTileToPartialTile( * image, "se", darken )->saveAsPNG( iso::homePath() );
                                                        FloorTile::fullTileToPartialTile( * image, "ne", darken )->saveAsPNG( iso::homePath() );
                                                        FloorTile::fullTileToPartialTile( * image, "nw", darken )->saveAsPNG( iso::homePath() );
                                                        FloorTile::fullTileToPartialTile( * image, "north", darken )->saveAsPNG( iso::homePath() );
                                                        FloorTile::fullTileToPartialTile( * image, "south", darken )->saveAsPNG( iso::homePath() );
                                                        FloorTile::fullTileToPartialTile( * image, "east", darken )->saveAsPNG( iso::homePath() );
                                                        FloorTile::fullTileToPartialTile( * image, "west", darken )->saveAsPNG( iso::homePath() );
                                                        FloorTile::fullTileToPartialTile( * image, "full", false )->saveAsPNG( iso::homePath() );
                                                        FloorTile::fullTileToPartialTile( * image, "darkfull", true )->saveAsPNG( iso::homePath() );
                                                }
                                        # endif
                                        }
                                }
                        }
                }
        }
        else
        {
                tinyxml2::XMLElement* floor = root->FirstChildElement( "floor" );
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
                                std::string imageFile = picture->FirstChild()->ToText()->Value();

                                const PicturePtr& image = floorImages.getOrLoadAndGet( imageFile ) ;

                                if ( image != nilPointer )
                                        theRoom->addFloorTile( new FloorTile( tileX, tileY, * image ) );
                        }
                }
        }

#if defined( DEBUG ) && DEBUG
        std::cout << std::endl ;
#endif

        theRoom->calculateBounds();
        theRoom->updateWallsWithDoors();

        std::cout << "built room \"" << theRoom->getNameOfFileWithDataAboutRoom() << "\"" << std::endl ;

        return theRoom ;
}

/* static */
PlayerItemPtr RoomBuilder::createPlayerInRoom( Room* room,
                                               const std::string& nameOfPlayer,
                                               bool justEntered,
                                               int x, int y, int z,
                                               const std::string& orientation, const std::string& wayOfEntry )
{
        if ( room == nilPointer ) return PlayerItemPtr () ;

        GameManager& gameManager = GameManager::getInstance();

        std::string nameOfPlayerToCreate( nameOfPlayer );

        // when composite player ran out of lives, check if any of simple players still survive
        if ( gameManager.getLives( nameOfPlayer ) == 0 )
        {
                if ( nameOfPlayer == "headoverheels" )
                {
                        if ( gameManager.getLives( "head" ) > 0 )
                        {
                                nameOfPlayerToCreate = "head";
                        }
                        else if ( gameManager.getLives( "heels" ) > 0 )
                        {
                                nameOfPlayerToCreate = "heels";
                        }
                        else
                        {
                                nameOfPlayerToCreate = "nobody";
                        }
                }
                // it is possible that two players join in room and have no lives
                else
                {
                        if ( gameManager.getLives( "head" ) == 0 && gameManager.getLives( "heels" ) == 0 )
                        {
                                nameOfPlayerToCreate = "game over";
                        }
                }
        }

        const DescriptionOfItem* itemDescription = gameManager.getIsomot().getItemDescriptions().getDescriptionByLabel( nameOfPlayerToCreate );

        // if it is found and has some lives left, place it in room
        if ( ( nameOfPlayerToCreate == "headoverheels" || nameOfPlayerToCreate == "head" || nameOfPlayerToCreate == "heels" )
                && itemDescription != nilPointer )
        {
                if ( gameManager.getLives( nameOfPlayerToCreate ) > 0 )
                {
                        PlayerItemPtr player( new PlayerItem( itemDescription, x, y, z, orientation ) );
                        player->fillWithData( gameManager );

                        std::string behaviorOfPlayer = "behavior of some player";

                        if ( nameOfPlayerToCreate == "headoverheels" )
                        {
                                behaviorOfPlayer = "behavior of Head over Heels";
                        }
                        else if ( nameOfPlayerToCreate == "head" )
                        {
                                behaviorOfPlayer = "behavior of Head";
                        }
                        else if ( nameOfPlayerToCreate == "heels" )
                        {
                                behaviorOfPlayer = "behavior of Heels";
                        }

                        player->setBehaviorOf( behaviorOfPlayer );

                        player->setWayOfEntry( wayOfEntry );

                        room->addPlayerToRoom( player, justEntered );

                        return player ;
                }
        }

        return PlayerItemPtr () ;
}

/* static */
Wall* RoomBuilder::buildWall( tinyxml2::XMLElement* wall, const std::string& gfxPrefix )
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
                iso::pathToFile( iso::sharePath() + gfxPrefix, pictureString )
        ) );

        if ( image->isNotNil() )
        {
                Picture* imageOfWall = new Picture( *image );
                imageOfWall->setName( pictureString );

                return new Wall( xy == "x" ? true : false, std::atoi( position->FirstChild()->ToText()->Value() ), imageOfWall );
        }
        else
                std::cerr << "picture \"" << pictureString << "\" from \"" << gfxPrefix << "\" is absent" << std::endl ;

        return nilPointer;
}

/* static */
GridItemPtr RoomBuilder::buildGridItem( tinyxml2::XMLElement* item, Room* room )
{
        ( void ) room ;

        std::string label = item->FirstChildElement( "label" )->FirstChild()->ToText()->Value();

        const DescriptionOfItem* itemDescription = GameManager::getInstance().getIsomot().getItemDescriptions().getDescriptionByLabel( label );

        if ( itemDescription != nilPointer )
        {
                int itemX = std::atoi( item->Attribute( "x" ) );
                int itemY = std::atoi( item->Attribute( "y" ) );
                int itemZ = std::atoi( item->Attribute( "z" ) );

                tinyxml2::XMLElement* orientation = item->FirstChildElement( "orientation" );
                if ( orientation == nilPointer ) orientation = item->FirstChildElement( "direction" ) ;
                std::string theWay = orientation->FirstChild()->ToText()->Value();

                GridItemPtr gridItem( new GridItem( itemDescription,
                                                    itemX, itemY, itemZ > Top ? itemZ * LayerHeight : Top ,
                                                    theWay ) );

                std::string behaviorOfItem = "still";
                tinyxml2::XMLElement* behavior = item->FirstChildElement( "behavior" );
                if ( behavior != nilPointer && behavior->FirstChild() != nilPointer )
                {
                        behaviorOfItem = behavior->FirstChild()->ToText()->Value() ;
                }

                gridItem->setBehaviorOf( behaviorOfItem );

                return gridItem ;
        }

        return GridItemPtr () ;
}

/* static */
FreeItemPtr RoomBuilder::buildFreeItem( tinyxml2::XMLElement* item, Room* room )
{
        std::string label = item->FirstChildElement( "label" )->FirstChild()->ToText()->Value();

        const DescriptionOfItem* itemDescription = GameManager::getInstance().getIsomot().getItemDescriptions().getDescriptionByLabel( label );

        if ( itemDescription != nilPointer )
        {
                int itemX = std::atoi( item->Attribute( "x" ) );
                int itemY = std::atoi( item->Attribute( "y" ) );
                int itemZ = std::atoi( item->Attribute( "z" ) );

                // in free coordinates
                int fx = itemX * room->getSizeOfOneTile() + ( ( room->getSizeOfOneTile() - itemDescription->getWidthX() ) >> 1 );
                int fy = ( itemY + 1 ) * room->getSizeOfOneTile() - ( ( room->getSizeOfOneTile() - itemDescription->getWidthY() ) >> 1 ) - 1;
                int fz = ( itemZ != Top ) ? itemZ * LayerHeight : Top;

                // don’t place an item if it is a bonus and has already been taken
                if ( BonusManager::getInstance().isAbsent( room->getNameOfFileWithDataAboutRoom(), itemDescription->getLabel() ) )
                {
                        return FreeItemPtr () ;
                }

                tinyxml2::XMLElement* orientation = item->FirstChildElement( "orientation" );
                if ( orientation == nilPointer ) orientation = item->FirstChildElement( "direction" ) ;
                std::string theWay = orientation->FirstChild()->ToText()->Value();

                FreeItemPtr freeItem( new FreeItem( itemDescription, fx, fy, fz, theWay ) );

                freeItem->setOriginalCellX( itemX );
                freeItem->setOriginalCellY( itemY );
                freeItem->setOriginalCellZ( itemZ );

                std::string behaviorOfItem = "still";
                tinyxml2::XMLElement* behavior = item->FirstChildElement( "behavior" );
                if ( behavior != nilPointer )
                {
                        behaviorOfItem = behavior->FirstChild()->ToText()->Value() ;
                }

                freeItem->setBehaviorOf( behaviorOfItem );

                // more data for behavior of elevator
                if ( behaviorOfItem == "behavior of elevator" )
                {
                        Elevator* behaviorOfElevator = dynamic_cast< Elevator* >( freeItem->getBehavior() );

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
        std::string label = item->FirstChildElement( "label" )->FirstChild()->ToText()->Value();

        int itemX = std::atoi( item->Attribute( "x" ) );
        int itemY = std::atoi( item->Attribute( "y" ) );
        int itemZ = std::atoi( item->Attribute( "z" ) );

        tinyxml2::XMLElement* orientation = item->FirstChildElement( "orientation" );
        if ( orientation == nilPointer ) orientation = item->FirstChildElement( "direction" ) ;

        return
                new Door( GameManager::getInstance().getIsomot().getItemDescriptions(), label,
                                itemX, itemY, ( itemZ > Top ? itemZ * LayerHeight : Top ),
                                        orientation->FirstChild()->ToText()->Value() );
}

}

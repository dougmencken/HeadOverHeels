
#include "RoomBuilder.hpp"
#include "Isomot.hpp"
#include "MapManager.hpp"
#include "ItemDataManager.hpp"
#include "Room.hpp"
#include "Behavior.hpp"
#include "Elevator.hpp"
#include "BonusManager.hpp"
#include "GameManager.hpp"


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

        tinyxml2::XMLElement* walls = root->FirstChildElement( "walls" );
        if ( walls != nilPointer )
        {
                for ( tinyxml2::XMLElement* wall = walls->FirstChildElement( "wall" ) ;
                                wall != nilPointer ;
                                wall = wall->NextSiblingElement( "wall" ) )
                {
                        Wall* wallSegment = buildWall( wall, iso::GameManager::getInstance().getChosenGraphicSet() );
                        if ( wallSegment != nilPointer ) theRoom->addWall( wallSegment );
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
                                        Picture* imageOfWall = new Picture( *picture.get() );
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
                                        theRoom->addGridItem( gridItem );
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

        // read about tiles with no floor

        std::vector< std::pair< int, int > > tilesWithoutFloor ;

        for ( tinyxml2::XMLElement* nofloor = root->FirstChildElement( "nofloor" ) ;
                        nofloor != nilPointer ;
                        nofloor = nofloor->NextSiblingElement( "nofloor" ) )
        {
                tilesWithoutFloor.push_back( std::pair< int, int >( std::atoi( nofloor->Attribute( "x" ) ), std::atoi( nofloor->Attribute( "y" ) ) ) );
        }

        if ( ! tilesWithoutFloor.empty () )
                theRoom->setTilesWithoutFloor( tilesWithoutFloor );

        std::string gfxPrefix = iso::GameManager::getInstance().getChosenGraphicSet() ;

        std::map< std::string /* imageFile */, PicturePtr /* image */ > floorImages ;

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
                                        suffixOfNotFullTile = "-sw" ;
                                }
                                else if ( ( eastDoor != nilPointer && eastDoor->getCellY() == tileY ) ||
                                        ( eastnorthDoor != nilPointer && eastnorthDoor->getCellY() == tileY ) ||
                                        ( eastsouthDoor != nilPointer && eastsouthDoor->getCellY() == tileY ) )
                                {
                                        suffixOfNotFullTile = "-east" ;
                                }
                                else if ( ( westDoor != nilPointer && westDoor->getCellY() == tileY ) ||
                                        ( westnorthDoor != nilPointer && westnorthDoor->getCellY() == tileY ) ||
                                        ( westsouthDoor != nilPointer && westsouthDoor->getCellY() == tileY ) )
                                {
                                        suffixOfNotFullTile = "-west" ;
                                }
                                else if ( ( northDoor != nilPointer && northDoor->getCellX() == tileX ) ||
                                        ( northeastDoor != nilPointer && northeastDoor->getCellX() == tileX ) ||
                                        ( northwestDoor != nilPointer && northwestDoor->getCellX() == tileX ) )
                                {
                                        suffixOfNotFullTile = "-north" ;
                                }
                                else if ( ( southDoor != nilPointer && southDoor->getCellX() == tileX ) ||
                                        ( southeastDoor != nilPointer && southeastDoor->getCellX() == tileX ) ||
                                        ( southwestDoor != nilPointer && southwestDoor->getCellX() == tileX ) )
                                {
                                        suffixOfNotFullTile = "-south" ;
                                }

                                std::string fileOfTile = sceneryPrefix + "floor" + suffixOfNotFullTile ;

                                if ( kindOfFloor == "mortal" && suffixOfNotFullTile.empty() )
                                        fileOfTile = sceneryPrefix + "mortal-floor" ;

                                if ( kindOfFloor == "absent" )
                                {
                                        if ( tileX == lastTileX && tileY == lastTileY )
                                                suffixOfNotFullTile = "-sw" ;
                                        else if ( tileX == 0 )
                                                suffixOfNotFullTile = "-north" ;
                                        else if ( tileY == 0 )
                                                suffixOfNotFullTile = "-east" ;
                                        else if ( tileX == lastTileX )
                                                suffixOfNotFullTile = "-south" ;
                                        else if ( tileY == lastTileY )
                                                suffixOfNotFullTile = "-west" ;
                                        else
                                                continue ;

                                        fileOfTile = sceneryPrefix + "nofloor" + suffixOfNotFullTile ;
                                }

                                fileOfTile += ".png" ;

                                if ( floorImages.find( fileOfTile ) == floorImages.end () )
                                {
                                        allegro::Pict* picture = allegro::Pict::fromPNGFile( iso::pathToFile( iso::sharePath() + gfxPrefix, fileOfTile ) );

                                        if ( picture->isNotNil() )
                                        {
                                                floorImages[ fileOfTile ] = PicturePtr( new Picture( *picture ) );
                                                floorImages[ fileOfTile ]->setName( fileOfTile );
                                        }
                                        else
                                        {
                                                std::cout << "picture \"" << fileOfTile << "\" from \"" << gfxPrefix << "\" is absent" << std::endl ;

                                                floorImages[ fileOfTile ] = PicturePtr( new Picture( 64, 37 ) ) ;
                                                floorImages[ fileOfTile ]->fillWithTransparencyChequerboard ();
                                                floorImages[ fileOfTile ]->setName( "transparency chequerboard for absent " + fileOfTile );

                                                std::cout << "made \"" << floorImages[ fileOfTile ]->getName() << "\"" << std::endl ;
                                        }

                                        delete picture ;
                                }

                                bool tileHasFloor = true ;

                                for ( size_t i = 0 ; i < tilesWithoutFloor.size () ; ++ i )
                                {
                                        if ( tilesWithoutFloor[ i ].first == tileX && tilesWithoutFloor[ i ].second == tileY )
                                        {
                                                tileHasFloor = false ;
                                                break ;
                                        }
                                }

                                if ( tileHasFloor )
                                {
                                        theRoom->addFloorTile( new FloorTile( tileX, tileY, * floorImages[ fileOfTile ] ) );
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

                                if ( floorImages.find( imageFile ) == floorImages.end () )
                                {
                                        allegro::Pict* picture = allegro::Pict::fromPNGFile( iso::pathToFile( iso::sharePath() + gfxPrefix, imageFile ) );

                                        if ( picture->isNotNil() )
                                        {
                                                floorImages[ imageFile ] = PicturePtr( new Picture( *picture ) );
                                                floorImages[ imageFile ]->setName( imageFile );
                                        }
                                        else
                                        {
                                                std::cerr << "picture \"" << imageFile << "\" from \"" << gfxPrefix << "\" is absent" << std::endl ;
                                                floorImages[ imageFile ] = PicturePtr ();
                                        }

                                        delete picture ;
                                }

                                if ( floorImages[ imageFile ] != nilPointer )
                                        theRoom->addFloorTile( new FloorTile( tileX, tileY, * floorImages[ imageFile ] ) );
                        }
                }
        }

        theRoom->calculateBounds();
        theRoom->updateWallsWithDoors();

        return theRoom ;
}

/* static */
PlayerItemPtr RoomBuilder::createPlayerInRoom( Room* room,
                                               const std::string& nameOfPlayer,
                                               bool justEntered,
                                               int x, int y, int z,
                                               const Way& orientation, const Way& wayOfEntry )
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

        const ItemData* dataOfItem = gameManager.getIsomot().getItemDataManager().findDataByLabel( nameOfPlayerToCreate );

        // if it is found and has some lives left, place it in room
        if ( ( nameOfPlayerToCreate == "headoverheels" || nameOfPlayerToCreate == "head" || nameOfPlayerToCreate == "heels" )
                && dataOfItem != nilPointer )
        {
                if ( gameManager.getLives( nameOfPlayerToCreate ) > 0 )
                {
                        PlayerItemPtr player( new PlayerItem( dataOfItem, x, y, z, orientation ) );
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

                        player->setWayOfEntry( wayOfEntry.toString() );

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
                Picture* imageOfWall = new Picture( *image.get() );
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

        const ItemData* dataOfItem = GameManager::getInstance().getIsomot().getItemDataManager().findDataByLabel( label );

        if ( dataOfItem != nilPointer )
        {
                int itemX = std::atoi( item->Attribute( "x" ) );
                int itemY = std::atoi( item->Attribute( "y" ) );
                int itemZ = std::atoi( item->Attribute( "z" ) );

                tinyxml2::XMLElement* orientation = item->FirstChildElement( "orientation" );
                if ( orientation == nilPointer ) orientation = item->FirstChildElement( "direction" ) ;
                std::string theWay = orientation->FirstChild()->ToText()->Value();

                GridItemPtr gridItem( new GridItem( dataOfItem, itemX, itemY, itemZ > Top ? itemZ * LayerHeight : Top ,
                                                    theWay == "none" ? Way( "nowhere" ) : Way( theWay ) ) );

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

        const ItemData* dataOfItem = GameManager::getInstance().getIsomot().getItemDataManager().findDataByLabel( label );

        if ( dataOfItem != nilPointer )
        {
                int itemX = std::atoi( item->Attribute( "x" ) );
                int itemY = std::atoi( item->Attribute( "y" ) );
                int itemZ = std::atoi( item->Attribute( "z" ) );

                // in free coordinates
                int fx = itemX * room->getSizeOfOneTile() + ( ( room->getSizeOfOneTile() - dataOfItem->getWidthX() ) >> 1 );
                int fy = ( itemY + 1 ) * room->getSizeOfOneTile() - ( ( room->getSizeOfOneTile() - dataOfItem->getWidthY() ) >> 1 ) - 1;
                int fz = ( itemZ != Top ) ? itemZ * LayerHeight : Top;

                // don’t place an item if it is a bonus and has already been taken
                if ( BonusManager::getInstance().isAbsent( room->getNameOfFileWithDataAboutRoom(), dataOfItem->getLabel() ) )
                {
                        return FreeItemPtr () ;
                }

                tinyxml2::XMLElement* orientation = item->FirstChildElement( "orientation" );
                if ( orientation == nilPointer ) orientation = item->FirstChildElement( "direction" ) ;
                std::string theWay = orientation->FirstChild()->ToText()->Value();

                FreeItemPtr freeItem( new FreeItem( dataOfItem, fx, fy, fz,
                                                    theWay == "none" ? Way( "nowhere" ) : Way( theWay ) ) );

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
                new Door( GameManager::getInstance().getIsomot().getItemDataManager(), label,
                                itemX, itemY, ( itemZ > Top ? itemZ * LayerHeight : Top ),
                                        orientation->FirstChild()->ToText()->Value() );
}

}

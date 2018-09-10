
#include "RoomBuilder.hpp"
#include "ItemDataManager.hpp"
#include "Room.hpp"
#include "Behavior.hpp"
#include "FloorTile.hpp"
#include "Wall.hpp"
#include "GridItem.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
#include "Door.hpp"
#include "BonusManager.hpp"
#include "GameManager.hpp"

#include <loadpng.h>


namespace isomot
{

/* static */
Room* RoomBuilder::buildRoom ( const std::string& roomFile )
{
        tinyxml2::XMLDocument roomXml;
        tinyxml2::XMLError result = roomXml.LoadFile( roomFile.c_str () );
        if ( result != tinyxml2::XML_SUCCESS )
        {
                std::cerr << "can’t read file " << roomFile.c_str () << std::endl ;
                return nilPointer;
        }

        std::cout << "building room via data from " << roomFile.c_str () << std::endl ;

        tinyxml2::XMLElement* root = roomXml.FirstChildElement( "room" );

        tinyxml2::XMLElement* scenery = root->FirstChildElement( "scenery" ) ;
        std::string sceneryString = ( scenery != nilPointer ? scenery->FirstChild()->ToText()->Value() : "" );
        tinyxml2::XMLElement* xTiles = root->FirstChildElement( "xTiles" ) ;
        tinyxml2::XMLElement* yTiles = root->FirstChildElement( "yTiles" ) ;
        tinyxml2::XMLElement* width = root->FirstChildElement( "width" ) ;
        tinyxml2::XMLElement* floorType = root->FirstChildElement( "floorType" ) ;

        std::string roomName = roomFile;
        const char* fromLastSlash = std::strrchr( roomFile.c_str (), pathSeparator[ 0 ] );
        if ( fromLastSlash != nilPointer )
                roomName = std::string( fromLastSlash + 1 );

        // create room with initial parameters like scenery, dimensions, type of floor
        Room * theRoom = new Room (
                roomName ,
                sceneryString ,
                std::atoi( xTiles->FirstChild()->ToText()->Value() ),
                std::atoi( yTiles->FirstChild()->ToText()->Value() ),
                std::atoi( width->FirstChild()->ToText()->Value() ),
                floorType->FirstChild()->ToText()->Value()
        );

        if ( theRoom == nilPointer )
        {
                std::cerr << "can’t create room \"" << roomFile << "\"" << std::endl ;
                return nilPointer;
        }

        // to calculate coordinates of origin in isometric space, need to know if room has a door to north and / or east
        bool hasNorthDoor = false;
        bool hasEastDoor = false;

        tinyxml2::XMLElement* items = root->FirstChildElement( "items" );

        // scroll through list of items to look for doors
        for ( tinyxml2::XMLElement* item = items->FirstChildElement( "item" ) ;
                        item != nilPointer ;
                        item = item->NextSiblingElement( "item" ) )
        {
                std::string type = item->FirstChildElement( "type" )->FirstChild()->ToText()->Value() ;

                if ( type == "door" )
                {
                        std::string direction = item->FirstChildElement( "direction" )->FirstChild()->ToText()->Value() ;

                        if ( direction == "north" || direction == "northeast" || direction == "northwest" )
                        {
                                hasNorthDoor = true;
                        }
                        else if ( direction == "east" || direction == "eastnorth" || direction == "eastsouth" )
                        {
                                hasEastDoor = true;
                        }
                }
        }

        // with knowledge about doors, calculate coordinates of origin
        theRoom->calculateCoordinates( hasNorthDoor, hasEastDoor );

        // for “ triple ” room there’s more data
        tinyxml2::XMLElement* tripleRoomData = root->FirstChildElement( "triple-room-data" );
        if ( tripleRoomData != nilPointer )
        {
                tinyxml2::XMLElement* northeast = tripleRoomData->FirstChildElement( "northeast" );
                tinyxml2::XMLElement* southeast = tripleRoomData->FirstChildElement( "southeast" );
                tinyxml2::XMLElement* northwest = tripleRoomData->FirstChildElement( "northwest" );
                tinyxml2::XMLElement* southwest = tripleRoomData->FirstChildElement( "southwest" );
                tinyxml2::XMLElement* eastnorth = tripleRoomData->FirstChildElement( "eastnorth" );
                tinyxml2::XMLElement* eastsouth = tripleRoomData->FirstChildElement( "eastsouth" );
                tinyxml2::XMLElement* westnorth = tripleRoomData->FirstChildElement( "westnorth" );
                tinyxml2::XMLElement* westsouth = tripleRoomData->FirstChildElement( "westsouth" );

                if ( northeast != nilPointer )
                {
                        theRoom->addTripleRoomInitialPoint( Northeast, std::atoi( northeast->Attribute( "x" ) ), std::atoi( northeast->Attribute( "y" ) ) );
                }
                if ( southeast != nilPointer )
                {
                        theRoom->addTripleRoomInitialPoint( Southeast, std::atoi( southeast->Attribute( "x" ) ), std::atoi( southeast->Attribute( "y" ) ) );
                }
                if ( northwest != nilPointer )
                {
                        theRoom->addTripleRoomInitialPoint( Northwest, std::atoi( northwest->Attribute( "x" ) ), std::atoi( northwest->Attribute( "y" ) ) );
                }
                if ( southwest != nilPointer )
                {
                        theRoom->addTripleRoomInitialPoint( Southwest, std::atoi( southwest->Attribute( "x" ) ), std::atoi( southwest->Attribute( "y" ) ) );
                }
                if ( eastnorth != nilPointer )
                {
                        theRoom->addTripleRoomInitialPoint( Eastnorth, std::atoi( eastnorth->Attribute( "x" ) ), std::atoi( eastnorth->Attribute( "y" ) ) );
                }
                if ( eastsouth != nilPointer )
                {
                        theRoom->addTripleRoomInitialPoint( Eastsouth, std::atoi( eastsouth->Attribute( "x" ) ), std::atoi( eastsouth->Attribute( "y" ) ) );
                }
                if ( westnorth != nilPointer )
                {
                        theRoom->addTripleRoomInitialPoint( Westnorth, std::atoi( westnorth->Attribute( "x" ) ), std::atoi( westnorth->Attribute( "y" ) ) );
                }
                if ( westsouth != nilPointer )
                {
                        theRoom->addTripleRoomInitialPoint( Westsouth, std::atoi( westsouth->Attribute( "x" ) ), std::atoi( westsouth->Attribute( "y" ) ) );
                }

                tinyxml2::XMLElement* boundX = tripleRoomData->FirstChildElement( "bound-x" );
                tinyxml2::XMLElement* boundY = tripleRoomData->FirstChildElement( "bound-y" );

                theRoom->assignTripleRoomBounds( std::atoi( boundX->Attribute( "minimum" ) ), std::atoi( boundX->Attribute( "maximum" ) ),
                                                 std::atoi( boundY->Attribute( "minimum" ) ), std::atoi( boundY->Attribute( "maximum" ) ) );
        }

        // build floor

        tinyxml2::XMLElement* floor = root->FirstChildElement( "floor" );
        if ( floor != nilPointer )
        {
                for ( tinyxml2::XMLElement* tile = floor->FirstChildElement( "tile" ) ;
                                tile != nilPointer ;
                                tile = tile->NextSiblingElement( "tile" ) )
                {
                        FloorTile* floorTile = buildFloorTile( tile, isomot::GameManager::getInstance()->getChosenGraphicSet() );
                        theRoom->addFloor( floorTile );
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
                        Wall* wallSegment = buildWall( wall, isomot::GameManager::getInstance()->getChosenGraphicSet() );
                        theRoom->addWall( wallSegment );
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

                std::string type = item->FirstChildElement( "type" )->FirstChild()->ToText()->Value() ;

                // it’s a door
                if ( type == "door" )
                {
                        Door* door = buildDoor( item );

                        if ( door != nilPointer )
                                theRoom->addDoor( door );
                        else
                                std::cout << "oops, can’t build a door with coordinates " << itemX << ", " << itemY << ", " << itemZ << std::endl ;
                }
                // it’s a grid item
                else if ( type == "griditem" )
                {
                        GridItem* gridItem = buildGridItem( item, theRoom );

                        if ( gridItem != nilPointer )
                                theRoom->addGridItem( gridItem );
                        else
                                std::cout << "oops, can’t build a grid item with coordinates " << itemX << ", " << itemY << ", " << itemZ << std::endl ;
                }
                // it is a free item
                else if ( type == "freeitem" )
                {
                        FreeItem* freeItem = buildFreeItem( item, theRoom );

                        if ( freeItem != nilPointer )
                                theRoom->addFreeItem( freeItem );
                        else // there may be bonus item already taken and thus absent
                                std::cout << "free item with coordinates " << itemX << ", " << itemY << ", " << itemZ << " is absent" << std::endl ;
                }
        }

        theRoom->calculateBounds();
        theRoom->updateWallsWithDoors();

        return theRoom ;
}

/* static */
PlayerItem* RoomBuilder::createPlayerInRoom( Room* room,
                                             const std::string& nameOfPlayer,
                                             bool justEntered,
                                             int x, int y, int z,
                                             const Way& orientation, const Way& wayOfEntry )
{
        if ( room == nilPointer ) return nilPointer ;

        GameManager* gameManager = GameManager::getInstance();

        std::string nameOfPlayerToCreate( nameOfPlayer );

        // when composite player ran out of lives, check if any of simple players still survive
        if ( gameManager->getLives( nameOfPlayer ) == 0 )
        {
                if ( nameOfPlayer == "headoverheels" )
                {
                        if ( gameManager->getLives( "head" ) > 0 )
                        {
                                nameOfPlayerToCreate = "head";
                        }
                        else if ( gameManager->getLives( "heels" ) > 0 )
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
                        if ( gameManager->getLives( "head" ) == 0 && gameManager->getLives( "heels" ) == 0 )
                        {
                                nameOfPlayerToCreate = "game over";
                        }
                }
        }

        ItemData* itemData = GameManager::getInstance()->getItemDataManager()->findDataByLabel( nameOfPlayerToCreate );
        PlayerItem* player = nilPointer;

        // if it is found and has some lives left, place it in room
        if ( ( nameOfPlayerToCreate == "headoverheels" || nameOfPlayerToCreate == "head" || nameOfPlayerToCreate == "heels" )
                && itemData != nilPointer )
        {
                if ( gameManager->getLives( nameOfPlayerToCreate ) > 0 )
                {
                        player = new PlayerItem( itemData, x, y, z, orientation );

                        // forget taken item
                        gameManager->emptyHandbag( nameOfPlayerToCreate );

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

                        player->assignBehavior( behaviorOfPlayer, reinterpret_cast< void * >( GameManager::getInstance()->getItemDataManager() ) );

                        player->setWayOfEntry( wayOfEntry.toString() );

                        room->addPlayerToRoom( player, justEntered );
                }
        }

        return player;
}

/* static */
FloorTile* RoomBuilder::buildFloorTile( tinyxml2::XMLElement* tile, const char* gfxPrefix )
{
        tinyxml2::XMLElement* x = tile->FirstChildElement( "x" );
        tinyxml2::XMLElement* y = tile->FirstChildElement( "y" );
        std::string pictureString = tile->FirstChildElement( "picture" )->FirstChild()->ToText()->Value();

        allegro::Pict* picture = allegro::loadPNG( isomot::pathToFile( isomot::sharePath() + gfxPrefix + pathSeparator + pictureString ) );
        if ( picture == nilPointer ) {
                std::cerr << "picture \"" << pictureString << "\" from \"" << gfxPrefix << "\" is absent" << std::endl ;
                return nilPointer;
        }

        return new FloorTile( std::atoi( x->FirstChild()->ToText()->Value() ), std::atoi( y->FirstChild()->ToText()->Value() ), picture );
}

/* static */
Wall* RoomBuilder::buildWall( tinyxml2::XMLElement* wall, const char* gfxPrefix )
{
        tinyxml2::XMLElement* index = wall->FirstChildElement( "index" );
        std::string axisString = wall->FirstChildElement( "axis" )->FirstChild()->ToText()->Value();
        std::string pictureString = wall->FirstChildElement( "picture" )->FirstChild()->ToText()->Value();

        allegro::Pict* picture = allegro::loadPNG( isomot::pathToFile( isomot::sharePath() + gfxPrefix + pathSeparator + pictureString ) );
        if ( picture == nilPointer ) {
                std::cerr << "picture \"" << pictureString << "\" from \"" << gfxPrefix << "\" is absent" << std::endl ;
                return nilPointer;
        }

        return new Wall( axisString == "x" ? true : false, std::atoi( index->FirstChild()->ToText()->Value() ), picture );
}

/* static */
GridItem* RoomBuilder::buildGridItem( tinyxml2::XMLElement* item, Room* room )
{
        GridItem* gridItem = nilPointer;

        std::string label = item->FirstChildElement( "label" )->FirstChild()->ToText()->Value();

        if ( label.find( "wall" ) != std::string::npos )
                std::cout << "grid item \"" << label << "\" is wall of room \"" << room->getNameOfFileWithDataAboutRoom() << "\"" << std::endl ;

        ItemData* itemData = GameManager::getInstance()->getItemDataManager()->findDataByLabel( label );

        if ( itemData != nilPointer )
        {
                int itemX = std::atoi( item->Attribute( "x" ) );
                int itemY = std::atoi( item->Attribute( "y" ) );
                int itemZ = std::atoi( item->Attribute( "z" ) );

                std::string direction = item->FirstChildElement( "direction" )->FirstChild()->ToText()->Value();

                // deal with difference between position from file and position in room
                gridItem = new GridItem( itemData, itemX, itemY, itemZ > Top ? itemZ * LayerHeight : Top ,
                                                direction == "none" ? Way( "nowhere" ) : Way( direction ) );

                std::string behaviorOfItem = "still";
                tinyxml2::XMLElement* behavior = item->FirstChildElement( "behavior" );
                if ( behavior != nilPointer )
                {
                        behaviorOfItem = behavior->FirstChild()->ToText()->Value() ;
                }

                if ( behaviorOfItem == "behavior of disappearance in time" ||
                        behaviorOfItem == "behavior of disappearance on touch" ||
                        behaviorOfItem == "behavior of disappearance on jump into" ||
                        behaviorOfItem == "behavior of slow disappearance on jump into" ||
                        behaviorOfItem == "behavior of disappearance as soon as Head appears" )
                {
                        gridItem->assignBehavior( behaviorOfItem, reinterpret_cast< void * >( GameManager::getInstance()->getItemDataManager()->findDataByLabel( "bubbles" ) ) );
                }
                else
                {
                        gridItem->assignBehavior( behaviorOfItem, nilPointer );
                }
        }

        return gridItem;
}

/* static */
FreeItem* RoomBuilder::buildFreeItem( tinyxml2::XMLElement* item, Room* room )
{
        FreeItem* freeItem = nilPointer;

        std::string label = item->FirstChildElement( "label" )->FirstChild()->ToText()->Value();

        ItemData* itemData = GameManager::getInstance()->getItemDataManager()->findDataByLabel( label );

        if ( itemData != nilPointer )
        {
                int itemX = std::atoi( item->Attribute( "x" ) );
                int itemY = std::atoi( item->Attribute( "y" ) );
                int itemZ = std::atoi( item->Attribute( "z" ) );

                // in free coordinates
                int fx = itemX * room->getSizeOfOneTile() + ( ( room->getSizeOfOneTile() - itemData->getWidthX() ) >> 1 );
                int fy = ( itemY + 1 ) * room->getSizeOfOneTile() - ( ( room->getSizeOfOneTile() - itemData->getWidthY() ) >> 1 ) - 1;
                int fz = itemZ != Top ? itemZ * LayerHeight : Top;

                // don’t place an item if it is a bonus and has already been taken
                if ( BonusManager::getInstance()->isPresent( room->getNameOfFileWithDataAboutRoom(), itemData->getLabel() ) )
                {
                        std::string direction = item->FirstChildElement( "direction" )->FirstChild()->ToText()->Value();

                        freeItem = new FreeItem( itemData, fx, fy, fz,
                                                 direction == "none" ? Way( "nowhere" ) : Way( direction ) );

                        std::string behaviorOfItem = "still";
                        tinyxml2::XMLElement* behavior = item->FirstChildElement( "behavior" );
                        if ( behavior != nilPointer )
                        {
                                behaviorOfItem = behavior->FirstChild()->ToText()->Value() ;
                        }

                        // extra data for behavior of elevator
                        if ( behaviorOfItem == "behavior of elevator" )
                        {
                                int* data = new int[ 3 ];
                                int foundData = 0;

                                for ( tinyxml2::XMLElement* extra = item->FirstChildElement( "extra" ) ;
                                                extra != nilPointer ;
                                                extra = extra->NextSiblingElement( "extra" ) )
                                {
                                        data[ foundData++ ] = std::atoi( extra->FirstChild()->ToText()->Value() );
                                        if ( foundData == 3 ) break ;
                                }

                                // three entries are needed
                                if ( foundData == 3 )
                                {
                                        freeItem->assignBehavior(
                                                behaviorOfItem,
                                                reinterpret_cast< void * >( data )
                                        );
                                }

                                delete data;
                        }
                        else if ( behaviorOfItem == "behavior of waiting hunter in four directions" )
                        {
                                freeItem->assignBehavior(
                                        behaviorOfItem,
                                        reinterpret_cast< void * >( GameManager::getInstance()->getItemDataManager()->findDataByLabel( "imperial-guard" ) )
                                );
                        }
                        else if ( behaviorOfItem == "behavior of something special" ||
                                        behaviorOfItem == "behavior of disappearance in time" ||
                                        behaviorOfItem == "behavior of disappearance on touch" ||
                                        behaviorOfItem == "behavior of disappearance on jump into" ||
                                        behaviorOfItem == "behaivor of final ball" )
                        {
                                freeItem->assignBehavior(
                                        behaviorOfItem,
                                        reinterpret_cast< void * >( GameManager::getInstance()->getItemDataManager()->findDataByLabel( "bubbles" ) )
                                );
                        }
                        else
                        {
                                freeItem->assignBehavior( behaviorOfItem, nilPointer );
                        }
                }
        }

        return freeItem;
}

/* static */
Door* RoomBuilder::buildDoor( tinyxml2::XMLElement* item )
{
        std::string label = item->FirstChildElement( "label" )->FirstChild()->ToText()->Value();

        int itemX = std::atoi( item->Attribute( "x" ) );
        int itemY = std::atoi( item->Attribute( "y" ) );
        int itemZ = std::atoi( item->Attribute( "z" ) );

        std::string direction = item->FirstChildElement( "direction" )->FirstChild()->ToText()->Value();

        return
                new Door( GameManager::getInstance()->getItemDataManager(), label,
                                itemX, itemY, ( itemZ > Top ? itemZ * LayerHeight : Top ),
                                        direction );
}

/* static */
int RoomBuilder::getXCenterOfRoom( ItemData* data, Room* theRoom )
{
        return
                ( ( theRoom->getLimitAt( "south" ) - theRoom->getLimitAt( "north" ) + data->getWidthX() ) >> 1 )
                        + ( theRoom->hasDoorAt( "north" ) ? theRoom->getSizeOfOneTile() >> 1 : 0 )
                                - ( theRoom->hasDoorAt( "south" ) ? theRoom->getSizeOfOneTile() >> 1 : 0 ) ;

}

/* static */
int RoomBuilder::getYCenterOfRoom( ItemData* data, Room* theRoom )
{
        return
                ( ( theRoom->getLimitAt( "west" ) - theRoom->getLimitAt( "east" ) + data->getWidthY() ) >> 1 )
                        + ( theRoom->hasDoorAt( "east" ) ? theRoom->getSizeOfOneTile() >> 1 : 0 )
                                - ( theRoom->hasDoorAt( "west" ) ? theRoom->getSizeOfOneTile() >> 1 : 0 )
                                        - 1 ;
}

}

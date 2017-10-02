
#include "RoomBuilder.hpp"
#include "Exception.hpp"
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

RoomBuilder::RoomBuilder( ItemDataManager* manager ) :
        itemDataManager( manager )
      , room( 0 )
{

}

RoomBuilder::~RoomBuilder( )
{

}

Room* RoomBuilder::buildRoom ( const std::string& fileName )
{
        try
        {
                std::auto_ptr< rxml::RoomXML > roomXML( rxml::room( fileName.c_str () ) );

                // create room with basic parameters like scenery, dimensions, existence of floor
                this->room = new Room( roomXML->name(), roomXML->scenery(), roomXML->xTiles(), roomXML->yTiles(), roomXML->width(), roomXML->floorType() );
                if ( this->room == 0 )
                {
                        std::cerr << "I can't create room \"" << fileName << "\"" << std::endl ;
                        throw "I can't create room \"" + fileName + "\"";
                }

                // to calculate coordinates of origin in isometric space, need to know if room has a door to north and / or east
                bool hasNorthDoor = false;
                bool hasEastDoor = false;

                // scroll through list of items to look for doors
                for ( rxml::items::item_const_iterator i = roomXML->items().item().begin (); i != roomXML->items().item().end (); ++i )
                {
                        if ( ( *i ).type() == rxml::type::door )
                        {
                                if ( ( *i ).direction() == rxml::direction::north ||
                                     ( *i ).direction() == rxml::direction::northeast ||
                                     ( *i ).direction() == rxml::direction::northwest )
                                {
                                        hasNorthDoor = true;
                                }
                                else if ( ( *i ).direction() == rxml::direction::east ||
                                          ( *i ).direction() == rxml::direction::eastnorth ||
                                          ( *i ).direction() == rxml::direction::eastsouth )
                                {
                                        hasEastDoor = true;
                                }
                        }
                }

                // with knowledge of existence of doors, calculate coordinates
                room->calculateCoordinates( hasNorthDoor, hasEastDoor,
                                            roomXML->deltaX().present() ? roomXML->deltaX().get() : 0,
                                            roomXML->deltaY().present() ? roomXML->deltaY().get() : 0 );

                // for "triple" room there’s data to position camera initially
                // and dimensions of room where to move camera
                if ( roomXML->triple_room_data().present() )
                {
                        if ( roomXML->triple_room_data().get().northeast().present() )
                        {
                                rxml::northeast d = roomXML->triple_room_data().get().northeast().get();
                                room->addTripleRoomInitialPoint( Northeast, d.x(), d.y() );
                        }
                        if ( roomXML->triple_room_data().get().southeast().present() )
                        {
                                rxml::southeast d = roomXML->triple_room_data().get().southeast().get();
                                room->addTripleRoomInitialPoint( Southeast, d.x(), d.y() );
                        }
                        if ( roomXML->triple_room_data().get().northwest().present() )
                        {
                                rxml::northwest d = roomXML->triple_room_data().get().northwest().get();
                                room->addTripleRoomInitialPoint( Northwest, d.x(), d.y() );
                        }
                        if ( roomXML->triple_room_data().get().southwest().present() )
                        {
                                rxml::southwest d = roomXML->triple_room_data().get().southwest().get();
                                room->addTripleRoomInitialPoint( Southwest, d.x(), d.y() );
                        }
                        if ( roomXML->triple_room_data().get().eastnorth().present() )
                        {
                                rxml::eastnorth d = roomXML->triple_room_data().get().eastnorth().get();
                                room->addTripleRoomInitialPoint( Eastnorth, d.x(), d.y() );
                        }
                        if ( roomXML->triple_room_data().get().eastsouth().present() )
                        {
                                rxml::eastsouth d = roomXML->triple_room_data().get().eastsouth().get();
                                room->addTripleRoomInitialPoint( Eastsouth, d.x(), d.y() );
                        }
                        if ( roomXML->triple_room_data().get().westnorth().present() )
                        {
                                rxml::westnorth d = roomXML->triple_room_data().get().westnorth().get();
                                room->addTripleRoomInitialPoint( Westnorth, d.x(), d.y() );
                        }
                        if ( roomXML->triple_room_data().get().westsouth().present() )
                        {
                                rxml::westsouth d = roomXML->triple_room_data().get().westsouth().get();
                                room->addTripleRoomInitialPoint( Westsouth, d.x(), d.y() );
                        }

                        room->assignTripleRoomBounds( roomXML->triple_room_data().get().bound_x().minimum(),
                                                      roomXML->triple_room_data().get().bound_x().maximum(),
                                                      roomXML->triple_room_data().get().bound_y().minimum(),
                                                      roomXML->triple_room_data().get().bound_y().maximum() );
                }

                // build tile-based floor
                for ( rxml::floor::tile_const_iterator i = roomXML->floor().tile().begin (); i != roomXML->floor().tile().end (); ++i )
                {
                        FloorTile* floorTile = this->buildFloorTile( *i, isomot::GameManager::getInstance()->getChosenGraphicSet() );
                        room->addFloor( floorTile );
                }

                // build walls without doors
                if ( roomXML->walls().present() )
                {
                        for ( rxml::walls::wall_const_iterator i = roomXML->walls().get().wall().begin (); i != roomXML->walls().get().wall().end (); ++i )
                        {
                                Wall* wall = this->buildWall( *i, isomot::GameManager::getInstance()->getChosenGraphicSet() );
                                room->addWall( wall );
                        }
                }

                // place items in the room
                // element may be a wall, a door, a grid one or a free one
                for ( rxml::items::item_const_iterator i = roomXML->items().item().begin (); i != roomXML->items().item().end (); ++i )
                {
                        // it’s a plain wall
                        if ( ( *i ).type () == rxml::type::wall )
                        {
                        }
                        // it’s a door
                        else if ( ( *i ).type () == rxml::type::door )
                        {
                                Door* door = this->buildDoor( *i );
                                if ( door == 0 )
                                {
                                        std::ostringstream oss;
                                        oss << "oops, can’t build a door with coordinates " << ( *i ).x () << ", " << ( *i ).y () << ", " << ( *i ).z () ;
                                        std::cout << oss.str () << std::endl ;
                                        /// throw oss.str ();
                                }
                                room->addDoor( door );
                        }
                        // it’s a grid item
                        else if( ( *i ).type () == rxml::type::griditem )
                        {
                                GridItem* gridItem = this->buildGridItem( *i );
                                if ( gridItem == 0 )
                                {
                                        std::ostringstream oss;
                                        oss << "oops, can’t build a grid item with coordinates " << ( *i ).x () << ", " << ( *i ).y () << ", " << ( *i ).z () ;
                                        std::cout << oss.str () << std::endl ;
                                        /// throw oss.str ();
                                }
                                room->addGridItem( gridItem );
                        }
                        // it is a free item
                        else if ( ( *i ).type () == rxml::type::freeitem )
                        {
                                FreeItem* freeItem = this->buildFreeItem( *i );
                                if ( freeItem == 0 )
                                {
                                        std::ostringstream oss;
                                        oss << "free item with coordinates " << ( *i ).x () << ", " << ( *i ).y () << ", " << ( *i ).z () << " is absent";
                                        std::cout << oss.str () << std::endl ;
                                        // don't throw an exception here
                                        // there may be bonus item already taken and thus absent
                                }
                                else
                                {
                                        room->addFreeItem( freeItem );
                                }
                        }
                }

                room->calculateBounds();
        }
        catch ( const xml_schema::exception& e )
        {
                std::cout << e << std::endl ;
        }
        catch ( const Exception& e )
        {
                std::cout << e.what () << std::endl ;
        }

        return this->room;
}

PlayerItem* RoomBuilder::createPlayerInTheSameRoom( bool justEntered,
                                                        const std::string& nameOfPlayer,
                                                        const std::string& behavior,
                                                        int x, int y, int z,
                                                        bool withItem,
                                                        const Direction& direction, const Direction& entry )
{
        return createPlayerInRoom( this->room, justEntered, nameOfPlayer, behavior, x, y, z, withItem, direction, entry );
}

PlayerItem* RoomBuilder::createPlayerInRoom( Room* room, bool justEntered,
                                                        const std::string& nameOfPlayer,
                                                        const std::string& behavior,
                                                        int x, int y, int z,
                                                        bool withItem,
                                                        const Direction& direction, const Direction& entry )
{
        GameManager* gameManager = GameManager::getInstance();

        std::string newNameOfPlayer( nameOfPlayer );
        std::string newBehaviorOfPlayer( behavior );

        // when composite player ran out of lives, check if any of simple players still survive
        if ( gameManager->getLives( nameOfPlayer ) == 0 )
        {
                if ( nameOfPlayer == "headoverheels" )
                {
                        if ( gameManager->getLives( "head" ) > 0 )
                        {
                                newNameOfPlayer = "head";
                                newBehaviorOfPlayer = "behavior of Head";
                        }
                        else if ( gameManager->getLives( "heels" ) > 0 )
                        {
                                newNameOfPlayer = "heels";
                                newBehaviorOfPlayer = "behavior of Heels";
                        }
                        else
                        {
                                newNameOfPlayer = "in~buildroom";
                        }
                }
                // it is possible that two players join in room and have no lives
                else
                {
                        if ( gameManager->getLives( "head" ) == 0 && gameManager->getLives( "heels" ) == 0 )
                        {
                                newNameOfPlayer = "no~lives";
                        }
                }
        }

        ItemData* itemData = this->itemDataManager->findItemByLabel( newNameOfPlayer );
        PlayerItem* player = 0;

        // if it is found and has some lives left, place it in room
        if ( ( newNameOfPlayer == "headoverheels" || newNameOfPlayer == "head" || newNameOfPlayer == "heels" ) && itemData != 0 )
        {
                if ( gameManager->getLives( newNameOfPlayer ) > 0 )
                {
                        player = new PlayerItem( itemData, x, y, z, direction );

                        // don’t move taken item to other room
                        if ( withItem )
                        {
                                gameManager->setItemTaken( 0 );
                        }

                        player->fillWithData( gameManager );

                        player->assignBehavior( newBehaviorOfPlayer, reinterpret_cast< void * >( itemDataManager ) );

                        player->setDirectionOfEntry( entry );

                        room->addPlayerToRoom( player, justEntered );
                }
        }

        return player;
}

FloorTile* RoomBuilder::buildFloorTile( const rxml::tile& tile, const char* gfxPrefix )
{
        FloorTile* floorTile = 0;

        try {
                BITMAP* picture = load_png( ( isomot::sharePath() + gfxPrefix + "/" + tile.bitmap() ).c_str(), 0 );
                if ( picture == 0 ) {
                        std::cerr << "picture \"" << tile.bitmap() << "\" at \"" << gfxPrefix << "\" is absent" << std::endl ;
                        throw "picture " + tile.bitmap() + " at " + gfxPrefix + " is absent";
                }
                int column = room->getTilesX() * tile.y() + tile.x();
                floorTile = new FloorTile( column, tile.x(), tile.y(), picture );
                floorTile->setOffset( tile.offsetX(), tile.offsetY() );
        } catch ( const Exception& e ) {
                std::cerr << e.what () << std::endl ;
        }

        return floorTile;
}

Wall* RoomBuilder::buildWall( const rxml::wall& wall, const char* gfxPrefix )
{
        Wall* roomWall = 0;

        try {
                BITMAP* picture = load_png( ( isomot::sharePath() + gfxPrefix + "/" + wall.bitmap() ).c_str(), 0 );
                if ( picture == 0 ) {
                        std::cerr << "picture \"" << wall.bitmap() << "\" at \"" << gfxPrefix << "\" is absent" << std::endl ;
                        throw "picture " + wall.bitmap() + " at " + gfxPrefix + " is absent";
                }
                roomWall = new Wall( wall.axis() == rxml::axis::x ? AxisX : AxisY, wall.index(), picture );
        } catch ( const Exception& e ) {
                std::cerr << e.what () << std::endl ;
        }

        return roomWall;
}

GridItem* RoomBuilder::buildGridItem( const rxml::item& item )
{
        GridItem* gridItem = 0;
        ItemData* itemData = this->itemDataManager->findItemByLabel( item.label () );

        // when found place item in room
        if ( itemData != 0 )
        {
                // deal with difference between position from file and position in room
                gridItem = new GridItem( itemData, item.x(), item.y(), ( item.z() > Top ? item.z() * LayerHeight : Top ),
                item.direction() == rxml::direction::none ? NoDirection : Direction( item.direction() - 1 ) );

                std::string behaviorOfItem = item.behavior ();
                if ( behaviorOfItem.empty () )
                        behaviorOfItem = "still";

                if ( behaviorOfItem == "behavior of disappearance in time" ||
                        behaviorOfItem == "behavior of disappearance on touch" ||
                        behaviorOfItem == "behavior of disappearance on jump into" ||
                        behaviorOfItem == "behavior of slow disappearance on jump into" ||
                        behaviorOfItem == "behavior of disappearance as soon as Head appears" )
                {
                        gridItem->assignBehavior( behaviorOfItem, reinterpret_cast< void * >( this->itemDataManager->findItemByLabel( "bubbles" ) ) );
                }
                else
                {
                        gridItem->assignBehavior( behaviorOfItem, 0 );
                }
        }

        return gridItem;
}

FreeItem* RoomBuilder::buildFreeItem( const rxml::item& item )
{
        FreeItem* freeItem = 0;

        ItemData* itemData = this->itemDataManager->findItemByLabel( item.label () );

        // if item is here, place it in room
        if ( itemData != 0 )
        {
                // in free coordinates
                int fx = item.x() * room->getSizeOfOneTile() + ( ( room->getSizeOfOneTile() - itemData->widthX ) >> 1 );
                int fy = ( item.y() + 1 ) * room->getSizeOfOneTile() - ( ( room->getSizeOfOneTile() - itemData->widthY ) >> 1 ) - 1;
                int fz = item.z() != Top ? item.z() * LayerHeight : Top;

                // don’t place an item if it is a bonus and has already been taken
                if ( BonusManager::getInstance()->isPresent( room->getNameOfFileWithDataAboutRoom(), itemData->label ) )
                {
                        freeItem = new FreeItem( itemData, fx, fy, fz,
                                                 item.direction() == rxml::direction::none ? NoDirection : Direction( item.direction() - 1 ) );

                        std::string behaviorOfItem = item.behavior ();
                        if ( behaviorOfItem.empty () )
                                behaviorOfItem = "still";

                        // extra data for behavior of item
                        if ( behaviorOfItem == "behavior of elevator" )
                        {
                                int* data = new int[ 3 ];
                                int foundData = 0;

                                for ( rxml::item::extra_const_iterator i = item.extra().begin (); i != item.extra().end (); ++i )
                                {
                                        data[ foundData++ ] = ( *i );
                                }

                                // three entries are needed
                                if ( foundData == 3 )
                                {
                                        freeItem->assignBehavior(
                                                behaviorOfItem,
                                                reinterpret_cast< void * >( data )
                                        );
                                        delete[] data;
                                }
                        }
                        else if ( behaviorOfItem == "behavior of waiting hunter in four directions" )
                        {
                                freeItem->assignBehavior(
                                        behaviorOfItem,
                                        reinterpret_cast< void * >( this->itemDataManager->findItemByLabel( "imperial-guard" ) )
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
                                        reinterpret_cast< void * >( this->itemDataManager->findItemByLabel( "bubbles" ) )
                                );
                        }
                        else
                        {
                                freeItem->assignBehavior( behaviorOfItem, 0 );
                        }
                }
        }

        return freeItem;
}

Door* RoomBuilder::buildDoor( const rxml::item& item )
{
        return
                new Door( this->itemDataManager, item.label(),
                                item.x(), item.y(), ( item.z() > Top ? item.z() * LayerHeight : Top ),
                                        Direction( item.direction() - 1 ) );
}

/* static */
int RoomBuilder::getXCenterOfRoom( ItemData* playerData, Room* theRoom )
{
        return
                ( ( theRoom->getBound( South ) - theRoom->getBound( North ) + playerData->widthX ) >> 1 )
                        + ( theRoom->getDoor( North ) != 0 ? theRoom->getSizeOfOneTile() >> 1 : 0 )
                                - ( theRoom->getDoor( South ) != 0 ? theRoom->getSizeOfOneTile() >> 1 : 0 ) ;

}

/* static */
int RoomBuilder::getYCenterOfRoom( ItemData* playerData, Room* theRoom )
{
        return
                ( ( theRoom->getBound( West ) - theRoom->getBound( East ) + playerData->widthY ) >> 1 )
                        + ( theRoom->getDoor( East ) != 0 ? theRoom->getSizeOfOneTile() >> 1 : 0 )
                                - ( theRoom->getDoor( West ) != 0 ? theRoom->getSizeOfOneTile() >> 1 : 0 )
                                        - 1 ;
}

}

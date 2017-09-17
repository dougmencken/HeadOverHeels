
#include "RoomBuilder.hpp"
#include "Exception.hpp"
#include "ItemDataManager.hpp"
#include "Room.hpp"
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

RoomBuilder::RoomBuilder( ItemDataManager* itemDataManager, const std::string& fileName )
{
        // Nombre del archivo
        this->fileName = fileName;

        // El gestor de datos
        this->itemDataManager = itemDataManager;
        // La sala en construcción
        this->room = 0;
}

RoomBuilder::RoomBuilder( ItemDataManager* itemDataManager )
{
        // El gestor de datos
        this->itemDataManager = itemDataManager;
        // La sala en construcción
        this->room = 0;
}

RoomBuilder::~RoomBuilder( )
{

}

Room* RoomBuilder::buildRoom ()
{
        // Procesa el archivo XML especificado para construir la sala
        try
        {
                std::auto_ptr< rxml::RoomXML > roomXML( rxml::room( fileName.c_str () ) );

                // Crea la sala con los parámetros básicos: identificador, dimensiones y existencia o no de suelo
                this->room = new Room( roomXML->name(), roomXML->scenery(), roomXML->xTiles(), roomXML->yTiles(), roomXML->width(), FloorId( int( roomXML->floorType() ) ) );
                if ( this->room == 0 )
                {
                        std::cerr << "I can't create room \"" << fileName << "\"" << std::endl ;
                        throw "I can't create room \"" + fileName + "\"";
                }

                // Para calcular las coordenadas de pantalla donde se situará el origen del espacio isométrico,
                // se necesita saber si la sala tiene puerta al norte y/o al este
                bool hasNorthDoor = false;
                bool hasEastDoor = false;

                // Recorre la lista de elementos buscando las puertas
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
                                          ( *i ).direction() == rxml::direction::eastsouth)
                                {
                                        hasEastDoor = true;
                                }
                        }
                }

                // Una vez que se sabe de la existencia o no de las puertas, se calculan las coordenadas
                room->calculateCoordinates( hasNorthDoor, hasEastDoor,
                                            roomXML->deltaX().present() ? roomXML->deltaX().get() : 0,
                                            roomXML->deltaY().present() ? roomXML->deltaY().get() : 0 );

                // Si la sala es triple se almacenan los datos constantes para situar inicialmente la cámara
                // y las coordenadas de la sala donde se realizará el desplazamiento de la cámara
                if ( roomXML->triple_room_data().present() )
                {
                        if ( roomXML->triple_room_data().get().northeast().present() )
                        {
                                rxml::northeast d = roomXML->triple_room_data().get().northeast().get();
                                room->addTripleRoomStartPoint( Northeast, d.x(), d.y() );
                        }
                        if ( roomXML->triple_room_data().get().southeast().present() )
                        {
                                rxml::southeast d = roomXML->triple_room_data().get().southeast().get();
                                room->addTripleRoomStartPoint( Southeast, d.x(), d.y() );
                        }
                        if ( roomXML->triple_room_data().get().northwest().present() )
                        {
                                rxml::northwest d = roomXML->triple_room_data().get().northwest().get();
                                room->addTripleRoomStartPoint( Northwest, d.x(), d.y() );
                        }
                        if ( roomXML->triple_room_data().get().southwest().present() )
                        {
                                rxml::southwest d = roomXML->triple_room_data().get().southwest().get();
                                room->addTripleRoomStartPoint( Southwest, d.x(), d.y() );
                        }
                        if ( roomXML->triple_room_data().get().eastnorth().present() )
                        {
                                rxml::eastnorth d = roomXML->triple_room_data().get().eastnorth().get();
                                room->addTripleRoomStartPoint( Eastnorth, d.x(), d.y() );
                        }
                        if ( roomXML->triple_room_data().get().eastsouth().present() )
                        {
                                rxml::eastsouth d = roomXML->triple_room_data().get().eastsouth().get();
                                room->addTripleRoomStartPoint( Eastsouth, d.x(), d.y() );
                        }
                        if ( roomXML->triple_room_data().get().westnorth().present() )
                        {
                                rxml::westnorth d = roomXML->triple_room_data().get().westnorth().get();
                                room->addTripleRoomStartPoint( Westnorth, d.x(), d.y() );
                        }
                        if ( roomXML->triple_room_data().get().westsouth().present() )
                        {
                                rxml::westsouth d = roomXML->triple_room_data().get().westsouth().get();
                                room->addTripleRoomStartPoint( Westsouth, d.x(), d.y() );
                        }

                        room->assignTripleRoomBounds( roomXML->triple_room_data().get().bound_x().minimum(),
                                                      roomXML->triple_room_data().get().bound_x().maximum(),
                                                      roomXML->triple_room_data().get().bound_y().minimum(),
                                                      roomXML->triple_room_data().get().bound_y().maximum() );
                }

                // Crea el suelo a base de losetas
                for ( rxml::floor::tile_const_iterator i = roomXML->floor().tile().begin (); i != roomXML->floor().tile().end (); ++i )
                {
                        FloorTile* floorTile = this->buildFloorTile( *i, isomot::GameManager::getInstance()->getChosenGraphicSet() );
                        room->addFloor( floorTile );
                }

                // Crea las paredes sin puertas
                if ( roomXML->walls().present() )
                {
                        for ( rxml::walls::wall_const_iterator i = roomXML->walls().get().wall().begin (); i != roomXML->walls().get().wall().end (); ++i )
                        {
                                Wall* wall = this->buildWall( *i, isomot::GameManager::getInstance()->getChosenGraphicSet() );
                                room->addWall( wall );
                        }
                }

                // Sitúa los elementos en la sala. Un elemento puede ser: un muro, una puerta, uno rejilla o uno libre
                for ( rxml::items::item_const_iterator i = roomXML->items().item().begin (); i != roomXML->items().item().end (); ++i )
                {
                        // Es una pared. Las paredes formadas por elementos son aquellas que tienen puertas
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
                                room->addItem( gridItem );
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
                                        // there are bonus elements that may already be taken and thus absent
                                }
                                else
                                {
                                        room->addItem( freeItem );
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

PlayerItem* RoomBuilder::buildPlayerInTheSameRoom( const std::string& player, const std::string& behavior, int x, int y, int z, const Direction& direction )
{
        return buildPlayerInRoom( this->room, player, behavior, x, y, z, direction );
}

PlayerItem* RoomBuilder::buildPlayerInRoom( Room* room, const std::string& player, const std::string& behavior, int x, int y, int z, const Direction& direction, bool withItem )
{
        PlayerItem* playerItem = 0;
        GameManager* gameManager = GameManager::getInstance();

        std::string newPlayer( player );
        std::string newBehaviorOfPlayer( behavior );

        // when composite player ran out of lives, check if any of simple players still survive
        if ( gameManager->getLives( player ) == 0 )
        {
                if ( player == "headoverheels" )
                {
                        // jugador superviviente
                        if ( gameManager->getLives( "head" ) > 0 )
                        {
                                newPlayer = "head";
                                newBehaviorOfPlayer = "behavior of Head";
                        }
                        else if ( gameManager->getLives( "heels" ) > 0 )
                        {
                                newPlayer = "heels";
                                newBehaviorOfPlayer = "behavior of Heels";
                        }
                        else
                        {
                                newPlayer = "in~buildroom";
                        }
                }
                // it is possible that two players join in room and have no lives
                else
                {
                        if ( gameManager->getLives( "head" ) == 0 && gameManager->getLives( "heels" ) == 0 )
                        {
                                newPlayer = "no~lives";
                        }
                }
        }

        ItemData* itemData = this->itemDataManager->findItemByLabel( newPlayer );

        // if it is found and has some lives left, place it in room
        if ( ( newPlayer == "headoverheels" || newPlayer == "head" || newPlayer == "heels" ) && itemData != 0 )
        {
                if ( gameManager->getLives( newPlayer ) > 0 )
                {
                        playerItem = new PlayerItem( itemData, x, y, z, direction );

                        // can’t move taken item to other room
                        if ( withItem )
                        {
                                gameManager->setItemTaken( 0 );
                        }

                        playerItem->setLives( gameManager->getLives( newPlayer ) );
                        playerItem->setTools( gameManager->playerTools( newPlayer ) );
                        playerItem->setAmmo( gameManager->getDonuts( newPlayer ) );
                        playerItem->setHighJumps( gameManager->getHighJumps() );
                        playerItem->setHighSpeed( gameManager->getHighSpeed() );
                        playerItem->setShieldTime( gameManager->getShield( newPlayer ) );
                        playerItem->assignBehavior( newBehaviorOfPlayer, reinterpret_cast< void * >( itemDataManager ) );

                        room->addPlayer( playerItem );
                }
        }

        return playerItem;
}

FloorTile* RoomBuilder::buildFloorTile( const rxml::tile& tile, const char* gfxPrefix )
{
        FloorTile* floorTile = 0;

        try {
                BITMAP* picture = load_png( ( isomot::sharePath() + gfxPrefix + "/" + tile.bitmap() ).c_str(), 0 );
                if ( picture == 0 ) {
                        std::cerr << "Picture \"" << tile.bitmap() << "\" at \"" << gfxPrefix << "\" is absent" << std::endl ;
                        throw "Picture " + tile.bitmap() + " at " + gfxPrefix + " is absent";
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
                        std::cerr << "Picture \"" << wall.bitmap() << "\" at \"" << gfxPrefix << "\" is absent" << std::endl ;
                        throw "Picture " + wall.bitmap() + " at " + gfxPrefix + " is absent";
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

                if ( item.behavior() == "behavior of disappearance in time" || item.behavior() == "behavior of disappearance on touch" ||
                     item.behavior() == "behavior of disappearance on jump into" || item.behavior() == "behavior of slow disappearance on jump into" ||
                     item.behavior() == "behavior of disappearance as soon as Head appears" )
                {
                        gridItem->assignBehavior( item.behavior(), reinterpret_cast< void * >( this->itemDataManager->findItemByLabel( "bubbles" ) ) );
                }
                else
                {
                        gridItem->assignBehavior( item.behavior(), 0 );
                }
        }

        return gridItem;
}

FreeItem* RoomBuilder::buildFreeItem( const rxml::item& item )
{
        FreeItem* freeItem = 0;

        // Se buscan los datos del elemento
        ItemData* itemData = this->itemDataManager->findItemByLabel( item.label () );

        // Si se han encontrado, se coloca el elemento en la sala
        if ( itemData != 0 )
        {
                // Coordenadas libres
                int fx = item.x() * room->getSizeOfOneTile() + ( ( room->getSizeOfOneTile() - itemData->widthX ) >> 1 );
                int fy = ( item.y() + 1 ) * room->getSizeOfOneTile() - ( ( room->getSizeOfOneTile() - itemData->widthY ) >> 1 ) - 1;
                int fz = item.z() != Top ? item.z() * LayerHeight : Top;

                // La única excepción para colocar un elemento es que sea un bonus y ya se haya cogido
                if ( BonusManager::getInstance()->isPresent( room->getIdentifier(), itemData->label ) )
                {
                        // Hay diferencia entre los enumerados de dirección manejados por el archivo y por las salas
                        freeItem = new FreeItem( itemData, fx, fy, fz,
                                                 item.direction() == rxml::direction::none ? NoDirection : Direction( item.direction() - 1 ) );

                        // extra data for behavior of item
                        if ( item.behavior () == "behavior of elevator" )
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
                                                item.behavior (),
                                                reinterpret_cast< void * >( data )
                                        );
                                        delete[] data;
                                }
                        }
                        else if ( item.behavior () == "behavior of waiting hunter in four directions" )
                        {
                                freeItem->assignBehavior(
                                        item.behavior (),
                                        reinterpret_cast< void * >( this->itemDataManager->findItemByLabel( "imperial-guard" ) )
                                );
                        }
                        else if ( item.behavior () == "behavior of something special" ||
                                        item.behavior () == "behavior of disappearance in time" ||
                                        item.behavior () == "behavior of disappearance on touch" ||
                                        item.behavior () == "behavior of disappearance on jump into" ||
                                        item.behavior () == "behaivor of final ball" )
                        {
                                freeItem->assignBehavior(
                                        item.behavior (),
                                        reinterpret_cast< void * >( this->itemDataManager->findItemByLabel( "bubbles" ) )
                                );
                        }
                        else
                        {
                                freeItem->assignBehavior( item.behavior (), 0 );
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

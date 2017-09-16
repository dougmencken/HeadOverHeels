
#include "Room.hpp"
#include "Exception.hpp"
#include "FloorTile.hpp"
#include "Wall.hpp"
#include "Item.hpp"
#include "GridItem.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
#include "Door.hpp"
#include "Mediator.hpp"
#include "Camera.hpp"
#include "GameManager.hpp"


namespace isomot
{

Room::Room( const std::string& identifier, const std::string& scenery, int xTiles, int yTiles, int tileSize, const FloorId& floorType )
: Mediated( )
        , identifier( identifier )
        , scenery( scenery )
        , tileSize( tileSize )
        , floorType( floorType )
        , exit( NoExit )
        , restart( false )
        , lastGridId( FirstGridId )
        , lastFreeId( FirstFreeId )
{
        this->tilesNumber.first = xTiles;
        this->tilesNumber.second = yTiles;
        this->mediator = new Mediator( this );
        this->camera = new Camera( this );

        memset( this->bounds, 0xffff, 12 * sizeof( unsigned short ) );
        memset( this->doors, 0, 12 * sizeof( Door * ) );

        // create empty floor
        for ( int i = 0; i < xTiles * yTiles + 1; i++ )
        {
                this->floor.push_back( 0 );
        }

        // Creación del vector de índices. Asigna el contenido de los elementos de la siguiente forma
        // para, por ejemplo, una rejilla de 8 x 8, donde la longitud de "drawIndex" es 64
        //
        // Elemento del vector
        //                00
        //              02  01
        //            05  04  03
        //          09  08  07  06
        //        14  13  12  11  10
        //      20  19  18  17  16  15
        //    27  26  25  24  23  22  21
        //  35  34  33  32  31  30  29  28
        //    42  41  40  39  38  37  36
        //      48  47  46  45  44  43
        //        53  52  51  50  49
        //          57  56  55  54
        //            60  59  58
        //              62  61
        //                63
        //
        // Contenido del elemento
        //                00
        //              01  08
        //            02  09  16
        //          03  10  17  24
        //        04  11  18  25  32
        //      05  12  19  26  33  40
        //    06  13  20  27  34  41  48
        //  07  14  21  28  35  42  49  56
        //    15  22  29  36  43  50  57
        //      23  30  37  44  51  58
        //        31  38  45  52  59
        //          39  46  53  60
        //            47  54  61
        //              55  62
        //                63

        int i( 0 );
        this->drawIndex = new int[ xTiles * yTiles ];

        for ( int f = 0; f <= ( xTiles + yTiles - 1 ); f++ )
        {
                for ( int n = yTiles - 1; n >= 0; n-- )
                {
                        int temp = n * ( xTiles - 1 ) + f;
                        if ( ( temp >= n * xTiles ) && ( temp < ( n + 1 ) * xTiles ) )
                        {
                                this->drawIndex[ i++ ] = temp;
                        }
                }
        }

        // Crea la imagen with suitable dimensions to draw the active room
        // single room ( id est up to 10 x 10 tiles ) just fits to the screen
        // but the image for double or triple room is larger
        if ( xTiles <= 10 && yTiles <= 10 )
        {
                picture = create_bitmap_ex( 32, ScreenWidth, ScreenHeight );
        }
        else if ( xTiles > 10 && yTiles > 10 )
        {
                picture = create_bitmap_ex( 32, ScreenWidth + 20 * ( tileSize << 1 ), ScreenHeight + 20 * tileSize );
        }
        else if ( xTiles > 10 || yTiles > 10 )
        {
                int w = ScreenWidth + ( xTiles > 10 ? ( ( xTiles - 10 ) * ( tileSize << 1 ) ) : 0 )
                                        + ( yTiles > 10 ? ( ( yTiles - 10 ) * ( tileSize << 1 ) ) : 0 );
                int h = ScreenHeight + ( xTiles > 10 ? ( ( xTiles - 10 ) * tileSize) : 0 )
                                        + ( yTiles > 10 ? ( ( yTiles - 10 ) * tileSize) : 0 );
                picture = create_bitmap_ex( 32, w, h );
        }

        // 128 for 50% opacity of shading or 256 for no shadows
        this->shadingScale = isomot::GameManager::getInstance()->getDrawShadows () ? 128 : 256 ;

        // Ahora la sala ya está activa
        this->active = true;
}

Room::~Room()
{
        // Destruye las puertas
        for ( int i = 0; i < 12; i++ )
        {
                if ( doors[ i ] != 0 )
                {
                        delete doors[ i ];
                }
        }

        // Destruye el suelo
        std::for_each( floor.begin (), floor.end (), DeleteObject() );

        // Destruye las paredes
        std::for_each( wallX.begin (), wallX.end (), DeleteObject() );
        std::for_each( wallY.begin (), wallY.end (), DeleteObject() );

        // Destruye el orden de dibujo de los elementos
        delete[ ] drawIndex;

        // Destruye la cámara
        delete camera;

        // Destruye el mediador
        delete mediator;

        // Destruye la imagen destino
        destroy_bitmap( picture );
}

void Room::addFloor( FloorTile * floorTile )
{
        // Se añade una nueva loseta
        floorTile->setMediator( mediator );
        floorTile->calculateOffset();
        this->floor[ floorTile->getColumn() ] = floorTile;
}

void Room::addWall( Wall * wall )
{
        // Se añade un segmento de muro sin puerta
        wall->setMediator( mediator );
        wall->calculateOffset();

        switch( wall->getAxis() )
        {
                case AxisX:
                        this->wallX.push_back( wall );
                        break;

                case AxisY:
                        this->wallY.push_back( wall );
                        break;
        }
}

void Room::addDoor( Door * door )
{
        door->setMediator( mediator );
        this->doors[ door->getDirection() ] = door;

        // each door is actually three free items
        this->addItem( door->getLeftJamb() );
        this->addItem( door->getRightJamb() );
        this->addItem( door->getLintel() );
}


#define NULL_GRIDITEM           "Cannot add grid item which is nil"
#define NO_ISOMOT_GRIDITEM      "Cannot add grid item because Isomot isn't active"
#define OUT_OF_LIMITS_GRIDITEM  "Coordinates for grid item are out of limits"
#define OUT_OF_ROOM_GRIDITEM    "Coordinates for grid item are out of room"
#define ZERO_HEIGHT_GRIDITEM    "Height for grid item is zero or negative"
#define COLLISION_GRIDITEM      "Collision with grid item"

void Room::addItem( GridItem * gridItem )
{
        try
        {
                if ( ! gridItem )
                        throw Exception( NULL_GRIDITEM );

                // La sala debe estar activa
                if ( ! this->active )
                        throw Exception( NO_ISOMOT_GRIDITEM );

                // Las coordenadas deben estar dentro de los límites
                if ( ( gridItem->getCellX() < 0 || gridItem->getCellY() < 0 ) ||
                        ( gridItem->getCellX() >= this->tilesNumber.first || gridItem->getCellY() >= this->tilesNumber.second ) )
                {
                        throw Exception( OUT_OF_LIMITS_GRIDITEM );
                }

                // El elemento debe tener altura
                if ( gridItem->getHeight() < 1 )
                {
                        throw Exception( ZERO_HEIGHT_GRIDITEM );
                }

                // Vacía la pila de colisiones
                mediator->clearStackOfCollisions ();

                // Asigna el identificador de Isomot
                gridItem->setId( this->lastGridId );

                // Si el elemento se va a colocar a una altura específica se buscan colisiones
                if ( gridItem->getZ() != Top )
                {
                        mediator->findCollisionWithItem( gridItem );
                }
                // Si el elemento se va en los más alto de la columna se busca el valor de Z
                else
                {
                        gridItem->setZ( mediator->findHighestZ( gridItem ) );
                }

                // Si se encontraron colisiones el elemento no se puede añadir
                if ( ! mediator->isStackOfCollisionsEmpty () )
                {
                        throw Exception( COLLISION_GRIDITEM );
                }

                // Cálculo del desplazamiento de la imagen respecto del píxel que marca el punto ( 0, 0, 0 ) de la sala
                if ( gridItem->getImage() )
                {
                        std::pair< int, int > offset (
                                ( ( this->tileSize * ( gridItem->getCellX() - gridItem->getCellY() ) ) << 1 ) - ( gridItem->getImage()->w >> 1 ) + 1,
                                this->tileSize * ( gridItem->getCellX() + gridItem->getCellY() + 2 ) - gridItem->getImage()->h - gridItem->getZ() - 1
                        ) ;
                        gridItem->setOffset( offset );
                }

                // Añade el elemento a la sala
                gridItem->setMediator( mediator );
                gridItem->setColumn( this->tilesNumber.first * gridItem->getCellY() + gridItem->getCellX() );
                mediator->insertItem( gridItem );

                // Pone a sombrear los elementos afectados por la inserción
                if ( this->shadingScale < 256 && gridItem->getShadow() )
                {
                        mediator->markItemsForShady( gridItem );
                }

                // Marca para enmascarar los elementos libres afectados por la inserción
                mediator->markItemsForMasking( gridItem );

                // Nuevo valor del identificador para elementos rejilla
                this->lastGridId += 2;
        }
        catch ( const Exception& e )
        {
                std::cout << e.what () << std::endl ;
        }
}


#define NULL_FREEITEM           "Cannot add free item which is nil"
#define NO_ISOMOT_FREEITEM      "Cannot add free item because Isomot isn't active"
#define OUT_OF_LIMITS_FREEITEM  "Coordinates for free item are out of limits"
#define OUT_OF_ROOM_FREEITEM    "Coordinates for free item are out of room"
#define ZERO_WIDTH_FREEITEM     "One or more of dimensions for free item are zero or negative"
#define COLLISION_FREEITEM      "Collision with free item"

void Room::addItem( FreeItem * freeItem )
{
        try
        {
                if ( ! freeItem )
                        throw Exception( NULL_FREEITEM );

                // La sala debe estar activa
                if ( ! this->active )
                        throw Exception( NO_ISOMOT_FREEITEM );

                // Las coordenadas deben estar dentro de los límites
                if ( freeItem->getX() < 0 || freeItem->getY() < 1 || freeItem->getZ() < Top )
                {
                        throw Exception( OUT_OF_LIMITS_FREEITEM );
                }

                // El elemento debe tener volumen
                if ( freeItem->getHeight() < 1 || freeItem->getWidthX() < 1 || freeItem->getWidthY() < 1 )
                {
                        throw Exception( ZERO_WIDTH_FREEITEM );
                }

                // El elemento no puede estar fuera de la sala
                if ( ( freeItem->getX() + freeItem->getWidthX() > this->tilesNumber.first * this->tileSize )
                        || ( freeItem->getY() - freeItem->getWidthY() < -1 )
                        || ( freeItem->getY() > this->tilesNumber.second * this->tileSize - 1 ) )
                {
                        throw Exception( OUT_OF_ROOM_FREEITEM );
                }

                // Vacía la pila de colisiones
                mediator->clearStackOfCollisions ();

                // Asigna el identificador de Isomot
                freeItem->setId( this->lastFreeId );

                // Si el elemento se va a colocar a una altura específica se buscan colisiones
                if ( freeItem->getZ() > Top )
                {
                        mediator->findCollisionWithItem( freeItem );
                }
                // Si el elemento se va en los más alto de la columna se busca el valor de Z
                else
                {
                        freeItem->setZ( mediator->findHighestZ( freeItem ) );
                }

                // Si se encontraron colisiones el elemento no se puede añadir
                if ( ! mediator->isStackOfCollisionsEmpty () )
                {
                        throw Exception( COLLISION_FREEITEM );
                }

                // Cálculo del desplazamiento de la imagen respecto del píxel que marca el punto ( 0, 0, 0 ) de la sala
                if ( freeItem->getImage () )
                {
                        std::pair<int, int> offset (
                                ( ( freeItem->getX() - freeItem->getY() ) << 1 ) + freeItem->getWidthX() + freeItem->getWidthY() - ( freeItem->getImage()->w >> 1 ) - 1,
                                freeItem->getX() + freeItem->getY() + freeItem->getWidthX() - freeItem->getImage()->h - freeItem->getZ()
                        ) ;
                        freeItem->setOffset( offset );
                }

                // Añade el elemento a la sala
                mediator->addTransparency( 0 );
                freeItem->setMediator( mediator );
                mediator->insertItem( freeItem );

                // Pone a sombrear los elementos afectados por la inserción
                if ( this->shadingScale < 256 && freeItem->getShadow() )
                {
                        mediator->markItemsForShady( freeItem );
                }

                // Marca para enmascarar los elementos libres afectados por la inserción
                mediator->markItemsForMasking( freeItem );

                // Nuevo valor del identificador para elementos rejilla
                this->lastFreeId += 2;
        }
        catch ( const Exception& e )
        {
                if ( freeItem ) {
                        std::cout << e.what ()
                                  << std::endl
                                  << "   free item at " << freeItem->getX() << " " << freeItem->getY() << " " << freeItem->getZ()
                                  << " has dimensions " << freeItem->getWidthX() << " x " << freeItem->getWidthY() << " x " << freeItem->getHeight()
                                  << std::endl
                                  << "   inside room of " << this->tilesNumber.first << " x " << this->tilesNumber.second << " tiles"
                                  << " each tile of " << this->tileSize << " pixels"
                                  << std::endl ;
                } else
                        std::cout << e.what () << std::endl ;
        }
}


#define NULL_PLAYER             "Cannot add player which is nil"
#define NO_ISOMOT_PLAYER        "Cannot add player because Isomot isn't active"
#define OUT_OF_LIMITS_PLAYER    "Coordinates for player are out of limits"
#define OUT_OF_ROOM_PLAYER      "Coordinates for player are out of room"
#define ZERO_WIDTH_PLAYER       "One or more of dimensions for player are zero or negative"
#define COLLISION_PLAYER        "Collision with player"

void Room::addPlayer( PlayerItem* playerItem )
{
        try
        {
                if ( ! playerItem )
                        throw Exception( NULL_PLAYER );

                // La sala debe estar activa
                if ( ! this->active )
                        throw Exception( NO_ISOMOT_PLAYER );

                // Las coordenadas deben estar dentro de los límites
                if ( playerItem->getX() < 0 || playerItem->getY() < 1 || playerItem->getZ() < Top )
                {
                        throw Exception( OUT_OF_LIMITS_PLAYER );
                }

                // El elemento debe tener volumen
                if ( playerItem->getHeight() < 1 || playerItem->getWidthX() < 1 || playerItem->getWidthY() < 1 )
                {
                        throw Exception( ZERO_WIDTH_PLAYER );
                }

                // El elemento no puede estar fuera de la sala
                if ( ( playerItem->getX() + playerItem->getWidthX() > this->tilesNumber.first * this->tileSize )
                        || ( playerItem->getY() - playerItem->getWidthY() < -1 )
                        || ( playerItem->getY() > this->tilesNumber.second * this->tileSize - 1 ) )
                {
                        throw Exception( OUT_OF_ROOM_PLAYER );
                }

                // Vacía la pila de colisiones
                mediator->clearStackOfCollisions ();

                // Asigna el identificador de Isomot
                playerItem->setId( this->lastFreeId );

                // Si el elemento se va a colocar a una altura específica se buscan colisiones
                if ( playerItem->getZ() > Top )
                {
                        mediator->findCollisionWithItem( playerItem );
                        while ( ! mediator->isStackOfCollisionsEmpty () )
                        {
                                playerItem->setZ( playerItem->getZ() + LayerHeight );
                                mediator->clearStackOfCollisions ();
                                mediator->findCollisionWithItem( playerItem );
                        }
                }
                // Si el elemento se va en los más alto de la columna se busca el valor de Z
                else
                {
                        playerItem->setZ( mediator->findHighestZ( playerItem ) );
                }

                // Si se encontraron colisiones el elemento no se puede añadir
                if ( ! mediator->isStackOfCollisionsEmpty () )
                {
                        throw Exception( COLLISION_PLAYER );
                }

                // Cálculo del desplazamiento de la imagen respecto del píxel que marca el punto (0,0,0) de la sala
                if ( playerItem->getImage () )
                {
                        std::pair<int, int> offset (
                                ( ( playerItem->getX() - playerItem->getY() ) << 1 ) + playerItem->getWidthX() + playerItem->getWidthY() - ( playerItem->getImage()->w >> 1 ) - 1,
                                playerItem->getX() + playerItem->getY() + playerItem->getWidthX() - playerItem->getImage()->h - playerItem->getZ()
                        ) ;
                        playerItem->setOffset( offset );
                }

                // Añade el elemento a la sala
                mediator->addTransparency( 0 );
                playerItem->setMediator( mediator );
                mediator->insertItem( playerItem );

                // Pone a sombrear los elementos afectados por la inserción
                if ( this->shadingScale < 256 && playerItem->getShadow() )
                {
                        mediator->markItemsForShady( playerItem );
                }

                // Marca para enmascarar los elementos libres afectados por la inserción
                mediator->markItemsForMasking( playerItem );

                // Nuevo valor del identificador para elementos rejilla
                this->lastFreeId += 2;
        }
        catch ( const Exception& e )
        {
                if ( playerItem ) {
                        std::cout << e.what ()
                                  << std::endl
                                  << "   player at " << playerItem->getX() << " " << playerItem->getY() << " " << playerItem->getZ()
                                  << " has dimensions " << playerItem->getWidthX() << " x " << playerItem->getWidthY() << " x " << playerItem->getHeight()
                                  << std::endl
                                  << "   inside room of " << this->tilesNumber.first << " x " << this->tilesNumber.second << " tiles"
                                  << " each tile of " << this->tileSize << " pixels"
                                  << std::endl ;
                } else
                        std::cout << e.what () << std::endl ;
        }
}

void Room::removeFloor( FloorTile * floorTile )
{
        // La eliminación es en realidad una sustitución por una loseta vacía
        this->floor[ floorTile->getColumn () ] = 0;
        delete floorTile;
}

void Room::removeItem( GridItem * gridItem )
{
        try
        {
                // Elimina el elemento de la sala
                mediator->removeItem( gridItem );

                // Pone a sombrear los elementos afectados por la eliminación
                if ( this->shadingScale < 256 && gridItem->getShadow() )
                {
                        mediator->markItemsForShady( gridItem );
                }

                // Marca para enmascarar los elementos libres afectados por la eliminación
                mediator->markItemsForMasking( gridItem );

                // Destrucción del elemento
                delete gridItem;
        }
        catch ( const Exception& e )
        {
                std::cout << e.what () << std::endl ;
        }
}

void Room::removeItem( FreeItem * freeItem )
{
        try
        {
                // Se elimina de la tabla de transparencias
                mediator->removeTransparency( freeItem->getTransparency() );

                // Elimina el elemento de la sala
                mediator->removeItem( freeItem );

                // Pone a sombrear los elementos afectados por la eliminación
                if( this->shadingScale < 256 && freeItem->getShadow() )
                {
                        mediator->markItemsForShady( freeItem );
                }

                // Marca para enmascarar los elementos libres afectados por la eliminación
                mediator->markItemsForMasking( freeItem );

                // Destrucción del elemento
                delete freeItem;
        }
        catch ( const Exception& e )
        {
                std::cout << e.what () << std::endl ;
        }
}

void Room::removePlayer( PlayerItem* playerItem )
{
        try
        {
                // Se elimina de la tabla de transparencias
                mediator->removeTransparency( playerItem->getTransparency() );

                // Elimina el elemento de la sala
                mediator->removeItem( playerItem );

                // Pone a sombrear los elementos afectados por la eliminación
                if ( this->shadingScale < 256 && playerItem->getShadow() )
                {
                        mediator->markItemsForShady( playerItem );
                }

                // Marca para enmascarar los elementos libres afectados por la eliminación
                mediator->markItemsForMasking( playerItem );

                // Destrucción del elemento
                delete playerItem;
        }
        catch ( const Exception& e )
        {
                std::cout << e.what () << std::endl ;
        }
}

void Room::draw( BITMAP* where )
{
        // draw room when it is active one
        if ( active )
        {
                // draw in picture of room itself when destination bitmap isn’t given
                if ( where == 0 )
                        where = this->picture ;

                // clean the image where to draw this room
                clear_bitmap( where );

                // acquire video memory before drawing
                if ( is_video_bitmap( where ) )
                {
                        acquire_bitmap( where );
                }

                // adjust position of camera
                if ( tilesNumber.first > 10 || tilesNumber.second > 10 )
                {
                        camera->centerOn( mediator->getActivePlayer () );
                }

                // draw tiles of room
                for ( int xCell = 0; xCell < this->tilesNumber.first; xCell++ )
                {
                        for ( int yCell = 0; yCell < this->tilesNumber.second; yCell++ )
                        {
                                int column = this->tilesNumber.first * yCell + xCell;

                                if ( floor[ column ] != 0 )  // if there is tile of floor here
                                {
                                        // shade this tile when shadows are on
                                        if ( shadingScale < 256 && floor[ column ]->getImage() )
                                        {
                                                mediator->lockGridItemMutex();
                                                mediator->lockFreeItemMutex();

                                                floor[ column ]->requestCastShadow();

                                                mediator->unlockGridItemMutex();
                                                mediator->unlockFreeItemMutex();
                                        }

                                        // draw this tile o’floor
                                        floor[ column ]->draw( where );
                                }
                        }
                }

                // draw walls without doors
                for ( std::vector< Wall * >::iterator wx = this->wallX.begin (); wx != this->wallX.end (); ++wx )
                {
                        ( *wx )->draw( where );
                }
                for ( std::vector< Wall * >::iterator wy = this->wallY.begin (); wy != this->wallY.end (); ++wy )
                {
                        ( *wy )->draw( where );
                }

                mediator->lockGridItemMutex();
                mediator->lockFreeItemMutex();

                // draw grid items
                for ( int i = 0; i < this->tilesNumber.first * this->tilesNumber.second; i++ )
                {
                        for ( std::list< GridItem * >::iterator g = mediator->structure[ drawIndex[ i ] ].begin (); g != mediator->structure[ drawIndex[ i ] ].end (); ++g )
                        {
                                GridItem* gridItem = static_cast< GridItem * >( *g );

                                if ( shadingScale < 256 && gridItem->getImage() )
                                {
                                        gridItem->requestCastShadow( drawIndex[ i ] );
                                }

                                gridItem->draw( where );
                        }
                }

                // for free items there’re two steps of drawing

                // at first shade them with both of grid items and other free items
                for ( std::list< FreeItem * >::iterator f = mediator->freeItems.begin (); f != mediator->freeItems.end (); ++f )
                {
                        FreeItem* freeItem = static_cast< FreeItem * >( *f );

                        if ( freeItem->getImage() )
                        {
                                // shade an item when shadows are on
                                if ( shadingScale < 256 )
                                        freeItem->requestCastShadow();
                        }
                }

                // then mask them and finally draw them
                for ( std::list< FreeItem * >::iterator f = mediator->freeItems.begin (); f != mediator->freeItems.end (); ++f )
                {
                        FreeItem* freeItem = static_cast< FreeItem * >( *f );

                        if ( freeItem->getImage() )
                        {
                                freeItem->requestMask();
                                freeItem->draw( where );
                        }
                }

                mediator->unlockGridItemMutex();
                mediator->unlockFreeItemMutex();

                // free used video memory after drawing
                if ( is_video_bitmap( where ) )
                {
                        release_bitmap( where );
                }
        }
}

void Room::startUpdate()
{
        mediator->startUpdate();
}

void Room::stopUpdate()
{
        mediator->stopUpdate();
}

void Room::calculateBounds()
{
        bounds[ North ] = doors[ North ] || doors[ Northeast ] || doors[ Northwest ] || this->floorType == NoFloor ? tileSize : 0;
        bounds[ East ] = doors[ East ] || doors[ Eastnorth ] || doors[ Eastsouth ]  || this->floorType == NoFloor ? tileSize : 0;
        bounds[ South ] = tileSize * tilesNumber.first - ( doors[ South ] || doors[ Southeast ] || doors[ Southwest ]  ? tileSize : 0 );
        bounds[ West ] = tileSize * tilesNumber.second - ( doors[ West ] || doors[ Westnorth ] || doors[ Westsouth ]  ? tileSize : 0 );

        // door limits of triple room
        bounds[ Northeast ] = doors[ Northeast ] ? doors[ Northeast ]->getLintel()->getX() + doors[ Northeast ]->getLintel()->getWidthX() - tileSize : bounds[ North ];
        bounds[ Northwest ] = doors[ Northwest ] ? doors[ Northwest ]->getLintel()->getX() + doors[ Northwest ]->getLintel()->getWidthX() - tileSize : bounds[ North ];
        bounds[ Southeast ] = doors[ Southeast ] ? doors[ Southeast ]->getLintel()->getX() + tileSize : bounds[ South ];
        bounds[ Southwest ] = doors[ Southwest ] ? doors[ Southwest ]->getLintel()->getX() + tileSize : bounds[ South ];
        bounds[ Eastnorth ] = doors[ Eastnorth ] ? doors[ Eastnorth ]->getLintel()->getY() + doors[ Eastnorth ]->getLintel()->getWidthY() - tileSize : bounds[ East ];
        bounds[ Eastsouth ] = doors[ Eastsouth ] ? doors[ Eastsouth ]->getLintel()->getY() + doors[ Eastsouth ]->getLintel()->getWidthY() - tileSize : bounds[ East ];
        bounds[ Westnorth ] = doors[ Westnorth ] ? doors[ Westnorth ]->getLintel()->getY() + tileSize : bounds[ West ];
        bounds[ Westsouth ] = doors[ Westsouth ] ? doors[ Westsouth ]->getLintel()->getY() + tileSize : bounds[ West ];
}

void Room::calculateCoordinates( bool hasNorthDoor, bool hasEastDoor, int deltaX, int deltaY )
{
        // Para calcular las coordenadas no se tiene en cuenta las losetas ocupadas por las puertas y sus muros
        int xGrid = hasNorthDoor || this->floorType == NoFloor ? tilesNumber.first - 1 : tilesNumber.first;
        int yGrid = hasEastDoor || this->floorType == NoFloor ? tilesNumber.second - 1 : tilesNumber.second;

        // Si las variables son impares quiere decir que hay puertas al sur y/o al oeste
        // Se resta 1 en tal caso para obtener el número de losetas hábiles de la sala
        xGrid += ( xGrid & 1 ? -1 : 0 );
        yGrid += ( yGrid & 1 ? -1 : 0 );

        // Para una sala de 8x8 (sin contar las puertas) el punto origen es x = anchura_imagen/2 e y = altura_imagen/3
        // Para una sala de dimensiones menores se desplaza el punto origen de tal manera que quede centrado
        // en la rejilla de 8x8. Por ejemplo: para una sala de 8x6, se desplaza la coordenada Y una loseta a la
        // izquierda respecto del punto origen
        int middlePointX = ( xGrid > 8 || yGrid > 8 ? picture->w : ScreenWidth ) >> 1;
        this->coordinates.first = middlePointX
                                - ( hasNorthDoor || this->floorType == NoFloor ? ( tileSize << 1 ) : 0)
                                + ( hasEastDoor || this->floorType == NoFloor ? ( tileSize << 1 ) : 0 );
        this->coordinates.second = ( ScreenHeight / 3 - deltaY )
                                 - ( hasNorthDoor || this->floorType == NoFloor ? tileSize : 0 )
                                 - ( hasEastDoor || this->floorType == NoFloor ? tileSize : 0 );

        if ( xGrid <= 8 && yGrid <= 8 )
        {
                this->coordinates.first += ( yGrid - xGrid ) * tileSize;
                this->coordinates.second += ( ( 16 - xGrid - yGrid ) >> 1 ) * tileSize;
        }
        else
        {
                this->coordinates.first = ( picture->w - ( ( xGrid + yGrid ) * ( tileSize << 1 ) ) ) / 2
                                        + ( yGrid * ( tileSize << 1 ) )
                                        - ( hasNorthDoor ? ( tileSize << 1 ) : 0 )
                                        + ( hasEastDoor ? ( tileSize << 1 ) : 0 );
        }
}

void Room::activatePlayer( const std::string& player )
{
        Item* item = mediator->findItemByLabel( player );
        if ( item != 0 )
        {
                mediator->setActivePlayer( static_cast< PlayerItem * >( item ) );
        }
}

void Room::activate()
{
        this->startUpdate();
        this->active = true;
}

void Room::deactivate()
{
        this->stopUpdate();
        this->active = false;
}

bool Room::changePlayer( ItemDataManager* itemDataManager )
{
        return mediator->nextPlayer( itemDataManager );
}

bool Room::alivePlayer( ItemDataManager* itemDataManager )
{
        if ( mediator->alivePlayer( itemDataManager ) )
        {
                this->activate();
                return true;
        }

        return false;
}

bool Room::calculateEntryCoordinates( const Direction& entry, int widthX, int widthY, int northBound, int eastBound, int southBound, int westBound, int* x, int* y, int* z )
{
        bool result = false;
        int differentSizeDeltaX = 0;
        int differentSizeDeltaY = 0;

        if ( entry == Up || entry == Down || entry == ByTeleport || entry == ByTeleportToo )
        {
                int limit = 10 * tileSize;

                // Cálculo de la diferencia que hay que aplicar al eje X cuando se pasa de una sala
                // simple a una doble o viceversa
                if ( ( bounds[ South ] - bounds[ North ] > limit && southBound - northBound <= limit ) ||
                        ( bounds[ South ] - bounds[ North ] <= limit && southBound - northBound > limit ) )
                {
                        differentSizeDeltaX = ( bounds[ South ] - bounds[ North ] - southBound + northBound ) >> 1;
                }

                // Cálculo de la diferencia que hay que aplicar al eje Y cuando se pasa de una sala
                // simple a una doble o viceversa
                if ( ( bounds[ West ] - bounds[ East ] > limit && westBound - eastBound <= limit ) ||
                        ( bounds[ West ] - bounds[ East ] <= limit && westBound - eastBound > limit ) )
                {
                        differentSizeDeltaY = ( bounds[ West ] - bounds[ East ] - westBound + eastBound ) >> 1;
                }
        }

        // Cálculo de coordenadas en función de la vía de entrada
        switch ( entry )
        {
                case North:
                        *x = bounds[ North ] - tileSize + 1;
                        *y = doors[ North ]->getLeftJamb()->getY() - doors[ North ]->getLeftJamb()->getWidthY();
                        *z = doors[ North ]->getLeftJamb()->getZ();
                        result = true;
                        break;

                case South:
                        *x = bounds[ South ] + tileSize - widthX;
                        *y = doors[ South ]->getLeftJamb()->getY() - doors[ South ]->getLeftJamb()->getWidthY();
                        *z = doors[ South ]->getLeftJamb()->getZ();
                        result = true;
                        break;

                case East:
                        *x = doors[ East ]->getLeftJamb()->getX() + doors[ East ]->getLeftJamb()->getWidthX();
                        *y = bounds[ East ] - tileSize + widthY;
                        *z = doors[ East ]->getLeftJamb()->getZ();
                        result = true;
                        break;

                case West:
                        *x = doors[ West ]->getLeftJamb()->getX() + doors[ West ]->getLeftJamb()->getWidthX();
                        *y = bounds[ West ] + tileSize - 1;
                        *z = doors[ West ]->getLeftJamb()->getZ();
                        result = true;
                        break;

                case Northeast:
                        *x = bounds[ Northeast ];
                        *y = doors[ Northeast ]->getLeftJamb()->getY() - doors[ Northeast ]->getLeftJamb()->getWidthY();
                        *z = doors[ Northeast ]->getLeftJamb()->getZ();
                        result = true;
                        break;

                case Northwest:
                        *x = bounds[ Northwest ];
                        *y = doors[ Northwest ]->getLeftJamb()->getY() - doors[ Northwest ]->getLeftJamb()->getWidthY();
                        *z = doors[ Northwest ]->getLeftJamb()->getZ();
                        result = true;
                        break;

                case Southeast:
                        *x = bounds[ Southeast ] - widthX;
                        *y = doors[ Southeast ]->getLeftJamb()->getY() - doors[ Southeast ]->getLeftJamb()->getWidthY();
                        *z = doors[ Southeast ]->getLeftJamb()->getZ();
                        result = true;
                        break;

                case Southwest:
                        *x = bounds[ Southwest ] - widthX;
                        *y = doors[ Southwest ]->getLeftJamb()->getY() - doors[ Southwest ]->getLeftJamb()->getWidthY();
                        *z = doors[ Southwest ]->getLeftJamb()->getZ();
                        result = true;
                        break;

                case Eastnorth:
                        *x = doors[ Eastnorth ]->getLeftJamb()->getX() + doors[ Eastnorth ]->getLeftJamb()->getWidthX();
                        *y = bounds[ Eastnorth ] + widthY;
                        *z = doors[ Eastnorth ]->getLeftJamb()->getZ();
                        result = true;
                        break;

                case Eastsouth:
                        *x = doors[ Eastsouth ]->getLeftJamb()->getX() + doors[ Eastsouth ]->getLeftJamb()->getWidthX();
                        *y = bounds[ Eastsouth ] + widthY;
                        *z = doors[ Eastsouth ]->getLeftJamb()->getZ();
                        result = true;
                        break;

                case Westnorth:
                        *x = doors[ Westnorth ]->getLeftJamb()->getX() + doors[ Westnorth ]->getLeftJamb()->getWidthX();
                        *y = bounds[ Westnorth ] - widthY;
                        *z = doors[ Westnorth ]->getLeftJamb()->getZ();
                        result = true;
                        break;

                case Westsouth:
                        *x = doors[ Westsouth ]->getLeftJamb()->getX() + doors[ Westsouth ]->getLeftJamb()->getWidthX();
                        *y = bounds[ Westsouth ] - widthY;
                        *z = doors[ Westsouth ]->getLeftJamb()->getZ();
                        result = true;
                        break;

                case Up:
                        *x += bounds[ North ] - northBound + ( ( bounds[ South ] - bounds[ North ] - southBound + northBound ) >> 1 );
                        *x += ( *x < ( ( bounds[ South ] - bounds[ North ] ) >> 1 ) ? -differentSizeDeltaX : differentSizeDeltaX );
                        *y += bounds[ East ] - eastBound + ( ( bounds[West] - bounds[East] - westBound + eastBound ) >> 1 );
                        *y += ( *y < ( ( bounds[ West ] - bounds[ East ] ) >> 1 ) ? -differentSizeDeltaY : differentSizeDeltaY );
                        *z = MaxLayers * LayerHeight;
                        result = true;
                        break;

                case Down:
                        *x += bounds[ North ] - northBound + ( ( bounds[ South ] - bounds[ North ] - southBound + northBound ) >> 1 );
                        *x += ( *x < ( ( bounds[ South ] - bounds[ North ] ) >> 1 ) ? -differentSizeDeltaX : differentSizeDeltaX );
                        *y += bounds[ East ] - eastBound + ( ( bounds[ West ] - bounds[ East ] - westBound + eastBound) >> 1 );
                        *y += ( *y < ( ( bounds[ West ] - bounds[ East ]) >> 1 ) ? -differentSizeDeltaY : differentSizeDeltaY );
                        *z = LayerHeight;
                        result = true;
                        break;

                case ByTeleport:
                case ByTeleportToo:
                        *x += bounds[ North ] - northBound + ( ( bounds[ South ] - bounds[ North ] - southBound + northBound ) >> 1 );
                        *x += ( *x < ( ( bounds[ South ] - bounds[ North ] ) >> 1 ) ? -differentSizeDeltaX : differentSizeDeltaX );
                        *y += differentSizeDeltaY + bounds[ East ] - eastBound + ( ( bounds[ West ] - bounds[ East ] - westBound + eastBound ) >> 1 );
                        *y += ( *y < ( ( bounds[ West ] - bounds[ East ] ) >> 1 ) ? -differentSizeDeltaY : differentSizeDeltaY );
                        result = true;
                        break;

                default:
                        ;
        }

        return result;
}

void Room::addTripleRoomStartPoint( const Direction& direction, int x, int y )
{
        this->tripleRoomStartPoints.push_back( TripleRoomStartPoint( direction, x, y ) );
}

void Room::assignTripleRoomBounds( int minX, int maxX, int minY, int maxY )
{
        this->tripleRoomBoundX.first = minX;
        this->tripleRoomBoundX.second = maxX;
        this->tripleRoomBoundY.first = minY;
        this->tripleRoomBoundY.second = maxY;
}

TripleRoomStartPoint* Room::findTripleRoomStartPoint( const Direction& direction )
{
        std::list< TripleRoomStartPoint >::iterator i = std::find_if(
                                                                tripleRoomStartPoints.begin (),
                                                                tripleRoomStartPoints.end (),
                                                                std::bind2nd( EqualTripleRoomStartPoint(), direction ) );
        return ( i != tripleRoomStartPoints.end () ? ( &( *i ) ) : 0 );
}

Door* Room::getDoor( const Direction& direction ) const
{
        return doors[ direction ];
}

Camera* Room::getCamera() const
{
        return camera;
}

TripleRoomStartPoint::TripleRoomStartPoint( const Direction& direction, int x, int y )
: direction( direction ),
  x( x ),
  y( y )
{
}

bool EqualTripleRoomStartPoint::operator()( TripleRoomStartPoint point, Direction direction ) const
{
        return ( point.getDirection() == direction );
}

}

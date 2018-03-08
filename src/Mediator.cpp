
#include "Mediator.hpp"
#include "Mediated.hpp"
#include "RoomBuilder.hpp"
#include "Item.hpp"
#include "ItemData.hpp"
#include "GridItem.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
#include "FloorTile.hpp"
#include "ShadowCaster.hpp"
#include "Behavior.hpp"

#ifdef DEBUG
#  define DEBUG_SHADOWS         0
#  define DEBUG_MASKS           0
#endif


namespace isomot
{

Mediator::Mediator( Room* room )
        : room( room )
        , gridItemsSorting( false )
        , freeItemsSorting( false )
        , switchInRoomIsOn( false )
        , activePlayer( nilPointer )
{
        pthread_mutex_init( &gridItemsMutex, nilPointer );
        pthread_mutex_init( &freeItemsMutex, nilPointer );

        // items which may be freezed by doughnut or switch
        badBoys.push_back( "behavior of detector" );
        badBoys.push_back( "behavior of hunter in four directions" );
        badBoys.push_back( "behavior of waiting hunter in four directions" );
        badBoys.push_back( "behavior of waiting hunter in eight directions" );
        badBoys.push_back( "behavior of there and back" );
        badBoys.push_back( "behavior of move then turn left and move" );
        badBoys.push_back( "behavior of move then turn right and move" );
        badBoys.push_back( "behavior of random patroling in four primary directions" );
        badBoys.push_back( "behavior of random patroling in four secondary directions" );
        badBoys.push_back( "behavior of random patroling in four secondary directions" );
        badBoys.push_back( "behavior of random patroling in eight directions" );

        // structure of room
        for ( unsigned int i = 0; i < room->getTilesX() * room->getTilesY() + 1; i++ )
        {
                structure.push_back( Column() );
        }
}

Mediator::~Mediator()
{
        // bin grid items
        for ( size_t i = 0; i < structure.size(); i++ )
        {
                std::for_each( structure[ i ].begin(), structure[ i ].end (), DeleteObject() );
                structure[ i ].clear();
        }

        // bin free items
        for ( std::list< FreeItem* >::const_iterator it = freeItems.begin () ; it != freeItems.end () ; ++ it )
        {
                FreeItem* item = *it;

                // player items are deleted in destructor of Room
                if ( item->whichKindOfItem() != "player item" )
                {
                        delete item ;
                }
        }
        freeItems.clear() ;

        pthread_mutex_destroy( &gridItemsMutex );
        pthread_mutex_destroy( &freeItemsMutex );
}

void Mediator::update()
{
        pthread_mutex_lock( &gridItemsMutex );

        // Ordenación de las listas de elementos rejilla, si procede
        if ( this->gridItemsSorting )
        {
                for ( unsigned int i = 0; i < structure.size(); i++ )
                {
                        if ( ! structure[ i ].empty() )
                        {
                                structure[ i ].sort( sortGridItemList );
                        }
                }
                this->gridItemsSorting = false;
        }

        // Elementos libres que se eliminarán después de la actualización
        std::stack< GridItem * > gridItemsToDelete;

        // Actualiza los elementos rejilla
        for ( unsigned int i = 0; i < structure.size(); i++ )
        {
                for ( std::list< GridItem* >::iterator g = structure[ i ].begin (); g != structure[ i ].end (); ++g )
                {
                        if ( ( *g )->update() )
                                gridItemsToDelete.push( *g );
                }
        }

        // Se eliminan los elementos rejilla que pudieran haberse destruido
        while ( ! gridItemsToDelete.empty() )
        {
                GridItem* gi = gridItemsToDelete.top();
                room->removeGridItem( gi );
                gridItemsToDelete.pop();
        }

        pthread_mutex_unlock( &gridItemsMutex );

        pthread_mutex_lock( &freeItemsMutex );

        // Ordenación de la lista de elementos libres, si procede
        if ( this->freeItemsSorting )
        {
                freeItems.sort( sortFreeItemList );
                this->freeItemsSorting = false;

                // Después de una reordenación es necesario volver a enmascarar los elementos porque
                // los solapamientos habrán cambiado
                for ( std::list< FreeItem * >::iterator f = freeItems.begin (); f != freeItems.end (); ++f )
                {
                        ( *f )->setWhichMask( WantRemask );
                }
        }

        if ( activePlayer != nilPointer )
        {
                activePlayer->behave ();
        }

        // Elementos libres que se eliminarán después de la actualización
        std::stack < FreeItem * > freeItemsToDelete;

        // Actualiza los elementos libres
        for ( std::list< FreeItem * >::iterator f = freeItems.begin (); f != freeItems.end (); ++f )
        {
                if ( ( *f )->update() )
                        freeItemsToDelete.push( *f );
        }

        // Se eliminan los elementos libres que pudieran haberse destruido
        while ( ! freeItemsToDelete.empty() )
        {
                FreeItem* fi = freeItemsToDelete.top();
                room->removeFreeItem( fi );
                freeItemsToDelete.pop();
        }

        std::list< PlayerItem * > playersInRoom = room->getPlayersYetInRoom();
        for ( std::list< PlayerItem * >::iterator p = playersInRoom.begin (); p != playersInRoom.end (); ++p )
        {
                if ( ( *p )->getWayOfExit().toString() != "no exit" )
                {
                        // transfer way of exit from player to room
                        room->setWayOfExit( ( *p )->getWayOfExit() );
                        ( *p )->setWayOfExit( NoExit );
                        break;
                }
        }

        pthread_mutex_unlock( &freeItemsMutex );
}

void Mediator::beginUpdate()
{
        this->threadRunning = true;

        pthread_attr_t attr;

        pthread_attr_init( &attr );
        pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE );

        pthread_create( &thread, nilPointer, updateThread, reinterpret_cast< void * >( this ) );

        pthread_attr_destroy( &attr );
}

void Mediator::endUpdate()
{
        this->threadRunning = false;
        pthread_join( thread, nilPointer );
}

void* updateThread( void* thisClass )
{
        Mediator* mediator = reinterpret_cast< Mediator* >( thisClass );

        while( mediator->isThreadRunning() )
        {
                mediator->update();
                milliSleep( 10 );
        }

        pthread_exit( nilPointer );
}

void Mediator::remaskWithFreeItem( FreeItem* item )
{
        if ( ! item ) return;

        // go through list of free items to see which ones to remask
        for ( std::list< FreeItem* >::iterator f = freeItems.begin (); f != freeItems.end (); ++f )
        {
                FreeItem* thatFreeItem = *f;

                if ( thatFreeItem->getId() != item->getId() && thatFreeItem->getRawImage() )
                {
                        // mask item if there’s overlap between images
                        if ( ( thatFreeItem->getOffsetX() < item->getOffsetX() + item->getRawImage()->w )
                                && ( thatFreeItem->getOffsetX() + thatFreeItem->getRawImage()->w > item->getOffsetX() )
                                && ( thatFreeItem->getOffsetY() < item->getOffsetY() + item->getRawImage()->h )
                                && ( thatFreeItem->getOffsetY() + thatFreeItem->getRawImage()->h > item->getOffsetY() ) )
                        {
                                thatFreeItem->setWhichMask( WantRemask );
                        }
                }
        }
}

void Mediator::remaskWithGridItem( GridItem* gridItem )
{
        if ( ! gridItem ) return;

        // go through list of free items to see which ones to remask
        for ( std::list< FreeItem* >::iterator f = freeItems.begin (); f != freeItems.end (); ++f )
        {
                FreeItem* freeItem = *f;

                if ( freeItem != nilPointer && freeItem->getRawImage() )
                {
                        // mask item if there’s overlap between images
                        if ( ( freeItem->getOffsetX() < gridItem->getOffsetX() + gridItem->getRawImage()->w )
                                && ( freeItem->getOffsetX() + freeItem->getRawImage()->w > gridItem->getOffsetX() )
                                && ( freeItem->getOffsetY() < gridItem->getOffsetY() + gridItem->getRawImage()->h )
                                && ( freeItem->getOffsetY() + freeItem->getRawImage()->h > gridItem->getOffsetY() ) )
                        {
                                // see whether free item is behind grid item
                                if ( ( freeItem->getX() < gridItem->getX() + static_cast< int >( gridItem->getWidthX() ) )
                                        && ( freeItem->getY() - static_cast< int >( freeItem->getWidthY() ) < gridItem->getY() )
                                        && ( freeItem->getZ() < gridItem->getZ() + static_cast< int >( gridItem->getHeight() ) ) )
                                {
                                        freeItem->setWhichMask( WantRemask );
                                }
                        }
                }
        }
}

void Mediator::reshadeWithGridItem( GridItem* item )
{
        // shade free items underneath
        this->shadeFreeItemsBeneathItem( item );

        // shade grid items below
        int column = room->getTilesX() * item->getCellY() + item->getCellX();

        if ( ! this->structure[ column ].empty() )
        {
                std::list< GridItem * >::iterator g = this->structure[ column ].begin ();
                GridItem* gridItem = *g;

                while ( g != this->structure[ column ].end() && item->getId() != gridItem->getId() )
                {
                        gridItem->setWhichShade( WantReshadow );
                        // next grid item in column
                        gridItem = *( ++g ) ;
                }
        }

        // shade floor in this column, if any
        if ( room->floor[ column ] != nilPointer )
        {
                room->floor[ column ]->binShadyImage();
                room->floor[ column ]->setWantShadow( true );
        }
}

void Mediator::reshadeWithFreeItem( FreeItem* item )
{
        // shade free items underneath
        this->shadeFreeItemsBeneathItem( item );

        // range of columns met with item
        int xStart = item->getX() / room->tileSize;
        int xEnd = ( item->getX() + item->getWidthX() - 1 ) / room->tileSize + 1;
        int yStart = ( item->getY() - item->getWidthY() + 1 ) / room->tileSize;
        int yEnd = item->getY() / room->tileSize + 1;

        // for every column that item meets
        for ( int i = xStart; i < xEnd; i++ )
        {
                for ( int j = yStart; j < yEnd; j++ )
                {
                        // Todo elemento de la columna que se encuentre por debajo, se marca para sombrear
                        int column = room->getTilesX() * j + i;
                        for ( std::list< GridItem* >::iterator g = structure[ column ]. begin (); g != structure[ column ]. end (); ++g )
                        {
                                GridItem* gridItem = *g ;

                                if ( item->getZ () > gridItem->getZ () )
                                {
                                        gridItem->setWhichShade( WantReshadow );
                                }
                        }

                        // shade floor in this column, if any
                        if ( room->floor[ column ] != nilPointer )
                        {
                                room->floor[ column ]->binShadyImage();
                                room->floor[ column ]->setWantShadow( true );
                        }
                }
        }
}

void Mediator::shadeFreeItemsBeneathItem( Item* item )
{
        for ( std::list < FreeItem* >::iterator f = freeItems.begin (); f != freeItems.end (); ++f )
        {
                FreeItem* freeItem = *f ;

                if ( freeItem->getId () != item->getId () )
                {
                        if ( ( freeItem->getX() + static_cast< int >( freeItem->getWidthX() ) > item->getX() )
                                && ( freeItem->getX() < item->getX() + static_cast< int >( item->getWidthX() ) )
                                && ( freeItem->getY() > freeItem->getY() - static_cast< int >( item->getWidthY() ) )
                                && ( freeItem->getY() - static_cast< int >( freeItem->getWidthY() ) < item->getY() )
                                && ( freeItem->getZ() < item->getZ() ) )
                        {
                                freeItem->setWhichShade( WantReshadow );
                        }
                }
        }
}

void Mediator::castShadowOnFloor( FloorTile* floorTile )
{
        int xCell = floorTile->getCellX();
        int yCell = floorTile->getCellY();
        int column = floorTile->getColumn();
        int tileSize = room->getSizeOfOneTile();

        // shade with every grid item above
        for ( std::list< GridItem * >::iterator g = structure[ column ].begin (); g != structure[ column ].end (); ++g )
        {
                GridItem* gridItem = *g ;

                // shade image of tile when item has shadow
                if ( gridItem->getImageOfShadow() != nilPointer )
                {
                # if  defined( DEBUG_SHADOWS )  &&  DEBUG_SHADOWS
                        std::cout << "casting shadow from " << gridItem->whichKindOfItem() << " \"" << gridItem->getLabel() << "\"" <<
                                " at x=" << gridItem->getX() << " y=" << gridItem->getY() << " z=" << gridItem->getZ() <<
                                " on floor tile at" <<
                                " x=" << tileSize << "*" << xCell << "=" << xCell * tileSize <<
                                " y=" << tileSize << "*" << yCell << "=" << yCell * tileSize
                                  << std::endl ;
                # endif

                        ShadowCaster::castShadowOnFloor (
                                floorTile,
                                /* x */ ( tileSize << 1 ) * ( xCell - yCell ) - ( gridItem->getImageOfShadow()->w >> 1 ) + room->getX0() + 1,
                                /* y */ tileSize * ( xCell + yCell + 1 ) - ( gridItem->getImageOfShadow()->h >> 1 ) + room->getY0() - 1,
                                /* shadow */ gridItem->getImageOfShadow(),
                                /* shadingScale */ room->shadingScale
                                /* transparency = 0 */
                        ) ;
                }
        }

        // scroll through list of free items to shade floor tile
        for ( std::list< FreeItem* >::iterator f = freeItems.begin (); f != freeItems.end (); ++f )
        {
                FreeItem* freeItem = *f ;

                if ( freeItem->getImageOfShadow() != nilPointer )
                {
                        int xStart = freeItem->getX() / tileSize;
                        int xEnd = ( freeItem->getX() + freeItem->getWidthX() - 1 ) / tileSize;
                        int yStart = ( freeItem->getY() - freeItem->getWidthY() + 1 ) / tileSize;
                        int yEnd = freeItem->getY() / tileSize;

                        // shade with free item above
                        if ( xCell >= xStart && xCell <= xEnd && yCell >= yStart && yCell <= yEnd )
                        {
                        # if  defined( DEBUG_SHADOWS )  &&  DEBUG_SHADOWS
                                std::cout << "casting shadow from " << freeItem->whichKindOfItem() << " \"" << freeItem->getLabel() << "\"" <<
                                        " at x=" << freeItem->getX() << " y=" << freeItem->getY() << " z=" << freeItem->getZ() <<
                                        " on floor tile at" <<
                                        " x=" << tileSize << "*" << xCell << "=" << xCell * tileSize <<
                                        " y=" << tileSize << "*" << yCell << "=" << yCell * tileSize
                                          << std::endl ;
                        # endif

                                ShadowCaster::castShadowOnFloor (
                                        floorTile,
                                        /* x */ ( ( freeItem->getX() - freeItem->getY() ) << 1 ) + room->getX0() + ( freeItem->getWidthX() + freeItem->getWidthY() ) - ( ( freeItem->getImageOfShadow()->w ) >> 1 ) - 1,
                                        /* y */ freeItem->getX() + freeItem->getY() + room->getY0() + ( ( freeItem->getWidthX() - freeItem->getWidthY() + 1 ) >> 1 ) - ( ( freeItem->getImageOfShadow()->h ) >> 1 ),
                                        /* shadow */ freeItem->getImageOfShadow(),
                                        /* shadingScale */ room->shadingScale,
                                        /* transparency */ freeItem->getTransparency()
                                ) ;
                        }
                }
        }
}

void Mediator::castShadowOnGridItem( GridItem* gridItem )
{
        int tileSize = room->getSizeOfOneTile();
        int column = gridItem->getColumn();

        // shade with grid items it may have above
        for ( std::list< GridItem* >::iterator g = structure[ column ].begin(); g != structure[ column ].end(); ++g )
        {
                GridItem* aboveItem = *g ;

                if ( aboveItem->getImageOfShadow() != nilPointer && aboveItem->getZ() > gridItem->getZ() )
                {
                # if  defined( DEBUG_SHADOWS )  &&  DEBUG_SHADOWS
                        std::cout << "casting shadow from " << aboveItem->whichKindOfItem() << " \"" << aboveItem->getLabel() << "\"" <<
                                " at x=" << aboveItem->getX() << " y=" << aboveItem->getY() << " z=" << aboveItem->getZ() <<
                                " on " << gridItem->whichKindOfItem() << " \"" << gridItem->getLabel() << "\"" <<
                                " at x=" << gridItem->getX() << " y=" << gridItem->getY() << " z=" << gridItem->getZ()
                                  << std::endl ;
                # endif

                        ShadowCaster::castShadowOnItem (
                                gridItem,
                                /* x */ gridItem->getOffsetX() + ( ( gridItem->getRawImage()->w - aboveItem->getImageOfShadow()->w ) >> 1 ),
                                /* y */ gridItem->getOffsetY() + gridItem->getRawImage()->h - room->getSizeOfOneTile() - gridItem->getHeight() - ( aboveItem->getImageOfShadow()->h >> 1 ),
                                /* shadow */ aboveItem->getImageOfShadow(),
                                /* shadingScale */ room->shadingScale
                                /* transparency = 0 */
                        ) ;
                }
        }

        // scroll thru list of free items to shade grid item
        for ( std::list< FreeItem* >::iterator f = freeItems.begin(); f != freeItems.end(); ++f )
        {
                FreeItem* freeItem = *f ;

                if ( freeItem->getImageOfShadow() != nilPointer && freeItem->getZ() > gridItem->getZ() )
                {
                        // range of columns met with item
                        int xStart = freeItem->getX() / tileSize;
                        int xEnd = ( freeItem->getX() + freeItem->getWidthX() - 1 ) / tileSize;
                        int yStart = ( freeItem->getY() - freeItem->getWidthY() + 1 ) / tileSize;
                        int yEnd = freeItem->getY() / tileSize;

                        // shade with free item above
                        if ( gridItem->getCellX() >= xStart && gridItem->getCellX() <= xEnd && gridItem->getCellY() >= yStart && gridItem->getCellY() <= yEnd )
                        {
                        # if  defined( DEBUG_SHADOWS )  &&  DEBUG_SHADOWS
                                std::cout << "casting shadow from " << freeItem->whichKindOfItem() << " \"" << freeItem->getLabel() << "\"" <<
                                        " at x=" << freeItem->getX() << " y=" << freeItem->getY() << " z=" << freeItem->getZ() <<
                                        " on " << gridItem->whichKindOfItem() << " \"" << gridItem->getLabel() << "\"" <<
                                        " at x=" << gridItem->getX() << " y=" << gridItem->getY() << " z=" << gridItem->getZ()
                                          << std::endl ;
                        # endif

                                ShadowCaster::castShadowOnItem (
                                        gridItem,
                                        /* x */ ( ( freeItem->getX() - freeItem->getY() ) << 1 ) + ( freeItem->getWidthX() + freeItem->getWidthY() ) - ( ( freeItem->getImageOfShadow()->w ) >> 1 ) - 1,
                                        /* y */ freeItem->getX() + freeItem->getY() + ( ( freeItem->getWidthX() - freeItem->getWidthY() + 1 ) >> 1 ) - ( ( freeItem->getImageOfShadow()->h ) >> 1 ) - gridItem->getZ() - gridItem->getHeight(),
                                        /* shadow */ freeItem->getImageOfShadow(),
                                        /* shadingScale */ room->shadingScale,
                                        /* transparency */ freeItem->getTransparency()
                                ) ;
                        }
                }
        }
}

void Mediator::castShadowOnFreeItem( FreeItem* freeItem )
{
        int tileSize = room->getSizeOfOneTile();

        // range of columns met with item
        int xStart = freeItem->getX() / tileSize;
        int xEnd = ( freeItem->getX() + freeItem->getWidthX() - 1 ) / tileSize;
        int yStart = ( freeItem->getY() - freeItem->getWidthY() + 1 ) / tileSize;
        int yEnd = freeItem->getY() / tileSize;

        // shadow from grid items above
        for ( int yCell = yStart; yCell <= yEnd; yCell++ )
        {
                for ( int xCell = xStart; xCell <= xEnd; xCell++ )
                {
                        int column = yCell * room->getTilesX() + xCell;

                        for ( std::list< GridItem* >::iterator g = structure[ column ].begin (); g != structure[ column ].end (); ++g )
                        {
                                GridItem* gridItem = *g ;

                                if ( gridItem->getImageOfShadow() != nilPointer && gridItem->getZ() > freeItem->getZ() )
                                {
                                # if  defined( DEBUG_SHADOWS )  &&  DEBUG_SHADOWS
                                        std::cout << "casting shadow from " << gridItem->whichKindOfItem() << " \"" << gridItem->getLabel() << "\"" <<
                                                        " at x=" << gridItem->getX() << " y=" << gridItem->getY() << " z=" << gridItem->getZ() <<
                                                        " on " << freeItem->whichKindOfItem() << " \"" << freeItem->getLabel() << "\"" <<
                                                        " at x=" << freeItem->getX() << " y=" << freeItem->getY() << " z=" << freeItem->getZ()
                                                        << std::endl ;
                                # endif

                                        ShadowCaster::castShadowOnItem (
                                                freeItem,
                                                /* x */ ( tileSize << 1 ) * ( xCell - yCell ) - ( gridItem->getImageOfShadow()->w >> 1 ) + 1,
                                                /* y */ tileSize * ( xCell + yCell + 1 ) - freeItem->getZ() - freeItem->getHeight() - ( gridItem->getImageOfShadow()->h >> 1 ) - 1,
                                                /* shadow */ gridItem->getImageOfShadow(),
                                                /* shadingScale */ room->shadingScale
                                                /* transparency = 0 */
                                        ) ;
                                }
                        }
                }
        }

        // scroll through list of free items
        for ( std::list< FreeItem* >::iterator f = freeItems.begin (); f != freeItems.end (); ++f )
        {
                FreeItem* shadeItem = *f ;

                if ( shadeItem->getImageOfShadow() != nilPointer && shadeItem->getId() != freeItem->getId() )
                {
                        // shadow with free item above
                        if ( freeItem->getZ() < shadeItem->getZ() &&
                                freeItem->getX() < shadeItem->getX() + static_cast< int >( shadeItem->getWidthX() ) &&
                                        shadeItem->getX() < freeItem->getX() + static_cast< int >( freeItem->getWidthX() ) &&
                                freeItem->getY() > shadeItem->getY() - static_cast< int >( shadeItem->getWidthY() ) &&
                                        shadeItem->getY() > freeItem->getY() - static_cast< int >( freeItem->getWidthY() ) )
                        {
                        # if  defined( DEBUG_SHADOWS )  &&  DEBUG_SHADOWS
                                std::cout << "casting shadow from " << shadeItem->whichKindOfItem() << " \"" << shadeItem->getLabel() << "\"" <<
                                        " at x=" << shadeItem->getX() << " y=" << shadeItem->getY() << " z=" << shadeItem->getZ() <<
                                        " on " << freeItem->whichKindOfItem() << " \"" << freeItem->getLabel() << "\"" <<
                                        " at x=" << freeItem->getX() << " y=" << freeItem->getY() << " z=" << freeItem->getZ()
                                          << std::endl ;
                        # endif

                                ShadowCaster::castShadowOnItem (
                                        freeItem,
                                        /* x */ ( ( shadeItem->getX() - shadeItem->getY() ) << 1 ) + shadeItem->getWidthX() + shadeItem->getWidthY() - ( shadeItem->getImageOfShadow()->w >> 1 ) - 1,
                                        /* y */ shadeItem->getX() + shadeItem->getY() - freeItem->getZ() - freeItem->getHeight() + ( ( shadeItem->getWidthX() - shadeItem->getWidthY() - shadeItem->getImageOfShadow()->h ) >> 1 ),
                                        /* shadow */ shadeItem->getImageOfShadow(),
                                        /* shadingScale */ room->shadingScale,
                                        /* transparency */ freeItem->getTransparency()
                                );
                        }
                }
        }
}

void Mediator::maskFreeItem( FreeItem* freeItem )
{
        // look for item in list
        // mask only with next items
        // for sorted list there’s no masking with previous items
        std::list< FreeItem * >::iterator f = std::find_if( freeItems.begin (), freeItems.end (), std::bind2nd( EqualItemId(), freeItem->getId() ) );
        if ( f == freeItems.end () ) /* there’s no such item in list */ return;

        while ( ++f != freeItems.end () ) // when there’s any next item
        {
                FreeItem* itemToMaskWith = *f ;

                if ( itemToMaskWith->getRawImage() && (
                        /* one of two is marked to mask and other of two isn’t transparent */
                        ( freeItem->whichMask() != NoMask && itemToMaskWith->getTransparency() == 0 ) ||
                        ( itemToMaskWith->whichMask() != NoMask && freeItem->getTransparency() == 0 ) )
                ) {
                        // do graphics overlap
                        if ( ( itemToMaskWith->getOffsetX() < freeItem->getOffsetX() + freeItem->getRawImage()->w ) &&
                                ( itemToMaskWith->getOffsetX() + itemToMaskWith->getRawImage()->w > freeItem->getOffsetX() ) &&
                                ( itemToMaskWith->getOffsetY() < freeItem->getOffsetY() + freeItem->getRawImage()->h ) &&
                                ( itemToMaskWith->getOffsetY() + itemToMaskWith->getRawImage()->h > freeItem->getOffsetY() ) )
                        {
                                // freeItem is behind itemToMaskWith
                                if ( ( freeItem->getX() + static_cast< int >( freeItem->getWidthX() ) <= itemToMaskWith->getX() ) ||
                                        ( freeItem->getY() <= itemToMaskWith->getY() - static_cast< int >( itemToMaskWith->getWidthY() ) ) ||
                                        ( freeItem->getZ() + static_cast< int >( freeItem->getHeight() ) <= itemToMaskWith->getZ() ) )
                                {
                                # if  defined( DEBUG_MASKS )  &&  DEBUG_MASKS
                                        std::cout << "masking " << freeItem->whichKindOfItem() << " \"" << freeItem->getLabel() << "\"" <<
                                                " at x=" << freeItem->getX() << " y=" << freeItem->getY() << " z=" << freeItem->getZ() <<
                                                " behind " << itemToMaskWith->whichKindOfItem() << " \"" << itemToMaskWith->getLabel() << "\"" <<
                                                " at x=" << itemToMaskWith->getX() << " y=" << itemToMaskWith->getY() << " z=" << itemToMaskWith->getZ()
                                                  << std::endl ;
                                # endif

                                        freeItem->maskImage( itemToMaskWith->getOffsetX(), itemToMaskWith->getOffsetY(), itemToMaskWith->getRawImage() );
                                }
                                // itemToMaskWith is behind freeItem
                                else
                                {
                                # if  defined( DEBUG_MASKS )  &&  DEBUG_MASKS
                                        std::cout << "masking " << itemToMaskWith->whichKindOfItem() << " \"" << itemToMaskWith->getLabel() << "\"" <<
                                                " at x=" << itemToMaskWith->getX() << " y=" << itemToMaskWith->getY() << " z=" << itemToMaskWith->getZ() <<
                                                " behind " << freeItem->whichKindOfItem() << " \"" << freeItem->getLabel() << "\"" <<
                                                " at x=" << freeItem->getX() << " y=" << freeItem->getY() << " z=" << freeItem->getZ()
                                                  << std::endl ;
                                # endif

                                        itemToMaskWith->maskImage( freeItem->getOffsetX(), freeItem->getOffsetY(), freeItem->getRawImage() );
                                }
                        }
                }
        }

        if ( freeItem->whichMask() != NoMask )
        {
                int xStart = freeItem->getX() / room->getSizeOfOneTile();
                int xEnd = ( ( freeItem->getX() + freeItem->getWidthX() - 1 ) / room->getSizeOfOneTile() ) + 1;
                int yStart = ( freeItem->getY() / room->getSizeOfOneTile() ) + 1;
                int yEnd = ( freeItem->getY() - freeItem->getWidthY() + 1 ) / room ->getSizeOfOneTile();

                do
                {
                        unsigned int i = 0;

                        while ( ( xStart + i < room->getTilesX() ) && ( yStart + i < room->getTilesY() ) )
                        {
                                int column = room->getTilesX() * ( yStart + i ) + xStart + i;

                                // proceed with grid items
                                for ( std::list< GridItem * >::iterator g = structure[ column ].begin (); g != structure[ column ].end (); ++g )
                                {
                                        GridItem* gridItem = *g ;

                                        if ( gridItem->getRawImage() )
                                        {
                                                // see if graphics overlap
                                                if ( ( gridItem->getOffsetX() < freeItem->getOffsetX() + freeItem->getRawImage()->w ) &&
                                                        ( gridItem->getOffsetX() + gridItem->getRawImage()->w > freeItem->getOffsetX() ) &&
                                                        ( gridItem->getOffsetY() < freeItem->getOffsetY() + freeItem->getRawImage()->h ) &&
                                                        ( gridItem->getOffsetY() + gridItem->getRawImage()->h > freeItem->getOffsetY() ) )
                                                {
                                                        // if free item is behind grid item
                                                        if ( ( freeItem->getX() + static_cast< int >( freeItem->getWidthX() ) <= static_cast< int >( ( xStart + i ) * room->getSizeOfOneTile() ) ) ||
                                                                ( freeItem->getY() <= static_cast< int >( ( yStart + i ) * room->getSizeOfOneTile() ) - 1 ) ||
                                                                ( freeItem->getZ() + static_cast< int >( freeItem->getHeight() ) <= gridItem->getZ() ) )
                                                        {
                                                        # if  defined( DEBUG_MASKS )  &&  DEBUG_MASKS
                                                                std::cout << "masking " << freeItem->whichKindOfItem() << " \"" << freeItem->getLabel() << "\"" <<
                                                                        " at x=" << freeItem->getX() << " y=" << freeItem->getY() << " z=" << freeItem->getZ() <<
                                                                        " behind " << gridItem->whichKindOfItem() << " \"" << gridItem->getLabel() << "\"" <<
                                                                        " at x=" << gridItem->getX() << " y=" << gridItem->getY() << " z=" << gridItem->getZ()
                                                                          << std::endl ;
                                                        # endif

                                                                freeItem->maskImage( gridItem->getOffsetX(), gridItem->getOffsetY(), gridItem->getRawImage() );
                                                        }
                                                }
                                        }
                                }

                                i++;
                        }

                        if ( yStart != yEnd )
                        {
                                yStart--;
                        }
                        else
                        {
                                xStart++;
                        }
                }
                while ( xStart <= xEnd );
        }
}

Item* Mediator::findItemById( int id )
{
        Item* item = nilPointer;

        // look for free item
        if ( id & 1 )
        {
                std::list< FreeItem * >::iterator f = std::find_if( freeItems.begin (), freeItems.end (), std::bind2nd( EqualItemId (), id ) );

                if ( f != freeItems.end () )
                {
                        item = dynamic_cast< Item * >( *f );
                }
        }
        // look for grid item
        else
        {
                std::list< GridItem * >::iterator g;

                for ( unsigned int i = 0; i < room->getTilesX() * room->getTilesY(); i++ )
                {
                        g = std::find_if( structure[ i ].begin (), structure[ i ].end (), std::bind2nd( EqualItemId (), id ) );

                        if ( g != structure[ i ].end() )
                        {
                                item = dynamic_cast< Item * >( *g );
                                i = room->getTilesX() * room->getTilesY();
                        }
                }
        }

        return item;
}

Item* Mediator::findItemByLabel( const std::string& label )
{
        Item* item = nilPointer;

        // look for free item
        std::list< FreeItem * >::iterator f = std::find_if( freeItems.begin (), freeItems.end (), std::bind2nd( EqualLabelOfItem (), label ) );

        if ( f != freeItems.end () )
        {
                item = dynamic_cast< Item * >( *f );
        }

        // look for grid item
        if ( item == nilPointer )
        {
                std::list< GridItem * >::iterator g;

                for ( unsigned int i = 0; i < room->getTilesX() * room->getTilesY(); i++ )
                {
                        g = std::find_if( structure[ i ].begin (), structure[ i ].end (), std::bind2nd( EqualLabelOfItem (), label ) );

                        if ( g != structure[ i ].end () )
                        {
                                item = dynamic_cast< Item * >( *g );
                                i = room->getTilesX() * room->getTilesY();
                        }
                }
        }

        return item;
}

Item* Mediator::findItemByBehavior( const std::string& behavior )
{
        Item* item = nilPointer;

        // look for free item
        std::list< FreeItem * >::iterator f = std::find_if( freeItems.begin (), freeItems.end (), std::bind2nd( EqualBehaviorOfItem (), behavior ) );

        if ( f != freeItems.end () )
        {
                item = dynamic_cast< Item * >( *f );
        }

        // look for grid item
        if ( item == nilPointer )
        {
                std::list< GridItem * >::iterator g;

                for ( unsigned int i = 0; i < room->getTilesX() * room->getTilesY(); i++ )
                {
                        g = std::find_if( structure[ i ].begin (), structure[ i ].end (), std::bind2nd( EqualBehaviorOfItem (), behavior ) );

                        if ( g != structure[ i ].end () )
                        {
                                item = dynamic_cast< Item * >( *g );
                                i = room->getTilesX() * room->getTilesY();
                        }
                }
        }

        return item;
}

bool Mediator::findCollisionWithItem( Item * item )
{
        bool collisionFound = false;

        // for grid item or free item with ability to detect collisions
        if ( dynamic_cast< GridItem * >( item ) || ( dynamic_cast< FreeItem * >( item ) && dynamic_cast< FreeItem * >( item )->isCollisionDetector() ) )
        {
                // traverse list of free items looking for collisions
                for ( std::list< FreeItem * >::iterator f = freeItems.begin (); f != freeItems.end (); ++f )
                {
                        FreeItem* freeItem = *f ;

                        if ( freeItem->getId() != item->getId() && freeItem->isCollisionDetector() )
                        {
                                // look for intersection
                                if ( ( freeItem->getX() + static_cast< int >( freeItem->getWidthX() ) > item->getX() ) &&
                                                ( freeItem->getX() < item->getX() + static_cast< int >( item->getWidthX() ) ) &&
                                        ( freeItem->getY() > item->getY() - static_cast< int >( item->getWidthX() ) ) &&
                                                ( freeItem->getY() - static_cast< int >( freeItem->getWidthY() ) < item->getY() ) &&
                                        ( freeItem->getZ() + static_cast< int >( freeItem->getHeight() ) > item->getZ() ) &&
                                                ( freeItem->getZ() < item->getZ() + static_cast< int >( item->getHeight() ) ) )
                                {
                                        collisions.push_back( freeItem->getId() );
                                        collisionFound = true;
                                }
                        }
                }

                // for grid item
                if ( item->whichKindOfItem() == "grid item" )
                {
                        GridItem* temp = dynamic_cast< GridItem * >( item );
                        int column = room->getTilesX() * temp->getCellY() + temp->getCellX();

                        // scroll through lists of grid items looking for collisions
                        for ( std::list< GridItem * >::iterator g = this->structure[ column ].begin (); g != this->structure[ column ].end (); ++g )
                        {
                                GridItem* gridItem = *g ;

                                if ( gridItem->getId() != item->getId() )
                                {
                                        // look for intersection
                                        if ( ( gridItem->getX() + static_cast< int >( gridItem->getWidthX() ) > item->getX() ) &&
                                                        ( gridItem->getX() < item->getX() + static_cast< int >( item->getWidthX() ) ) &&
                                                ( gridItem->getY() > item->getY() - static_cast< int >( item->getWidthX() ) ) &&
                                                        ( gridItem->getY() - static_cast< int >( gridItem->getWidthY() ) < item->getY() ) &&
                                                ( gridItem->getZ() + static_cast< int >( gridItem->getHeight() ) > item->getZ() ) &&
                                                        ( gridItem->getZ() < item->getZ() + static_cast< int >( item->getHeight() ) ) )
                                        {
                                                collisions.push_back( gridItem->getId() );
                                                collisionFound = true;
                                        }
                                }
                        }
                }
                // for free item
                else if( dynamic_cast< FreeItem * >( item ) )
                {
                        int xStart = item->getX() / room->tileSize;
                        int xEnd = ( item->getX() + item->getWidthX() - 1 ) / room->tileSize + 1;
                        int yStart = ( item->getY() - item->getWidthY() + 1 ) / room->tileSize;
                        int yEnd = item->getY() / room->tileSize + 1;

                        // in case when item collides with some wall
                        if ( xStart < 0 ) xStart = 0;
                        if ( xEnd > static_cast< int >( room->getTilesX () ) ) xEnd = room->getTilesX();
                        if ( yStart < 0 ) yStart = 0;
                        if ( yEnd > static_cast< int >( room->getTilesY () ) ) yEnd = room->getTilesY();

                        // see collision only in range of columns intersecting with item
                        for ( int i = xStart; i < xEnd; i++ )
                        {
                                for ( int j = yStart; j < yEnd; j++ )
                                {
                                        for ( std::list< GridItem * >::iterator g = this->structure[ room->getTilesX() * j + i ].begin ();
                                                g != this->structure[ room->getTilesX() * j + i ].end (); ++g )
                                        {
                                                GridItem* gridItem = static_cast< GridItem * >( *g );

                                                if ( ( gridItem->getZ() + static_cast< int >( gridItem->getHeight() ) > item->getZ() ) &&
                                                                ( gridItem->getZ() < item->getZ() + static_cast< int >( item->getHeight() ) ) )
                                                {
                                                        collisions.push_back( gridItem->getId() );
                                                        collisionFound = true;
                                                }
                                        }
                                }
                        }
                }
        }

        return collisionFound;
}

int Mediator::findHighestZ( Item * item )
{
        int z = 0 ;

        // go thru list of free items looking for highest in column
        for ( std::list< FreeItem * >::iterator f = freeItems.begin (); f != freeItems.end (); ++f )
        {
                FreeItem* freeItem = *f ;

                if ( freeItem->getId() != item->getId() && freeItem->isCollisionDetector() )
                {
                        // look for intersection on X and Y with higher Z
                        if ( ( freeItem->getX() + static_cast< int >( freeItem->getWidthX() ) > item->getX() ) &&
                                        ( freeItem->getX() < item->getX() + static_cast< int >( item->getWidthX() ) ) &&
                                ( freeItem->getY() > item->getY() - static_cast< int >( item->getWidthX() ) ) &&
                                        ( freeItem->getY() - static_cast< int >( freeItem->getWidthY() ) < item->getY() ) &&
                                ( freeItem->getZ() + static_cast< int >( freeItem->getHeight() ) > item->getZ() ) )
                        {
                                z = freeItem->getZ() + static_cast< int >( freeItem->getHeight() );
                        }
                }
        }

        if ( item->whichKindOfItem() == "grid item" )
        {
                GridItem* gridItem = dynamic_cast< GridItem * >( item );
                int column = room->getTilesX() * gridItem->getCellY() + gridItem->getCellX();

                // look for highest Z in the column
                for ( std::list< GridItem * >::iterator g = this->structure[ column ].begin (); g != this->structure[ column ].end (); ++g )
                {
                        GridItem* tempItem = *g ;

                        if ( tempItem->getZ() + static_cast< int >( tempItem->getHeight() ) > z )
                        {
                                z = tempItem->getZ() + tempItem->getHeight();
                        }
                }
        }
        else if ( item->whichKindOfItem() == "free item" || item->whichKindOfItem() == "player item" )
        {
                FreeItem* freeItem = dynamic_cast< FreeItem * >( item );

                // get columns where item is
                int xStart = freeItem->getX() / room->getSizeOfOneTile();
                int xEnd = ( freeItem->getX() + freeItem->getWidthX() - 1 ) / room->getSizeOfOneTile() + 1;
                int yStart = ( freeItem->getY() - freeItem->getWidthY() + 1 ) / room->getSizeOfOneTile();
                int yEnd = freeItem->getY() / room->getSizeOfOneTile() + 1;

                // walk thru items in columns met by item
                for ( int i = xStart; i < xEnd; i++ )
                {
                        for ( int j = yStart; j < yEnd; j++ )
                        {
                                int column( room->getTilesX() * j + i );
                                for ( std::list< GridItem * >::iterator g = structure[ column ].begin (); g != structure[ column ].end (); ++g )
                                {
                                        GridItem* gridItem = *g ;

                                        if ( gridItem->getZ() + static_cast< int >( gridItem->getHeight() ) > z )
                                        {
                                                z = gridItem->getZ() + gridItem->getHeight();
                                        }
                                }
                        }
                }
        }

        return z;
}

void Mediator::addItem( GridItem* gridItem )
{
        pthread_mutex_lock( &gridItemsMutex );

        int column( room->getTilesX() * gridItem->getCellY() + gridItem->getCellX() );

        structure[ column ].push_back( gridItem );
        structure[ column ].sort( sortGridItemList );

        pthread_mutex_unlock( &gridItemsMutex );
}

void Mediator::addItem( FreeItem* freeItem )
{
        freeItems.push_back( freeItem );
        freeItems.sort( sortFreeItemList );
}

void Mediator::removeItem( GridItem* gridItem )
{
        int column( room->getTilesX() * gridItem->getCellY() + gridItem->getCellX() );
        structure[ column ].erase(
                std::remove_if(
                        structure[ column ].begin(),
                        structure[ column ].end (),
                        std::bind2nd( EqualItemId(), gridItem->getId() ) ),
                structure[ column ].end () );
}

void Mediator::removeItem( FreeItem* freeItem )
{
        freeItems.erase(
                std::remove_if(
                        freeItems.begin (), freeItems.end (),
                        std::bind2nd( EqualItemId(), freeItem->getId() ) ),
                freeItems.end () );
}

void Mediator::pushCollision( int id )
{
        collisions.push_back( id );
}

int Mediator::popCollision()
{
        int id = 0;

        if ( ! collisions.empty() )
        {
                std::vector< int >::iterator i = collisions.begin ();
                id = *i;
                i = collisions.erase( i );
        }

        return id;
}

Item* Mediator::collisionWithByLabel( const std::string& label )
{
        for ( unsigned int i = 0; i < collisions.size(); i++ )
        {
                Item* item = findItemById( collisions[ i ] );

                if ( item != nilPointer && item->getLabel() == label )
                        return item;
        }

        return nilPointer;
}

Item* Mediator::collisionWithByBehavior( const std::string& behavior )
{
        for ( unsigned int i = 0; i < collisions.size(); i++ )
        {
                Item* item = findItemById( collisions[ i ] );

                if ( item != nilPointer && item->getBehavior() != nilPointer &&
                        item->getBehavior()->getNameOfBehavior () == behavior )
                {
                        return item;
                }
        }

        return nilPointer;
}

Item* Mediator::collisionWithBadBoy()
{
        for ( unsigned int i = 0; i < collisions.size(); i++ )
        {
                Item* item = findItemById( collisions[ i ] );

                if ( item != nilPointer && item->getBehavior() != nilPointer && item->isMortal()
                        && std::find( badBoys.begin(), badBoys.end(), item->getBehavior()->getNameOfBehavior () ) != badBoys.end() )
                {
                        return item;
                }
        }

        return nilPointer;
}

bool Mediator::selectNextPlayer( ItemDataManager* itemDataManager )
{
        // get exclusive access to the list of free elements
        pthread_mutex_lock( &freeItemsMutex );

        PlayerItem* previousPlayer = activePlayer;

        // search for next player
        std::list< PlayerItem * > playersInRoom = room->getPlayersYetInRoom();
        std::list< PlayerItem * >::iterator i = std::find_if(
                playersInRoom.begin (), playersInRoom.end (),
                std::bind2nd( EqualLabelOfItem(), activePlayer->getLabel() ) );
        ++i;
        activePlayer = ( i != playersInRoom.end () ? *i : *playersInRoom.begin () );

        // see if players may join here
        if ( previousPlayer != activePlayer )
        {
                const int delta = room->getSizeOfOneTile() >> 1;

                if ( ( previousPlayer->getLabel() == "head" && activePlayer->getLabel() == "heels" ) ||
                                ( previousPlayer->getLabel() == "heels" && activePlayer->getLabel() == "head" ) )
                {
                        if ( ( previousPlayer->getX() + delta >= activePlayer->getX() )
                                && ( previousPlayer->getX() + previousPlayer->getWidthX() - delta <= activePlayer->getX() + activePlayer->getWidthX() )
                                && ( previousPlayer->getY() + delta >= activePlayer->getY() )
                                && ( previousPlayer->getY() + previousPlayer->getWidthY() - delta <= activePlayer->getY() + activePlayer->getWidthY() )
                                && ( ( previousPlayer->getLabel() == "head" && previousPlayer->getZ() - LayerHeight == activePlayer->getZ() ) ||
                                        ( previousPlayer->getLabel() == "heels" && activePlayer->getZ() - LayerHeight == previousPlayer->getZ() ) ) )
                        {
                                PlayerItem* reference = previousPlayer->getLabel() == "heels" ? previousPlayer : activePlayer;
                                this->lastActivePlayer = previousPlayer->getLabel() == "heels" ? "heels" : "head";

                                int x = reference->getX();
                                int y = reference->getY();
                                int z = reference->getZ();
                                Way orientation = reference->getOrientation();

                                // item that Heels may have in handbag
                                PlayerItem* heels = reference;
                                ItemData* takenItemData = heels->getTakenItemData ();
                                BITMAP* takenItemImage = heels->getTakenItemImage ();
                                std::string behaviorOfItemTaken = heels->getTakenItemBehavior( );

                                // remove simple players
                                this->room->removePlayerFromRoom( previousPlayer, false );
                                this->room->removePlayerFromRoom( activePlayer, false );

                                // create composite player
                                std::auto_ptr< RoomBuilder > roomBuilder( new RoomBuilder( itemDataManager ) );
                                activePlayer = roomBuilder->createPlayerInRoom( this->room, false, "headoverheels", x, y, z, orientation );

                                // transfer item in handbag
                                if ( takenItemData != nilPointer )
                                {
                                        std::cout << "transfer item \"" << takenItemData->getLabel() << "\" to player \"" << activePlayer->getLabel() << "\"" << std::endl ;
                                        activePlayer->assignTakenItem( takenItemData, takenItemImage, behaviorOfItemTaken );
                                }

                                // release exclusive access to the list of free items
                                pthread_mutex_unlock( &freeItemsMutex );

                                std::cout << "join both characters into \"" << activePlayer->getLabel() << "\""
                                                << " in room " << room->getNameOfFileWithDataAboutRoom() << std::endl ;
                                return true;
                        }
                }
        }
        // is it composite player
        else if ( activePlayer->getLabel() == "headoverheels" )
        {
                std::cout << "split \"" << activePlayer->getLabel() << "\" in room " << room->getNameOfFileWithDataAboutRoom() << std::endl ;

                int x = activePlayer->getX();
                int y = activePlayer->getY();
                int z = activePlayer->getZ();
                Way orientation = activePlayer->getOrientation();

                // get data of item in handbag
                ItemData* takenItemData = activePlayer->getTakenItemData ();
                std::string behaviorOfItemTaken = activePlayer->getTakenItemBehavior( );
                BITMAP* takenItemImage = activePlayer->getTakenItemImage ();

                // remove composite player
                this->room->removePlayerFromRoom( activePlayer, false );

                // create simple players

                std::auto_ptr< RoomBuilder > roomBuilder( new RoomBuilder( itemDataManager ) );

                PlayerItem* heelsPlayer = roomBuilder->createPlayerInRoom( this->room, false, "heels", x, y, z, orientation );

                if ( takenItemData != nilPointer )
                {
                        std::cout << "transfer item \"" << takenItemData->getLabel() << "\" to player \"" << heelsPlayer->getLabel() << "\"" << std::endl ;
                        heelsPlayer->assignTakenItem( takenItemData, takenItemImage, behaviorOfItemTaken );
                }

                PlayerItem* headPlayer = roomBuilder->createPlayerInRoom( this->room, false, "head", x, y, z + LayerHeight, orientation );

                activePlayer = ( this->lastActivePlayer == "head" ) ? heelsPlayer : headPlayer;
                previousPlayer = ( activePlayer == headPlayer ) ? heelsPlayer : headPlayer;
        }

        // release exclusive access to the list of free items
        pthread_mutex_unlock( &freeItemsMutex );

        if ( previousPlayer == activePlayer )
        {
                return false;
        }

        std::cout << "swop character \"" << previousPlayer->getLabel() << "\" to character \"" << activePlayer->getLabel() << "\""
                        << " in room " << room->getNameOfFileWithDataAboutRoom() << std::endl ;
        return true;
}

void Mediator::toggleSwitchInRoom ()
{
        this->switchInRoomIsOn = ! this->switchInRoomIsOn ;

        // look for volatile and mortal free items to freeze them
        for ( std::list< FreeItem* >::iterator f = freeItems.begin (); f != freeItems.end (); ++f )
        {
                FreeItem* freeItem = *f;

                if ( freeItem != nilPointer && freeItem->getBehavior() != nilPointer )
                {
                        std::string behavior = freeItem->getBehavior()->getNameOfBehavior ();

                        if ( behavior == "behavior of disappearance on touch" || behavior == "behavior of disappearance on jump into" ||
                                        std::find( badBoys.begin (), badBoys.end (), behavior ) != badBoys.end () )
                        {
                                freeItem->getBehavior()->changeActivityOfItem( this->switchInRoomIsOn ? Freeze : WakeUp );
                        }
                }
        }

        // look for volatile grid items to freeze them
        for ( unsigned int i = 0; i < room->getTilesX() * room->getTilesY(); i++ )
        {
                for ( std::list< GridItem* >::iterator g = structure[ i ].begin (); g != structure[ i ].end (); ++g )
                {
                        GridItem* gridItem = *g;

                        if ( gridItem != nilPointer && gridItem->getBehavior() != nilPointer )
                        {
                                std::string behavior = gridItem->getBehavior()->getNameOfBehavior ();

                                if ( behavior == "behavior of disappearance on touch" || behavior == "behavior of disappearance on jump into" )
                                {
                                        gridItem->getBehavior()->changeActivityOfItem( this->switchInRoomIsOn ? Freeze : Wait );
                                }
                        }
                }
        }
}

bool Mediator::sortGridItemList( GridItem* g1, GridItem* g2 )
{
        return ( g1->getZ() < g2->getZ() + static_cast< int >( g2->getHeight() ) );
}

bool Mediator::sortFreeItemList( FreeItem* f1, FreeItem* f2 )
{
        return (
                ( f1->getZ() < f2->getZ() + static_cast< int >( f2->getHeight() ) ) ||
                ( f1->getX() < f2->getX() + static_cast< int >( f2->getWidthX() ) ) ||
                ( f1->getY() - static_cast< int >( f1->getWidthY() ) < f2->getY() )
        );
}

void Mediator::setActivePlayer( PlayerItem* playerItem )
{
        this->activePlayer = playerItem;
}

PlayerItem* Mediator::getWaitingPlayer() const
{
        std::list< PlayerItem * > playersInRoom = room->getPlayersYetInRoom();

        for ( std::list< PlayerItem * >::iterator p = playersInRoom.begin (); p != playersInRoom.end (); ++p )
        {
                if ( ( *p ) != this->activePlayer )
                {
                        return ( *p ) ;
                }
        }

        return nilPointer ;
}

bool EqualItemId::operator() ( Item* item, int id ) const
{
        return ( item->getId() == id );
}

bool EqualLabelOfItem::operator() ( Item* item, const std::string& label ) const
{
        return ( item->getLabel() == label );
}

bool EqualBehaviorOfItem::operator() ( Item* item, const std::string& behavior ) const
{
        return ( item->getBehavior() != nilPointer && item->getBehavior()->getNameOfBehavior () == behavior );
}

}

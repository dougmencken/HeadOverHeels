
#include "Mediator.hpp"
#include "Mediated.hpp"
#include "RoomBuilder.hpp"
#include "Item.hpp"
#include "ItemData.hpp"
#include "GridItem.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
#include "FloorTile.hpp"
#include "Behavior.hpp"


namespace isomot
{

Mediator::Mediator( Room* room )
{
        this->room = room;
        this->transparencies = new TableOfTransparencies() ;
        this->gridItemsSorting = false;
        this->freeItemsSorting = false;
        this->switchInRoomIsOn = false;
        this->activePlayer = 0;

        pthread_mutex_init( &gridItemsMutex, 0 );
        pthread_mutex_init( &freeItemsMutex, 0 );

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

        delete transparencies;
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
                        if ( static_cast< GridItem* >( *g )->update() )
                                gridItemsToDelete.push( static_cast< GridItem * >( *g ) );
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
                        dynamic_cast< FreeItem * >( *f )->setWhichMask( WantMask );
                }
        }

        if ( activePlayer != 0 )
        {
                activePlayer->behave ();
        }

        // Elementos libres que se eliminarán después de la actualización
        std::stack < FreeItem * > freeItemsToDelete;

        // Actualiza los elementos libres
        for ( std::list< FreeItem * >::iterator f = freeItems.begin (); f != freeItems.end (); ++f )
        {
                if ( static_cast< FreeItem* >( *f )->update() )
                        freeItemsToDelete.push( static_cast< FreeItem * >( *f ) );
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
                        room->setWayOfExit( ( *p )->getWayOfExit() );
                        room->setActive( false );
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

        pthread_create( &thread, 0, updateThread, reinterpret_cast< void * >( this ) );

        pthread_attr_destroy( &attr );
}

void Mediator::endUpdate()
{
        this->threadRunning = false;
        pthread_join( thread, 0 );
}

void* updateThread( void* thisClass )
{
        Mediator* mediator = reinterpret_cast< Mediator* >( thisClass );

        while( mediator->isThreadRunning() )
        {
                mediator->update();
                sleep( 10 );
        }

        pthread_exit( 0 );
}

void Mediator::remaskWithItem( FreeItem* item )
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
                                thatFreeItem->setWhichMask( WantMask );
                        }
                }
        }
}

void Mediator::remaskWithItem( GridItem* gridItem )
{
        if ( ! gridItem ) return;

        // go through list of free items to see which ones to remask
        for ( std::list< FreeItem* >::iterator f = freeItems.begin (); f != freeItems.end (); ++f )
        {
                FreeItem* freeItem = *f;

                if ( freeItem != 0 && freeItem->getRawImage() )
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
                                        freeItem->setWhichMask( WantMask );
                                }
                        }
                }
        }
}

void Mediator::reshadeWithItem( GridItem* item )
{
        // shade free items underneath
        this->shadeFreeItemsBeneathItem( item );

        // shade grid items below
        int column = room->getTilesX() * item->getCellY() + item->getCellX();

        if ( ! this->structure[ column ].empty() )
        {
                std::list< GridItem * >::iterator g = this->structure[ column ].begin ();
                GridItem* gridItem = *g;

                while( g != this->structure[ column ].end() && item->getId() != gridItem->getId() )
                {
                        gridItem->setWhichShade( WantShadow );
                        // Siguiente elemento rejilla de la columna
                        gridItem = *( ++g ) ;
                }
        }

        // Marca para sombrear la loseta de la columna, si existe
        if ( room->floor[ column ] != 0 )
                room->floor[ column ]->setWhichShade( WantShadow );
}

void Mediator::reshadeWithItem( FreeItem* item )
{
        // shade free items underneath
        this->shadeFreeItemsBeneathItem( item );

        // Rango de columnas interseccionadas por el elemento
        int xStart = item->getX() / room->tileSize;
        int xEnd = ( item->getX() + item->getWidthX() - 1 ) / room->tileSize + 1;
        int yStart = ( item->getY() - item->getWidthY() + 1 ) / room->tileSize;
        int yEnd = item->getY() / room->tileSize + 1;

        // Para toda columna que intersecciona con el elemento:
        for ( int i = xStart; i < xEnd; i++ )
        {
                for ( int j = yStart; j < yEnd; j++ )
                {
                        // Todo elemento de la columna que se encuentre por debajo, se marca para sombrear
                        int column = room->getTilesX() * j + i;
                        for ( std::list< GridItem* >::iterator g = structure[ column ]. begin (); g != structure[ column ]. end (); ++g )
                        {
                                GridItem* gridItem = static_cast< GridItem* >( *g );

                                if ( item->getZ () > gridItem->getZ () )
                                {
                                        gridItem->setWhichShade( WantShadow );
                                }
                        }

                        // Marca para sombrear la loseta de la columna
                        if ( room->floor[ column ] != 0 )
                                room->floor[ column ]->setWhichShade( WantShadow );
                }
        }
}

void Mediator::shadeFreeItemsBeneathItem( Item* item )
{
        for ( std::list < FreeItem* >::iterator f = freeItems.begin (); f != freeItems.end (); ++f )
        {
                FreeItem* freeItem = static_cast< FreeItem* >( *f );

                if ( freeItem->getId () != item->getId () )
                {
                        if ( ( freeItem->getX() + static_cast< int >( freeItem->getWidthX() ) > item->getX() )
                                && ( freeItem->getX() < item->getX() + static_cast< int >( item->getWidthX() ) )
                                && ( freeItem->getY() > freeItem->getY() - static_cast< int >( item->getWidthY() ) )
                                && ( freeItem->getY() - static_cast< int >( freeItem->getWidthY() ) < item->getY() )
                                && ( freeItem->getZ() < item->getZ() ) )
                        {
                                freeItem->setWhichShade( WantShadow );
                        }
                }
        }
}

void Mediator::castShadowOnFloor( FloorTile* floorTile )
{
        int xCell = floorTile->getX();
        int yCell = floorTile->getY();
        int column = floorTile->getColumn();
        int tileSize = room->getSizeOfOneTile();

        // Se sombrea con todos los elementos rejilla que pueda tener por encima
        for ( std::list< GridItem * >::iterator g = structure[ column ].begin (); g != structure[ column ].end (); ++g )
        {
                GridItem* gridItem = static_cast< GridItem * >( *g );

                // Si el elemento tiene sombra entonces se sombrea la imagen de la loseta
                if(  gridItem->getImageOfShadow() )
                {
                        floorTile->castShadowImage (
                                /* x */ ( tileSize << 1 ) * ( xCell - yCell ) - ( gridItem->getImageOfShadow()->w >> 1 ) + room->getX0() + 1,
                                /* y */ tileSize * ( xCell + yCell + 1 ) - ( gridItem->getImageOfShadow()->h >> 1 ) + room->getY0() - 1,
                                /* shadow */ gridItem->getImageOfShadow(),
                                /* shadingScale */ room->shadingScale
                                /* transparency = 0 (default) */
                        ) ;
                }
        }

        // Se sombrea con los elementos libres que pueda tener por encima
        for ( int percent = 0; percent <= 100; percent++ )
        {
                int howManyItems = transparencies->countItemsWithDegreeOfTransparency( percent );

                // Tiene que haber elementos en el porcentaje de transparencia actual
                if ( howManyItems != 0 )
                {
                        // Para todo elemento con ese grado de transparencia
                        for ( int n = 0; n < howManyItems; n++ )
                        {
                                // Recorre la lista de elementos libres para sombrear la loseta
                                for ( std::list< FreeItem* >::iterator f = freeItems.begin (); f != freeItems.end (); ++f )
                                {
                                        FreeItem* freeItem = static_cast< FreeItem* >( *f );

                                        if ( freeItem->getImageOfShadow() && freeItem->getTransparency() == percent )
                                        {
                                                // Rango de columnas que interseccionan por el elemento
                                                int xStart = freeItem->getX() / tileSize;
                                                int xEnd = ( freeItem->getX() + freeItem->getWidthX() - 1 ) / tileSize;
                                                int yStart = ( freeItem->getY() - freeItem->getWidthY() + 1 ) / tileSize;
                                                int yEnd = freeItem->getY() / tileSize;

                                                if ( xCell >= xStart && xCell <= xEnd && yCell >= yStart && yCell <= yEnd )
                                                {
                                                        floorTile->castShadowImage (
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
                }
        }
}

void Mediator::castShadowOnGrid( GridItem* gridItem )
{
        int tileSize = room->getSizeOfOneTile();
        int column = gridItem->getColumn();

        // shade with grid items it may have above
        for ( std::list< GridItem* >::iterator g = structure[ column ].begin(); g != structure[ column ].end(); ++g )
        {
                GridItem* tempItem = static_cast< GridItem* >( *g );

                if ( tempItem->getImageOfShadow() && tempItem->getZ() > gridItem->getZ() )
                {
                        gridItem->castShadowImage (
                                /* x */ gridItem->getOffsetX() + ( ( gridItem->getRawImage()->w - tempItem->getImageOfShadow()->w) >> 1 ),
                                /* y */ gridItem->getOffsetY() + gridItem->getRawImage()->h - room->getSizeOfOneTile() - gridItem->getHeight() - ( tempItem->getImageOfShadow()->h >> 1 ),
                                /* shadow */ tempItem->getImageOfShadow(),
                                /* shadingScale */ room->shadingScale
                                /* transparency = 0 (default) */
                        ) ;
                }
        }

        // shade with free items it may have above
        for ( int percent = 0; percent <= 100; percent++ )
        {
                int howManyItems = transparencies->countItemsWithDegreeOfTransparency( percent );

                // there are items with current amount of transparency
                if ( howManyItems != 0 )
                {
                        for ( int n = 0; n < howManyItems; n++ )
                        {
                                // scroll thru list of free items to shade this item
                                for ( std::list< FreeItem* >::iterator f = freeItems.begin(); f != freeItems.end(); ++f )
                                {
                                        FreeItem* freeItem = static_cast< FreeItem* >( *f );

                                        if ( freeItem->getImageOfShadow() && freeItem->getTransparency() == percent && freeItem->getZ() > gridItem->getZ() )
                                        {
                                                // Rango de columnas que interseccionan por el elemento
                                                int xStart = freeItem->getX() / tileSize;
                                                int xEnd = (freeItem->getX() + freeItem->getWidthX() - 1) / tileSize;
                                                int yStart = (freeItem->getY() - freeItem->getWidthY() + 1) / tileSize;
                                                int yEnd = freeItem->getY() / tileSize;

                                                if ( gridItem->getCellX() >= xStart && gridItem->getCellX() <= xEnd && gridItem->getCellY() >= yStart && gridItem->getCellY() <= yEnd )
                                                {
                                                        gridItem->castShadowImage (
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
                }
        }
}

void Mediator::castShadowOnFreeItem( FreeItem* freeItem )
{
        int tileSize = room->getSizeOfOneTile();

        // Rango de columnas que interseccionan con el elemento
        int xStart = freeItem->getX() / tileSize;
        int xEnd = ( freeItem->getX() + freeItem->getWidthX() - 1 ) / tileSize;
        int yStart = ( freeItem->getY() - freeItem->getWidthY() + 1 ) / tileSize;
        int yEnd = freeItem->getY() / tileSize;

        // shadow from grid items
        for ( int yCell = yStart; yCell <= yEnd; yCell++ )
        {
                for ( int xCell = xStart; xCell <= xEnd; xCell++ )
                {
                        int column = yCell * room->getTilesX() + xCell;

                        for ( std::list< GridItem* >::iterator g = structure[ column ].begin (); g != structure[ column ].end (); ++g )
                        {
                                GridItem* gridItem = static_cast< GridItem* >( *g );

                                if ( gridItem->getImageOfShadow() && gridItem->getZ() > freeItem->getZ() )
                                {
                                        freeItem->castShadowImage (
                                                /* x */ ( tileSize << 1 ) * ( xCell - yCell ) - ( gridItem->getImageOfShadow()->w >> 1 ) + 1,
                                                /* y */ tileSize * ( xCell + yCell + 1 ) - freeItem->getZ() - freeItem->getHeight() - ( gridItem->getImageOfShadow()->h >> 1 ) - 1,
                                                /* shadow */ gridItem->getImageOfShadow(),
                                                /* shadingScale */ room->shadingScale
                                                /* transparency = 0 (default) */
                                        ) ;
                                }
                        }
                }
        }

        // shadow from free items it may have above
        for ( int percent = 0; percent <= 100; percent++ )
        {
                int countOfItems = transparencies->countItemsWithDegreeOfTransparency( percent );

                // Tiene que haber elementos en el porcentaje de transparencia actual
                if ( countOfItems != 0 )
                {
                        // Para todo elemento con ese grado de transparencia
                        for ( int n = 0; n < countOfItems; n++ )
                        {
                                // Recorre la lista de elementos libres para sombrear el elemento
                                for ( std::list< FreeItem* >::iterator f = freeItems.begin (); f != freeItems.end (); ++f )
                                {
                                        FreeItem* tempItem = static_cast< FreeItem* >( *f );

                                        if ( tempItem->getImageOfShadow() && tempItem->getTransparency() == percent && tempItem->getId() != freeItem->getId() )
                                        {
                                                if ( freeItem->getZ() < tempItem->getZ() &&
                                                        freeItem->getX() < tempItem->getX() + static_cast< int >( tempItem->getWidthX() ) &&
                                                                tempItem->getX() < freeItem->getX() + static_cast< int >( freeItem->getWidthX() ) &&
                                                        freeItem->getY() > tempItem->getY() - static_cast< int >( tempItem->getWidthY() ) &&
                                                                tempItem->getY() > freeItem->getY() - static_cast< int >( freeItem->getWidthY() ) )
                                                {
                                                        freeItem->castShadowImage(
                                                                /* x */ ( ( tempItem->getX() - tempItem->getY() ) << 1 ) + tempItem->getWidthX() + tempItem->getWidthY() - ( tempItem->getImageOfShadow()->w >> 1 ) - 1,
                                                                /* y */ tempItem->getX() + tempItem->getY() - freeItem->getZ() - freeItem->getHeight() + ( ( tempItem->getWidthX() - tempItem->getWidthY() - tempItem->getImageOfShadow()->h ) >> 1 ),
                                                                /* shadow */ tempItem->getImageOfShadow(),
                                                                /* shadingScale */ room->shadingScale,
                                                                /* transparency */ freeItem->getTransparency()
                                                        );
                                                }
                                        }
                                }
                        }
                }
        }
}

void Mediator::mask( FreeItem* freeItem )
{
        // look for item in list
        // check cloaking only with next items
        // for sorted list there’s no cloaking with previous items
        std::list< FreeItem * >::iterator f = std::find_if( freeItems.begin (), freeItems.end (), std::bind2nd( EqualItemId(), freeItem->getId() ) );

        // Si se ha encontrado el elemento se procede a enmascarar
        if ( f != freeItems.end () )
        {
                // Se compara freeItem con los siguientes elementos libres de la lista para comprobar las ocultaciones
                while ( ++f != freeItems.end () )
                {
                        FreeItem* tempItem = static_cast< FreeItem * >( *f );

                        // El otro elemento tiene que tener imagen y uno de los dos tiene que estar
                        // marcado para enmascararlo; además el otro no tiene que ser transparente
                        if ( tempItem->getRawImage() &&
                                ( ( freeItem->whichMask() != NoMask && tempItem->getTransparency() == 0 ) ||
                                ( tempItem->whichMask() != NoMask && freeItem->getTransparency() == 0 ) ) )
                        {
                                // Se comprueba si sus gráficos se solapan
                                if ( ( tempItem->getOffsetX() < freeItem->getOffsetX() + freeItem->getRawImage()->w ) &&
                                        ( tempItem->getOffsetX() + tempItem->getRawImage()->w > freeItem->getOffsetX() ) &&
                                        ( tempItem->getOffsetY() < freeItem->getOffsetY() + freeItem->getRawImage()->h ) &&
                                        ( tempItem->getOffsetY() + tempItem->getRawImage()->h > freeItem->getOffsetY() ) )
                                {
                                        // freeItem está detrás de tempItem
                                        if ( ( freeItem->getX() + static_cast< int >( freeItem->getWidthX() ) <= tempItem->getX() ) ||
                                                ( freeItem->getY() <= tempItem->getY() - static_cast< int >( tempItem->getWidthY() ) ) ||
                                                ( freeItem->getZ() + static_cast< int >( freeItem->getHeight() ) <= tempItem->getZ() ) )
                                        {
                                                freeItem->maskImage( tempItem->getOffsetX(), tempItem->getOffsetY(), tempItem->getRawImage() );
                                        }
                                        // tempItem está detrás de freeItem
                                        else
                                        {
                                                tempItem->maskImage( freeItem->getOffsetX(), freeItem->getOffsetY(), freeItem->getRawImage() );
                                        }
                                }
                        }
                }
        }

        // Se compara freeItem con los elementos rejilla que tiene debajo para comprobar las ocultaciones
        int xStart = freeItem->getX() / room->getSizeOfOneTile();
        int xEnd = ( ( freeItem->getX() + freeItem->getWidthX() - 1 ) / room->getSizeOfOneTile() ) + 1;
        int yStart = ( freeItem->getY() / room->getSizeOfOneTile() ) + 1;
        int yEnd = ( freeItem->getY() - freeItem->getWidthY() + 1 ) / room ->getSizeOfOneTile();

        // Si el elemento libre hay que enmascararlo, se procede a hacerlo con los elementos rejilla
        if ( freeItem->whichMask() != NoMask )
        {
                do
                {
                        unsigned int i = 0;

                        while ( ( xStart + i < room->getTilesX() ) && ( yStart + i < room->getTilesY() ) )
                        {
                                int column = room->getTilesX() * ( yStart + i ) + xStart + i;

                                for ( std::list< GridItem * >::iterator g = structure[ column ].begin (); g != structure[ column ].end (); ++g )
                                {
                                        GridItem* gridItem = static_cast< GridItem * >( *g );

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
        Item* item = 0;

        // Búsqueda en la lista de elementos libres
        if ( id & 1 )
        {
                std::list< FreeItem * >::iterator f = std::find_if( freeItems.begin (), freeItems.end (), std::bind2nd( EqualItemId (), id ) );

                if ( f != freeItems.end () )
                {
                        item = dynamic_cast< Item * >( *f );
                }
        }
        // Búsqueda en las listas de elementos rejilla
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
        Item* item = 0;

        // search in free items
        std::list< FreeItem * >::iterator f = std::find_if( freeItems.begin (), freeItems.end (), std::bind2nd( EqualLabelOfItem (), label ) );

        if ( f != freeItems.end () )
        {
                item = dynamic_cast< Item * >( *f );
        }

        // search in grid items
        if ( item == 0 )
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
        Item* item = 0;

        // search in free items
        std::list< FreeItem * >::iterator f = std::find_if( freeItems.begin (), freeItems.end (), std::bind2nd( EqualBehaviorOfItem (), behavior ) );

        if ( f != freeItems.end () )
        {
                item = dynamic_cast< Item * >( *f );
        }

        // search in grid items
        if ( item == 0 )
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
                        FreeItem* freeItem = static_cast< FreeItem * >( *f );

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
                if ( dynamic_cast< GridItem * >( item ) )
                {
                        GridItem* temp = dynamic_cast< GridItem * >( item );
                        int column = room->getTilesX() * temp->getCellY() + temp->getCellX();

                        // scroll through lists of grid items looking for collisions
                        for ( std::list< GridItem * >::iterator g = this->structure[ column ].begin (); g != this->structure[ column ].end (); ++g )
                        {
                                GridItem* gridItem = static_cast< GridItem * >( *g );

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

        // Se recorre la lista de elementos libres buscando aquel que intersecta
        // con la columna y de mayor altura
        for ( std::list< FreeItem * >::iterator f = freeItems.begin (); f != freeItems.end (); ++f )
        {
                FreeItem* freeItem = static_cast< FreeItem * >( *f );

                if ( freeItem->getId() != item->getId() && freeItem->isCollisionDetector() )
                {
                        // Se busca la intersección en los ejes X e Y y la mayor Z
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

        // Búsqueda en la estructura de la sala en el caso de que sea un elemento rejilla
        if ( dynamic_cast< GridItem * >( item ) )
        {
                GridItem* gridItem = dynamic_cast< GridItem * >( item );
                int column( room->getTilesX() * gridItem->getCellY() + gridItem->getCellX() );

                // Se recorren los elementos de la columna buscando el mayor
                for ( std::list< GridItem * >::iterator g = this->structure[ column ].begin (); g != this->structure[ column ].end (); ++g )
                {
                        GridItem* tempItem = static_cast< GridItem * >( *g );

                        if ( tempItem->getZ() + static_cast< int >( tempItem->getHeight() ) > z )
                        {
                                z = tempItem->getZ() + tempItem->getHeight();
                        }
                }
        }
        // Búsqueda en la estructura de la sala en el caso de que sea un elemento libre
        else if ( dynamic_cast< FreeItem * >( item ) )
        {
                FreeItem* freeItem = dynamic_cast< FreeItem * >( item );

                // Rango de columnas intersectadas por el elemento
                int xStart = freeItem->getX() / room->getSizeOfOneTile();
                int xEnd = ( freeItem->getX() + freeItem->getWidthX() - 1 ) / room->getSizeOfOneTile() + 1;
                int yStart = ( freeItem->getY() - freeItem->getWidthY() + 1 ) / room->getSizeOfOneTile();
                int yEnd = freeItem->getY() / room->getSizeOfOneTile() + 1;

                // Se recorren los elementos de la columas intersectadas
                for ( int i = xStart; i < xEnd; i++ )
                {
                        for ( int j = yStart; j < yEnd; j++ )
                        {
                                int column( room->getTilesX() * j + i );
                                for ( std::list< GridItem * >::iterator g = structure[ column ].begin (); g != structure[ column ].end (); ++g )
                                {
                                        GridItem* gridItem = static_cast< GridItem * >( *g );

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

                if ( item != 0 && item->getLabel() == label )
                        return item;
        }

        return 0;
}

Item* Mediator::collisionWithByBehavior( const std::string& behavior )
{
        for ( unsigned int i = 0; i < collisions.size(); i++ )
        {
                Item* item = findItemById( collisions[ i ] );

                if ( item != 0 && item->getBehavior() != 0 && item->getBehavior()->getNameOfBehavior () == behavior )
                        return item;
        }

        return 0;
}

Item* Mediator::collisionWithBadBoy()
{
        for ( unsigned int i = 0; i < collisions.size(); i++ )
        {
                Item* item = findItemById( collisions[ i ] );

                if ( item != 0 && item->getBehavior() != 0 && item->isMortal()
                        && std::find( badBoys.begin(), badBoys.end(), item->getBehavior()->getNameOfBehavior () ) != badBoys.end() )
                {
                        return item;
                }
        }

        return 0;
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
                                if ( takenItemData != 0 )
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

                if ( takenItemData != 0 )
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

                if ( freeItem != 0 && freeItem->getBehavior() != 0 )
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

                        if ( gridItem != 0 && gridItem->getBehavior() != 0 )
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

        return 0 ;
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
        return ( item->getBehavior() != 0 && item->getBehavior()->getNameOfBehavior () == behavior );
}

}

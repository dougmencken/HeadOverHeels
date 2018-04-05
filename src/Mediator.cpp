
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

#include <algorithm> // std::for_each

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
        , activeCharacter( nilPointer )
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

        // make structure of room
        for ( unsigned int i = 0; i < room->getTilesX() * room->getTilesY() + 1; i++ )
        {
                gridItems.push_back( std::list< GridItem * > () );
        }
}

Mediator::~Mediator()
{
        // bin grid items
        for ( size_t i = 0; i < gridItems.size(); i++ )
        {
                std::for_each( gridItems[ i ].begin(), gridItems[ i ].end (), DeleteObject() );
                gridItems[ i ].clear();
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
        lockGridItemMutex ();

        if ( this->gridItemsSorting )
        {
                for ( unsigned int column = 0; column < gridItems.size(); column ++ )
                {
                        if ( ! gridItems[ column ].empty() )
                        {
                                gridItems[ column ].sort( sortColumnOfGridItems );
                        }
                }

                this->gridItemsSorting = false;
        }

        // grid items to remove after update
        std::stack< GridItem * > vanishedGridItems ;

        // update grid items
        for ( unsigned int column = 0; column < gridItems.size(); column ++ )
        {
                for ( std::list< GridItem* >::iterator g = gridItems[ column ].begin (); g != gridItems[ column ].end (); ++g )
                {
                        if ( ( *g )->update() )
                                vanishedGridItems.push( *g );
                }
        }

        while ( ! vanishedGridItems.empty() )
        {
                GridItem* gi = vanishedGridItems.top();
                room->removeGridItem( gi );
                vanishedGridItems.pop();
        }

        unlockGridItemMutex ();

        lockFreeItemMutex ();

        if ( this->freeItemsSorting )
        {
                freeItems.sort( sortListOfFreeItems );
                this->freeItemsSorting = false;

                // remask items after sorting because overlaps may change
                for ( std::list< FreeItem * >::iterator f = freeItems.begin (); f != freeItems.end (); ++f )
                {
                        ( *f )->setWhichMask( WantRemask );
                }
        }

        if ( activeCharacter != nilPointer )
        {
                activeCharacter->behave ();
        }

        // free items to remove after update
        std::stack < FreeItem * > vanishedFreeItems ;

        // update free items
        for ( std::list< FreeItem * >::iterator f = freeItems.begin (); f != freeItems.end (); ++f )
        {
                if ( ( *f )->update() )
                        vanishedFreeItems.push( *f );
        }

        while ( ! vanishedFreeItems.empty() )
        {
                FreeItem* fi = vanishedFreeItems.top();
                room->removeFreeItem( fi );
                vanishedFreeItems.pop();
        }

        std::list< PlayerItem * > playersInRoom = room->getPlayersYetInRoom();
        for ( std::list< PlayerItem * >::iterator p = playersInRoom.begin (); p != playersInRoom.end (); ++p )
        {
                // when inactive character falls down to room below this one
                // then make it active to let it fall
                if ( ( *p )->getWayOfExit() == "down" && ! ( *p )->isActiveCharacter() )
                {
                        std::cout << "inactive character \"" << ( *p )->getLabel() << "\" falls down to another room, swap characters to make it active" << std::endl ;
                        activeCharacter->setWayOfExit( "no exit" );
                        this->pickNextCharacter( nilPointer );
                }

                if ( ( *p )->isActiveCharacter() )
                {
                        if ( ( *p )->getWayOfExit() != "no exit" )
                        {
                                // transfer way of exit from player to room
                                room->setWayOfExit( ( *p )->getWayOfExit() );
                                ( *p )->setWayOfExit( "no exit" );
                        }

                        break; // only active character may exit room
                }
        }

        unlockFreeItemMutex ();
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

        while ( mediator->isThreadRunning() )
        {
                mediator->update();
                milliSleep( 10 );
        }

        pthread_exit( nilPointer );
}

void Mediator::remaskWithFreeItem( FreeItem* item )
{
        if ( item == nilPointer ) return;

        // go through list of free items to see which ones to remask
        for ( std::list< FreeItem* >::iterator f = freeItems.begin (); f != freeItems.end (); ++f )
        {
                FreeItem* thatFreeItem = *f;

                if ( thatFreeItem != nilPointer && thatFreeItem->getRawImage() != nilPointer &&
                        thatFreeItem != item && thatFreeItem->getUniqueName() + " copy" != item->getUniqueName() )
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
        if ( gridItem == nilPointer ) return;

        // go through list of free items to see which ones to remask
        for ( std::list< FreeItem* >::iterator f = freeItems.begin (); f != freeItems.end (); ++f )
        {
                FreeItem* freeItem = *f;

                if ( freeItem != nilPointer && freeItem->getRawImage() != nilPointer )
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

        int column = item->getColumnOfGrid();

        // shade grid items below
        if ( ! this->gridItems[ column ].empty() )
        {
                std::list< GridItem * >::iterator g = this->gridItems[ column ].begin ();
                GridItem* gridItem = *g;

                while ( g != this->gridItems[ column ].end() && item->getUniqueName() != gridItem->getUniqueName() + " copy" )
                {
                        gridItem->binProcessedImage();
                        gridItem->setWantShadow( true );

                        // next grid item in column
                        gridItem = *( ++g ) ;
                }
        }

        // shade floor in this column, if any
        if ( room->floor[ column ] != nilPointer )
        {
                room->floor[ column ]->freshShadyImage();
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
                        // mark to shade every item in this column that is below free item
                        int column = room->getTilesX() * j + i;
                        for ( std::list< GridItem* >::iterator g = gridItems[ column ]. begin (); g != gridItems[ column ]. end (); ++g )
                        {
                                GridItem* gridItem = *g ;

                                if ( item->getZ () > gridItem->getZ () )
                                {
                                        gridItem->binProcessedImage();
                                        gridItem->setWantShadow( true );
                                }
                        }

                        // shade floor in this column, if any
                        if ( room->floor[ column ] != nilPointer )
                        {
                                room->floor[ column ]->freshShadyImage ();
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

                if ( freeItem != item &&
                        item->getUniqueName() != freeItem->getUniqueName() + " copy" )
                {
                        int itemX = item->getX();       int itemY = item->getY();
                        int freeX = freeItem->getX();   int freeY = freeItem->getY();
                        int freeXend = freeX + static_cast< int >( freeItem->getWidthX() );
                        int freeYend = freeY - static_cast< int >( freeItem->getWidthY() );
                        int itemXend = itemX + static_cast< int >( item->getWidthX() );
                        int itemYend = itemY - static_cast< int >( item->getWidthY() );

                        if ( ( freeXend > itemX ) && ( freeX < itemXend )
                                && ( freeY > itemYend ) && ( freeYend < itemY )
                                && ( freeItem->getZ() < item->getZ() ) )
                        {
                        # if  defined( DEBUG_SHADOWS )  &&  DEBUG_SHADOWS
                                std::cout << "Mediator::shadeFreeItemsBeneathItem got item \"" << freeItem->getUniqueName ()
                                          << "\" to shade from \"" << item->getUniqueName () << "\""
                                          << std::endl ;
                        #endif

                                freeItem->binBothProcessedImages();
                                freeItem->setWantShadow( true );
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
        for ( std::list< GridItem * >::iterator g = gridItems[ column ].begin (); g != gridItems[ column ].end (); ++g )
        {
                GridItem* gridItem = *g ;

                // shade image of tile when item has shadow
                if ( gridItem->getImageOfShadow() != nilPointer )
                {
                # if  defined( DEBUG_SHADOWS )  &&  DEBUG_SHADOWS
                        std::cout << "casting shadow from " << gridItem->whichKindOfItem() << " \"" << gridItem->getUniqueName() << "\"" <<
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
                                std::cout << "casting shadow from " << freeItem->whichKindOfItem() << " \"" << freeItem->getUniqueName() << "\"" <<
                                        " at x=" << freeItem->getX() << " y=" << freeItem->getY() << " z=" << freeItem->getZ() <<
                                        " on floor tile at" <<
                                        " x=" << tileSize << "*" << xCell << "=" << xCell * tileSize <<
                                        " y=" << tileSize << "*" << yCell << "=" << yCell * tileSize
                                          << std::endl ;
                        # endif

                                ShadowCaster::castShadowOnFloor (
                                        floorTile,
                                        /* x */ ( ( freeItem->getX() - freeItem->getY() ) << 1 ) + room->getX0()
                                                        + static_cast< int >( freeItem->getWidthX() + freeItem->getWidthY() )
                                                        - ( ( freeItem->getImageOfShadow()->w ) >> 1 ) - 1,
                                        /* y */ freeItem->getX() + freeItem->getY() + room->getY0()
                                                        + ( ( static_cast< int >( freeItem->getWidthX() ) - static_cast< int >( freeItem->getWidthY() ) + 1 ) >> 1 )
                                                        - ( ( freeItem->getImageOfShadow()->h ) >> 1 ),
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
        int column = gridItem->getColumnOfGrid();

        // shade with grid items it may have above
        for ( std::list< GridItem* >::iterator g = gridItems[ column ].begin(); g != gridItems[ column ].end(); ++g )
        {
                GridItem* aboveItem = *g ;

                if ( aboveItem->getImageOfShadow() != nilPointer && aboveItem->getZ() > gridItem->getZ() )
                {
                # if  defined( DEBUG_SHADOWS )  &&  DEBUG_SHADOWS
                        std::ostringstream positionOfFrom;
                        positionOfFrom << "x=" << aboveItem->getX() << " y=" << aboveItem->getY() << " z=" << aboveItem->getZ() ;
                        std::ostringstream positionOfOn;
                        positionOfOn << "x=" << gridItem->getX() << " y=" << gridItem->getY() << " z=" << gridItem->getZ() ;

                        std::cout << "casting shadow from " << aboveItem->whichKindOfItem() << " \"" << aboveItem->getUniqueName() << "\"" << " at " << positionOfFrom.str() <<
                                        " on " << gridItem->whichKindOfItem() << " \"" << gridItem->getUniqueName() << "\"" << " at " << positionOfOn.str() << std::endl ;
                # endif

                        ShadowCaster::castShadowOnItem (
                                gridItem,
                                /* x */ gridItem->getOffsetX() + ( ( gridItem->getRawImage()->w - aboveItem->getImageOfShadow()->w ) >> 1 ),
                                /* y */ gridItem->getOffsetY() + gridItem->getRawImage()->h - tileSize - gridItem->getHeight() - ( aboveItem->getImageOfShadow()->h >> 1 ),
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
                                std::ostringstream positionOfFrom;
                                positionOfFrom << "x=" << freeItem->getX() << " y=" << freeItem->getY() << " z=" << freeItem->getZ() ;
                                std::ostringstream positionOfOn;
                                positionOfOn << "x=" << gridItem->getX() << " y=" << gridItem->getY() << " z=" << gridItem->getZ() ;

                                std::cout << "casting shadow from " << freeItem->whichKindOfItem() << " \"" << freeItem->getUniqueName() << "\"" << " at " << positionOfFrom.str() <<
                                                " on " << gridItem->whichKindOfItem() << " \"" << gridItem->getUniqueName() << "\"" << " at " << positionOfOn.str() << std::endl ;
                        # endif

                                ShadowCaster::castShadowOnItem (
                                        gridItem,
                                        /* x */ ( ( freeItem->getX() - freeItem->getY() ) << 1 )
                                                        + static_cast< int >( freeItem->getWidthX() + freeItem->getWidthY() )
                                                        - ( ( freeItem->getImageOfShadow()->w ) >> 1 ) - 1,
                                        /* y */ freeItem->getX() + freeItem->getY()
                                                        + ( ( static_cast< int >( freeItem->getWidthX() ) - static_cast< int >( freeItem->getWidthY() ) + 1 ) >> 1 )
                                                        - ( ( freeItem->getImageOfShadow()->h ) >> 1 ) - gridItem->getZ() - gridItem->getHeight(),
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

                        for ( std::list< GridItem* >::iterator g = gridItems[ column ].begin (); g != gridItems[ column ].end (); ++g )
                        {
                                GridItem* gridItem = *g ;

                                if ( gridItem->getImageOfShadow() != nilPointer && gridItem->getZ() > freeItem->getZ() )
                                {
                                # if  defined( DEBUG_SHADOWS )  &&  DEBUG_SHADOWS
                                        std::ostringstream positionOfFrom;
                                        positionOfFrom << "x=" << gridItem->getX() << " y=" << gridItem->getY() << " z=" << gridItem->getZ() ;
                                        std::ostringstream positionOfOn;
                                        positionOfOn << "x=" << freeItem->getX() << " y=" << freeItem->getY() << " z=" << freeItem->getZ() ;

                                        std::cout << "casting shadow from " << gridItem->whichKindOfItem() << " \"" << gridItem->getUniqueName() << "\"" << " at " << positionOfFrom.str() <<
                                                        " on " << freeItem->whichKindOfItem() << " \"" << freeItem->getUniqueName() << "\"" << " at " << positionOfOn.str() << std::endl ;
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
                FreeItem* aboveItem = *f ;

                if ( aboveItem->getImageOfShadow() != nilPointer && aboveItem->getUniqueName() != freeItem->getUniqueName() )
                {
                        // shadow with free item above
                        if ( freeItem->getZ() < aboveItem->getZ() &&
                                freeItem->getX() < aboveItem->getX() + static_cast< int >( aboveItem->getWidthX() ) &&
                                        aboveItem->getX() < freeItem->getX() + static_cast< int >( freeItem->getWidthX() ) &&
                                freeItem->getY() > aboveItem->getY() - static_cast< int >( aboveItem->getWidthY() ) &&
                                        aboveItem->getY() > freeItem->getY() - static_cast< int >( freeItem->getWidthY() ) )
                        {
                        # if  defined( DEBUG_SHADOWS )  &&  DEBUG_SHADOWS
                                std::ostringstream positionOfFrom;
                                positionOfFrom << "x=" << aboveItem->getX() << " y=" << aboveItem->getY() << " z=" << aboveItem->getZ() ;
                                std::ostringstream positionOfOn;
                                positionOfOn << "x=" << freeItem->getX() << " y=" << freeItem->getY() << " z=" << freeItem->getZ() ;

                                std::cout << "casting shadow from " << aboveItem->whichKindOfItem() << " \"" << aboveItem->getUniqueName() << "\"" << " at " << positionOfFrom.str() <<
                                        " on " << freeItem->whichKindOfItem() << " \"" << freeItem->getUniqueName() << "\"" << " at " << positionOfOn.str() << std::endl ;
                        # endif

                                ShadowCaster::castShadowOnItem (
                                        freeItem,
                                        /* x */ ( ( aboveItem->getX() - aboveItem->getY() ) << 1 )
                                                        + static_cast< int >( aboveItem->getWidthX() + aboveItem->getWidthY() )
                                                        - ( aboveItem->getImageOfShadow()->w >> 1 ) - 1,
                                        /* y */ aboveItem->getX() + aboveItem->getY() - freeItem->getZ() - freeItem->getHeight()
                                                        + ( ( static_cast< int >( aboveItem->getWidthX() ) - static_cast< int >( aboveItem->getWidthY() ) - aboveItem->getImageOfShadow()->h ) >> 1 ),
                                        /* shadow */ aboveItem->getImageOfShadow(),
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
        std::list< FreeItem * >::iterator f = std::find_if( freeItems.begin (), freeItems.end (), std::bind2nd( EqualUniqueNameOfItem(), freeItem->getUniqueName() ) );
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
                                        std::ostringstream positionOfItemBehind;
                                        positionOfItemBehind << "x=" << freeItem->getX() << " y=" << freeItem->getY() << " z=" << freeItem->getZ() ;
                                        std::ostringstream positionOfItem;
                                        positionOfItem << "x=" << itemToMaskWith->getX() << " y=" << itemToMaskWith->getY() << " z=" << itemToMaskWith->getZ() ;

                                        std::cout << "masking \"" << freeItem->getUniqueName() << "\" at " << positionOfItemBehind.str() <<
                                                        " behind \"" << itemToMaskWith->getUniqueName() << "\" at " << positionOfItem.str() << std::endl ;
                                # endif

                                        freeItem->maskImage( itemToMaskWith->getOffsetX(), itemToMaskWith->getOffsetY(), itemToMaskWith->getRawImage() );
                                }
                                // itemToMaskWith is behind freeItem
                                else
                                {
                                # if  defined( DEBUG_MASKS )  &&  DEBUG_MASKS
                                        std::ostringstream positionOfItemBehind;
                                        positionOfItemBehind << "x=" << itemToMaskWith->getX() << " y=" << itemToMaskWith->getY() << " z=" << itemToMaskWith->getZ() ;
                                        std::ostringstream positionOfItem;
                                        positionOfItem << "x=" << freeItem->getX() << " y=" << freeItem->getY() << " z=" << freeItem->getZ() ;

                                        std::cout << "masking \"" << itemToMaskWith->getUniqueName() << "\" at " << positionOfItemBehind.str() <<
                                                        " behind \"" << freeItem->getUniqueName() << "\" at " << positionOfItem.str() << std::endl ;
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
                                for ( std::list< GridItem * >::iterator g = gridItems[ column ].begin (); g != gridItems[ column ].end (); ++g )
                                {
                                        GridItem* gridItem = *g ;

                                        if ( gridItem->getRawImage() != nilPointer )
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
                                                                std::ostringstream positionOfItemBehind;
                                                                positionOfItemBehind << "x=" << freeItem->getX() << " y=" << freeItem->getY() << " z=" << freeItem->getZ() ;
                                                                std::ostringstream positionOfItem;
                                                                positionOfItem << "x=" << gridItem->getX() << " y=" << gridItem->getY() << " z=" << gridItem->getZ() ;

                                                                std::cout << "masking \"" << freeItem->getUniqueName() << "\" at " << positionOfItemBehind.str() <<
                                                                                " behind \"" << gridItem->getUniqueName() << "\" at " << positionOfItem.str() << std::endl ;
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

Item* Mediator::findItemByUniqueName( const std::string& uniqueName )
{
        Item* item = nilPointer;

        // first look for free item
        std::list< FreeItem * >::iterator f = std::find_if( freeItems.begin (), freeItems.end (), std::bind2nd( EqualUniqueNameOfItem (), uniqueName ) );

        if ( f != freeItems.end () )
        {
                item = dynamic_cast< Item * >( *f );
        }

        // then look for grid item
        if ( item == nilPointer )
        {
                std::list< GridItem * >::iterator g;

                for ( unsigned int column = 0; column < gridItems.size(); column ++ )
                {
                        g = std::find_if( gridItems[ column ].begin (), gridItems[ column ].end (), std::bind2nd( EqualUniqueNameOfItem (), uniqueName ) );

                        if ( g != gridItems[ column ].end() )
                        {
                                item = dynamic_cast< Item * >( *g );
                                break;
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

                for ( unsigned int column = 0; column < gridItems.size(); column ++ )
                {
                        g = std::find_if( gridItems[ column ].begin (), gridItems[ column ].end (), std::bind2nd( EqualLabelOfItem (), label ) );

                        if ( g != gridItems[ column ].end () )
                        {
                                item = dynamic_cast< Item * >( *g );
                                break;
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

                for ( unsigned int column = 0; column < gridItems.size(); column ++ )
                {
                        g = std::find_if( gridItems[ column ].begin (), gridItems[ column ].end (), std::bind2nd( EqualBehaviorOfItem (), behavior ) );

                        if ( g != gridItems[ column ].end () )
                        {
                                item = dynamic_cast< Item * >( *g );
                                break;
                        }
                }
        }

        return item;
}

bool Mediator::findCollisionWithItem( Item * item )
{
        bool collisionFound = false;

        // for grid item or free item with ability to detect collisions
        if ( item->whichKindOfItem() == "grid item" || ( dynamic_cast< FreeItem * >( item ) && dynamic_cast< FreeItem * >( item )->isCollisionDetector() ) )
        {
                // traverse list of free items looking for collisions
                for ( std::list< FreeItem * >::iterator f = freeItems.begin (); f != freeItems.end (); ++ f )
                {
                        FreeItem* freeItem = *f ;

                        if ( freeItem->getUniqueName() != item->getUniqueName() && freeItem->isCollisionDetector() )
                        {
                                // look for intersection
                                if ( ( freeItem->getX() + static_cast< int >( freeItem->getWidthX() ) > item->getX() ) &&
                                                ( freeItem->getX() < item->getX() + static_cast< int >( item->getWidthX() ) ) &&
                                        ( freeItem->getY() > item->getY() - static_cast< int >( item->getWidthX() ) ) &&
                                                ( freeItem->getY() - static_cast< int >( freeItem->getWidthY() ) < item->getY() ) &&
                                        ( freeItem->getZ() + static_cast< int >( freeItem->getHeight() ) > item->getZ() ) &&
                                                ( freeItem->getZ() < item->getZ() + static_cast< int >( item->getHeight() ) ) )
                                {
                                        collisions.push_back( freeItem->getUniqueName() );
                                        collisionFound = true;
                                }
                        }
                }

                // for grid item
                if ( item->whichKindOfItem() == "grid item" )
                {
                        int column = dynamic_cast< GridItem * >( item )->getColumnOfGrid() ;

                        // scroll through lists of grid items looking for collisions
                        for ( std::list< GridItem * >::iterator g = this->gridItems[ column ].begin (); g != this->gridItems[ column ].end (); ++ g )
                        {
                                GridItem* gridItem = *g ;

                                if ( gridItem->getUniqueName() != item->getUniqueName() )
                                {
                                        // look for intersection
                                        if ( ( gridItem->getX() + static_cast< int >( gridItem->getWidthX() ) > item->getX() ) &&
                                                        ( gridItem->getX() < item->getX() + static_cast< int >( item->getWidthX() ) ) &&
                                                ( gridItem->getY() > item->getY() - static_cast< int >( item->getWidthX() ) ) &&
                                                        ( gridItem->getY() - static_cast< int >( gridItem->getWidthY() ) < item->getY() ) &&
                                                ( gridItem->getZ() + static_cast< int >( gridItem->getHeight() ) > item->getZ() ) &&
                                                        ( gridItem->getZ() < item->getZ() + static_cast< int >( item->getHeight() ) ) )
                                        {
                                                collisions.push_back( gridItem->getUniqueName() );
                                                collisionFound = true;
                                        }
                                }
                        }
                }
                // for free item
                else if ( dynamic_cast< FreeItem * >( item ) )
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
                                        for ( std::list< GridItem * >::iterator g = this->gridItems[ room->getTilesX() * j + i ].begin ();
                                                g != this->gridItems[ room->getTilesX() * j + i ].end (); ++g )
                                        {
                                                GridItem* gridItem = static_cast< GridItem * >( *g );

                                                if ( ( gridItem->getZ() + static_cast< int >( gridItem->getHeight() ) > item->getZ() ) &&
                                                                ( gridItem->getZ() < item->getZ() + static_cast< int >( item->getHeight() ) ) )
                                                {
                                                        collisions.push_back( gridItem->getUniqueName() );
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

                if ( freeItem->getUniqueName() != item->getUniqueName() && freeItem->isCollisionDetector() )
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
                int column = dynamic_cast< GridItem * >( item )->getColumnOfGrid ();

                // look for highest Z in the column
                for ( std::list< GridItem * >::iterator g = this->gridItems[ column ].begin (); g != this->gridItems[ column ].end (); ++g )
                {
                        GridItem* gridItem = *g ;

                        if ( gridItem->getZ() + static_cast< int >( gridItem->getHeight() ) > z )
                        {
                                z = gridItem->getZ() + gridItem->getHeight();
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
                                int column = room->getTilesX() * j + i ;
                                for ( std::list< GridItem * >::iterator g = gridItems[ column ].begin (); g != gridItems[ column ].end (); ++g )
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

void Mediator::addGridItemToList( GridItem* gridItem )
{
        lockGridItemMutex ();

        int column = gridItem->getColumnOfGrid();

        gridItems[ column ].push_back( gridItem );
        gridItems[ column ].sort( sortColumnOfGridItems );

        unlockGridItemMutex ();
}

void Mediator::addFreeItemToList( FreeItem* freeItem )
{
        freeItems.push_back( freeItem );
        freeItems.sort( sortListOfFreeItems );
}

void Mediator::removeGridItemFromList( GridItem* gridItem )
{
        gridItems[ gridItem->getColumnOfGrid() ].remove( gridItem );
}

void Mediator::removeFreeItemFromList( FreeItem* freeItem )
{
        freeItems.remove( freeItem );
}

void Mediator::pushCollision( const std::string& name )
{
        collisions.push_back( name );
}

std::string Mediator::popCollision()
{
        std::string name = "";

        if ( ! collisions.empty() )
        {
                std::vector< std::string >::iterator it = collisions.begin ();
                name = *it ;
                it = collisions.erase( it );
        }

        return name;
}

Item* Mediator::collisionWithByLabel( const std::string& label )
{
        for ( unsigned int i = 0; i < collisions.size(); i++ )
        {
                Item* item = findItemByUniqueName( collisions[ i ] );

                if ( item != nilPointer && item->getLabel() == label )
                {
                        return item;
                }
        }

        return nilPointer;
}

Item* Mediator::collisionWithByBehavior( const std::string& behavior )
{
        for ( unsigned int i = 0; i < collisions.size(); i++ )
        {
                Item* item = findItemByUniqueName( collisions[ i ] );

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
                Item* item = findItemByUniqueName( collisions[ i ] );

                if ( item != nilPointer && item->getBehavior() != nilPointer && item->isMortal()
                        && std::find( badBoys.begin(), badBoys.end(), item->getBehavior()->getNameOfBehavior () ) != badBoys.end() )
                {
                        return item;
                }
        }

        return nilPointer;
}

void Mediator::setActiveCharacter ( PlayerItem * character )
{
        if ( this->activeCharacter != character )
        {
                this->activeCharacter = character ;

                if ( character != nilPointer )
                        this->labelOfActiveCharacter = character->getLabel ();
                else
                        this->labelOfActiveCharacter.clear();

                std::cout << "character \"" << labelOfActiveCharacter << "\" is yet active in room \"" << room->getNameOfFileWithDataAboutRoom() << "\"" << std::endl ;
        }
}

bool Mediator::pickNextCharacter( ItemDataManager* itemDataManager )
{
        PlayerItem* previousCharacter = activeCharacter;

        // search for next player
        std::list< PlayerItem * > playersInRoom = room->getPlayersYetInRoom();
        std::list< PlayerItem * >::iterator i = std::find_if(
                playersInRoom.begin (), playersInRoom.end (),
                std::bind2nd( EqualLabelOfItem(), activeCharacter->getLabel() ) );
        ++ i;
        setActiveCharacter( i != playersInRoom.end () ? *i : *playersInRoom.begin () );

        // see if players may join here
        if ( previousCharacter != activeCharacter && itemDataManager != nilPointer )
        {
                const int delta = room->getSizeOfOneTile() >> 1;

                if ( ( previousCharacter->getLabel() == "head" && activeCharacter->getLabel() == "heels" ) ||
                                ( previousCharacter->getLabel() == "heels" && activeCharacter->getLabel() == "head" ) )
                {
                        if ( ( previousCharacter->getX() + delta >= activeCharacter->getX() )
                                && ( previousCharacter->getX() + previousCharacter->getWidthX() - delta <= activeCharacter->getX() + activeCharacter->getWidthX() )
                                && ( previousCharacter->getY() + delta >= activeCharacter->getY() )
                                && ( previousCharacter->getY() + previousCharacter->getWidthY() - delta <= activeCharacter->getY() + activeCharacter->getWidthY() )
                                && ( ( previousCharacter->getLabel() == "head" && previousCharacter->getZ() - LayerHeight == activeCharacter->getZ() ) ||
                                        ( previousCharacter->getLabel() == "heels" && activeCharacter->getZ() - LayerHeight == previousCharacter->getZ() ) ) )
                        {
                                lockFreeItemMutex ();

                                PlayerItem* reference = previousCharacter->getLabel() == "heels" ? previousCharacter : activeCharacter;
                                this->lastActiveCharacterBeforeJoining = previousCharacter->getLabel() == "heels" ? "heels" : "head";

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
                                this->room->removePlayerFromRoom( previousCharacter, false );
                                this->room->removePlayerFromRoom( activeCharacter, false );

                                // create composite player
                                setActiveCharacter( RoomBuilder::createPlayerInRoom( this->room, "headoverheels", false, x, y, z, orientation ) );

                                // transfer item in handbag
                                if ( takenItemData != nilPointer )
                                {
                                        std::cout << "transfer item \"" << takenItemData->getLabel() << "\" to player \"" << activeCharacter->getLabel() << "\"" << std::endl ;
                                        activeCharacter->assignTakenItem( takenItemData, takenItemImage, behaviorOfItemTaken );
                                }

                                unlockFreeItemMutex ();

                                std::cout << "join both characters into \"" << activeCharacter->getLabel() << "\""
                                                << " in room " << room->getNameOfFileWithDataAboutRoom() << std::endl ;
                                return true;
                        }
                }
        }
        // is it composite player
        else if ( activeCharacter->getLabel() == "headoverheels" && itemDataManager != nilPointer )
        {
                std::cout << "split \"" << activeCharacter->getLabel() << "\" in room " << room->getNameOfFileWithDataAboutRoom() << std::endl ;

                int x = activeCharacter->getX();
                int y = activeCharacter->getY();
                int z = activeCharacter->getZ();
                Way orientation = activeCharacter->getOrientation();

                lockFreeItemMutex ();

                // get data of item in handbag
                ItemData* takenItemData = activeCharacter->getTakenItemData ();
                std::string behaviorOfItemTaken = activeCharacter->getTakenItemBehavior( );
                BITMAP* takenItemImage = activeCharacter->getTakenItemImage ();

                // remove composite player
                this->room->removePlayerFromRoom( activeCharacter, false );

                // create simple players

                PlayerItem* heelsPlayer = RoomBuilder::createPlayerInRoom( this->room, "heels", false, x, y, z, orientation );

                if ( takenItemData != nilPointer )
                {
                        std::cout << "transfer item \"" << takenItemData->getLabel() << "\" to player \"" << heelsPlayer->getLabel() << "\"" << std::endl ;
                        heelsPlayer->assignTakenItem( takenItemData, takenItemImage, behaviorOfItemTaken );
                }

                PlayerItem* headPlayer = RoomBuilder::createPlayerInRoom( this->room, "head", false, x, y, z + LayerHeight, orientation );

                setActiveCharacter( ( this->lastActiveCharacterBeforeJoining == "head" ) ? heelsPlayer : headPlayer );
                previousCharacter = ( activeCharacter == headPlayer ) ? heelsPlayer : headPlayer;

                unlockFreeItemMutex ();
        }

        if ( previousCharacter == activeCharacter )
        {
                return false;
        }

        std::cout << "swop character \"" << previousCharacter->getLabel() << "\" to character \"" << activeCharacter->getLabel() << "\""
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
        for ( unsigned int column = 0; column < gridItems.size(); column ++ )
        {
                for ( std::list< GridItem* >::iterator g = gridItems[ column ].begin (); g != gridItems[ column ].end (); ++g )
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

        std::cout << "toggled switch in room " << getRoom()->getNameOfFileWithDataAboutRoom() << std::endl ;
}

bool Mediator::sortColumnOfGridItems( GridItem* g1, GridItem* g2 )
{
        return ( g1->getZ() < g2->getZ() + static_cast< int >( g2->getHeight() ) );
}

bool Mediator::sortListOfFreeItems( FreeItem* f1, FreeItem* f2 )
{
        return (
                ( f1->getZ() < f2->getZ() + static_cast< int >( f2->getHeight() ) ) ||
                ( f1->getX() < f2->getX() + static_cast< int >( f2->getWidthX() ) ) ||
                ( f1->getY() - static_cast< int >( f1->getWidthY() ) < f2->getY() )
        );
}

PlayerItem* Mediator::getWaitingCharacter() const
{
        std::list< PlayerItem * > playersInRoom = room->getPlayersYetInRoom();

        for ( std::list< PlayerItem * >::iterator p = playersInRoom.begin (); p != playersInRoom.end (); ++p )
        {
                if ( ( *p ) != this->activeCharacter )
                {
                        return ( *p ) ;
                }
        }

        return nilPointer ;
}

bool EqualUniqueNameOfItem::operator() ( Item* item, const std::string& uniqueName ) const
{
        return ( item != nilPointer && item->getUniqueName() == uniqueName );
}

bool EqualLabelOfItem::operator() ( Item* item, const std::string& label ) const
{
        return ( item != nilPointer && item->getLabel() == label );
}

bool EqualBehaviorOfItem::operator() ( Item* item, const std::string& behavior ) const
{
        return ( item != nilPointer && item->getBehavior() != nilPointer && item->getBehavior()->getNameOfBehavior () == behavior );
}

}

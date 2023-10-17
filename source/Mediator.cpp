
#include "Mediator.hpp"
#include "Mediated.hpp"
#include "RoomBuilder.hpp"
#include "GameManager.hpp"
#include "DescriptionOfItem.hpp"
#include "FloorTile.hpp"
#include "ShadowCaster.hpp"
#include "Masker.hpp"
#include "Behavior.hpp"

#include "sleep.hpp"

#include <algorithm>

#ifdef DEBUG
#  define DEBUG_COLLISIONS      0
#  define DEBUG_SHADOWS         0
#  define DEBUG_MASKS           0
#endif


namespace iso
{

Mediator::Mediator( Room* room )
        : room( room )
        , threadRunning( false )
        , needGridItemsSorting( false )
        , needFreeItemsSorting( false )
        , switchInRoomIsOn( false )
        , activeCharacter( nilPointer )
{
        pthread_mutex_init( &gridItemsMutex, nilPointer );
        pthread_mutex_init( &freeItemsMutex, nilPointer );
        pthread_mutex_init( &collisionsMutex, nilPointer );

        // behaviors of items which may be freezed by doughnut or switch
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
                room->gridItems.push_back( std::vector< GridItemPtr > () );
        }
}

Mediator::~Mediator()
{
        pthread_mutex_destroy( &gridItemsMutex );
        pthread_mutex_destroy( &freeItemsMutex );
        pthread_mutex_destroy( &collisionsMutex );
}

void Mediator::update()
{
# if defined( DEBUG_COLLISIONS ) && DEBUG_COLLISIONS
        if ( ! collisions.empty () )
        {
                std::cout << "stack of collisions" << std::endl ;
                for ( std::deque< std::string >::const_iterator c = collisions.begin () ; c != collisions.end () ; ++ c )
                        std::cout << "   " << *c << std::endl ;
        }
# endif

        lockGridItemsMutex ();

        // grid items to remove after update
        std::vector< std::string > vanishedGridItems ;

        // update grid items
        for ( unsigned int column = 0; column < room->gridItems.size(); ++ column )
        {
                for ( std::vector< GridItemPtr >::iterator g = room->gridItems[ column ].begin (); g != room->gridItems[ column ].end (); ++ g )
                {
                        if ( ( *g )->updateItem() )
                                vanishedGridItems.push_back( ( *g )->getUniqueName() );
                }
        }

        for ( std::vector< std::string >::const_iterator i = vanishedGridItems.begin () ; i != vanishedGridItems.end () ; ++ i )
        {
                std::cout << "grid item \"" << *i << "\" is to be gone" << std::endl ;
                room->removeGridItemByUniqueName( *i );
        }

        if ( needGridItemsSorting )
        {
                for ( unsigned int column = 0; column < room->gridItems.size(); ++ column )
                {
                        if ( ! room->gridItems[ column ].empty() )
                        {
                                std::sort( room->gridItems[ column ].begin (), room->gridItems[ column ].end () );
                        }
                }

                needGridItemsSorting = false;
        }

        unlockGridItemsMutex ();

        if ( activeCharacter != nilPointer )
        {
                activeCharacter->behave ();
        }

        lockFreeItemsMutex ();

        // free items to remove after update
        std::vector< std::string > vanishedFreeItems ;

        // update free items
        for ( std::vector< FreeItemPtr >::iterator f = room->freeItems.begin (); f != room->freeItems.end (); ++ f )
        {
                if ( ( *f )->updateItem() )
                        vanishedFreeItems.push_back( ( *f )->getUniqueName() );
        }

        for ( std::vector< std::string >::const_iterator i = vanishedFreeItems.begin () ; i != vanishedFreeItems.end () ; ++ i )
        {
                std::cout << "free item \"" << *i << "\" is to be gone" << std::endl ;
                room->removeFreeItemByUniqueName( *i );
        }

        if ( needFreeItemsSorting )
        {
                std::sort( room->freeItems.begin (), room->freeItems.end () );
                needFreeItemsSorting = false;

                // remask items after sorting because overlaps may change
                for ( std::vector< FreeItemPtr >::iterator f = room->freeItems.begin (); f != room->freeItems.end (); ++ f )
                {
                        ( *f )->setWantMaskTrue();
                }
        }

        std::vector< AvatarItemPtr > charactersInRoom = room->getCharactersYetInRoom() ;
        for ( std::vector< AvatarItemPtr >::iterator p = charactersInRoom.begin (); p != charactersInRoom.end (); ++ p )
        {
                // when inactive character falls down to room below this one
                // then make it active to let it fall
                if ( ( *p )->getWayOfExit() == "below" && ! ( *p )->isActiveCharacter() )
                {
                        std::cout << "inactive character \"" << ( *p )->getLabel() << "\" falls down to another room, swap characters to make it active" << std::endl ;
                        activeCharacter->setWayOfExit( "did not quit" );
                        this->pickNextCharacter () ;
                }
        }

        unlockFreeItemsMutex ();
}

void Mediator::beginUpdate ()
{
        if ( this->threadRunning ) return ;

        IF_DEBUG( fprintf ( stdout, "Mediator::beginUpdate ()\n" ) )

        pthread_attr_t attr;

        pthread_attr_init( &attr );
        pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE );

        pthread_create( &thread, &attr, Mediator::updateThread, reinterpret_cast< void * >( this ) );

        this->threadRunning = true;

        pthread_attr_destroy( &attr );
}

void Mediator::endUpdate ()
{
        if ( ! this->threadRunning ) return ;

        IF_DEBUG( fprintf ( stdout, "Mediator::endUpdate ()\n" ) )

        this->threadRunning = false;

        pthread_join( thread, nilPointer );
}

void* Mediator::updateThread( void* mediatorAsVoid )
{
        Mediator* mediator = reinterpret_cast< Mediator* >( mediatorAsVoid );

        while ( mediator->isThreadRunning() )
        {
                mediator->update() ;
                somn::milliSleep( 1000 / Isomot::updatesPerSecond );
        }

        pthread_exit( nilPointer );
}

void Mediator::wantToMaskWithFreeItem( const FreeItem& item )
{
        wantToMaskWithFreeItemAt( item, item.getOffset () );
}

void Mediator::wantToMaskWithFreeItemAt( const FreeItem& item, std::pair< int, int > offset )
{
        for ( std::vector< FreeItemPtr >::iterator f = room->freeItems.begin (); f != room->freeItems.end (); ++ f )
        {
                FreeItem& thatFreeItem = *( *f );

                if ( thatFreeItem.getUniqueName() != item.getUniqueName() &&
                        thatFreeItem.getUniqueName() + " copy" != item.getUniqueName() )
                {
                        if ( item.doGraphicsOverlapAt( thatFreeItem, offset ) )
                        {
                                thatFreeItem.setWantMaskTrue();
                        }
                }
        }
}

void Mediator::wantToMaskWithGridItem( const GridItem& gridItem )
{
        wantToMaskWithGridItemAt( gridItem, gridItem.getX (), gridItem.getY (), gridItem.getZ (), gridItem.getOffset () );
}

void Mediator::wantToMaskWithGridItemAt( const GridItem& gridItem, int x, int y, int z, std::pair< int, int > offset )
{
        for ( std::vector< FreeItemPtr >::iterator f = room->freeItems.begin (); f != room->freeItems.end (); ++ f )
        {
                FreeItem& freeItem = *( *f );

                if ( gridItem.doGraphicsOverlapAt( freeItem, offset ) )
                {
                        // mask when free item is behind grid item
                        if ( freeItem.isBehindAt( gridItem, x, y, z ) )
                        {
                                freeItem.setWantMaskTrue();
                        }
                }
        }
}

void Mediator::wantShadowFromGridItem( const GridItem& item )
{
        if ( room->getOpacityOfShadows() >= 256 ) return ;

        shadeFreeItemsBeneathItemAt( item, item.getX (), item.getY (), item.getZ () );

        int column = item.getColumnOfGrid();

        // shade grid items below
        if ( ! room->gridItems[ column ].empty() )
        {
                std::vector< GridItemPtr >::iterator g = room->gridItems[ column ].begin ();

                while ( g != room->gridItems[ column ].end() && item.getUniqueName() != ( *g )->getUniqueName() + " copy" )
                {
                        GridItem& gridItem = *( *g );

                        gridItem.freshProcessedImage();
                        gridItem.setWantShadow( true );

                        ++ g ;
                }
        }

        // shade floor in this column, if any
        if ( room->floorTiles[ column ] != nilPointer )
        {
                room->floorTiles[ column ]->freshShadyImage();
                room->floorTiles[ column ]->setWantShadow( true );
        }
}

void Mediator::wantShadowFromFreeItem( const FreeItem& item )
{
        wantShadowFromFreeItemAt( item, item.getX (), item.getY (), item.getZ () );
}

void Mediator::wantShadowFromFreeItemAt( const FreeItem& item, int x, int y, int z )
{
        if ( room->getOpacityOfShadows() >= 256 ) return ;

        shadeFreeItemsBeneathItemAt( item, x, y, z );

        // range of columns met with item
        int xStart = x / room->tileSize ;
        int xEnd = ( x + item.getWidthX() - 1 ) / room->tileSize + 1 ;
        int yStart = ( y - item.getWidthY() + 1 ) / room->tileSize ;
        int yEnd = y / room->tileSize + 1 ;

        // for every column that item meets
        for ( int i = xStart; i < xEnd; i++ )
        {
                for ( int j = yStart; j < yEnd; j++ )
                {
                        // mark to shade every item in this column that is below free item
                        int column = room->getTilesX() * j + i;
                        for ( std::vector< GridItemPtr >::iterator g = room->gridItems[ column ]. begin (); g != room->gridItems[ column ]. end (); ++ g )
                        {
                                GridItem& gridItem = *( *g );

                                if ( z > gridItem.getZ () )
                                {
                                        gridItem.freshProcessedImage();
                                        gridItem.setWantShadow( true );
                                }
                        }

                        // shade floor in this column, if any
                        if ( room->floorTiles[ column ] != nilPointer )
                        {
                                room->floorTiles[ column ]->freshShadyImage ();
                                room->floorTiles[ column ]->setWantShadow( true );
                        }
                }
        }
}

void Mediator::shadeFreeItemsBeneathItemAt( const Item& item, int x, int y, int z )
{
        for ( std::vector< FreeItemPtr >::iterator f = room->freeItems.begin (); f != room->freeItems.end (); ++ f )
        {
                FreeItem& freeItem = *( *f );

                if ( freeItem.getUniqueName() != item.getUniqueName() &&
                        item.getUniqueName() != freeItem.getUniqueName() + " copy" )
                {
                        int itemX = x ;
                        int itemY = y ;
                        int freeX = freeItem.getX();
                        int freeY = freeItem.getY();
                        int freeXend = freeX + static_cast< int >( freeItem.getWidthX() );
                        int freeYend = freeY - static_cast< int >( freeItem.getWidthY() );
                        int itemXend = itemX + static_cast< int >( item.getWidthX() );
                        int itemYend = itemY - static_cast< int >( item.getWidthY() );

                        if ( ( freeXend > itemX ) && ( freeX < itemXend )
                                && ( freeY > itemYend ) && ( freeYend < itemY )
                                && ( freeItem.getZ() < z ) )
                        {
                        # if  defined( DEBUG_SHADOWS )  &&  DEBUG_SHADOWS
                                std::cout << "Mediator::shadeFreeItemsBeneathItem got item \"" << freeItem.getUniqueName ()
                                          << "\" to shade from \"" << item.getUniqueName () << "\""
                                          << std::endl ;
                        #endif

                                freeItem.freshBothProcessedImages();
                                freeItem.setWantShadow( true );
                        }
                }
        }
}

void Mediator::castShadowOnFloor( FloorTile& floorTile )
{
        if ( room->getOpacityOfShadows() >= 256 ) return ;

        int xCell = floorTile.getCellX();
        int yCell = floorTile.getCellY();
        int column = floorTile.getColumn();
        int tileSize = room->getSizeOfOneTile();

        // shade with every grid item above
        for ( std::vector< GridItemPtr >::const_iterator g = room->gridItems[ column ].begin (); g != room->gridItems[ column ].end (); ++ g )
        {
                const GridItem& gridItem = *( *g );

                if ( ! gridItem.hasShadow() ) continue ;

        # if  defined( DEBUG_SHADOWS )  &&  DEBUG_SHADOWS
                std::cout << "casting shadow from " << gridItem.whichKindOfItem() << " \"" << gridItem.getUniqueName() << "\"" <<
                        " at x=" << gridItem.getX() << " y=" << gridItem.getY() << " z=" << gridItem.getZ() <<
                        " on floor tile at" <<
                        " x=" << tileSize << "*" << xCell << "=" << xCell * tileSize <<
                        " y=" << tileSize << "*" << yCell << "=" << yCell * tileSize
                          << std::endl ;
        # endif

                ShadowCaster::castShadowOnFloor (
                        floorTile,
                        /* x */ ( tileSize << 1 ) * ( xCell - yCell )
                                                - ( static_cast< int >( gridItem.getImageOfShadow().getWidth() ) >> 1 )
                                                + room->getX0() + 1,
                        /* y */ tileSize * ( xCell + yCell + 1 )
                                                - ( static_cast< int >( gridItem.getImageOfShadow().getHeight() ) >> 1 )
                                                + room->getY0() - 1,
                        /* shadow */ gridItem.getImageOfShadow(),
                        /* shading */ room->getOpacityOfShadows()
                        /* transparency = 0 */
                ) ;
        }

        // scroll through list of free items to shade floor tile
        for ( std::vector< FreeItemPtr >::const_iterator f = room->freeItems.begin (); f != room->freeItems.end (); ++ f )
        {
                const FreeItem& freeItem = *( *f );

                if ( ! freeItem.hasShadow() ) continue ;

                int xStart = freeItem.getX() / tileSize;
                int xEnd = ( freeItem.getX() + freeItem.getWidthX() - 1 ) / tileSize;
                int yStart = ( freeItem.getY() - freeItem.getWidthY() + 1 ) / tileSize;
                int yEnd = freeItem.getY() / tileSize;

                // shade with free item above
                if ( xCell >= xStart && xCell <= xEnd && yCell >= yStart && yCell <= yEnd )
                {
                # if  defined( DEBUG_SHADOWS )  &&  DEBUG_SHADOWS
                        std::cout << "casting shadow from " << freeItem.whichKindOfItem() << " \"" << freeItem.getUniqueName() << "\"" <<
                                " at x=" << freeItem.getX() << " y=" << freeItem.getY() << " z=" << freeItem.getZ() <<
                                " on floor tile at" <<
                                " x=" << tileSize << "*" << xCell << "=" << xCell * tileSize <<
                                " y=" << tileSize << "*" << yCell << "=" << yCell * tileSize
                                  << std::endl ;
                # endif

                        ShadowCaster::castShadowOnFloor (
                                floorTile,
                                /* x */ ( ( freeItem.getX() - freeItem.getY() ) << 1 ) + room->getX0()
                                                + static_cast< int >( freeItem.getWidthX() + freeItem.getWidthY() )
                                                - ( static_cast< int >( freeItem.getImageOfShadow().getWidth() ) >> 1 ) - 1,
                                /* y */ freeItem.getX() + freeItem.getY() + room->getY0()
                                                + ( ( static_cast< int >( freeItem.getWidthX() ) - static_cast< int >( freeItem.getWidthY() ) + 1 ) >> 1 )
                                                - ( static_cast< int >( freeItem.getImageOfShadow().getHeight() ) >> 1 ),
                                /* shadow */ freeItem.getImageOfShadow(),
                                /* shading */ room->getOpacityOfShadows(),
                                /* transparency */ freeItem.getTransparency()
                        ) ;
                }
        }
}

void Mediator::castShadowOnGridItem( GridItem& gridItem )
{
        if ( room->getOpacityOfShadows() >= 256 ) return ;

        int tileSize = room->getSizeOfOneTile();
        int column = gridItem.getColumnOfGrid();

        // shade with grid items it may have above
        for ( std::vector< GridItemPtr >::const_iterator g = room->gridItems[ column ].begin(); g != room->gridItems[ column ].end(); ++ g )
        {
                const GridItem& aboveItem = *( *g );

                if ( ! aboveItem.hasShadow() ) continue ;

                if ( aboveItem.getZ() > gridItem.getZ() )
                {
                # if  defined( DEBUG_SHADOWS )  &&  DEBUG_SHADOWS
                        std::ostringstream positionOfFrom;
                        positionOfFrom << "x=" << aboveItem.getX() << " y=" << aboveItem.getY() << " z=" << aboveItem.getZ() ;
                        std::ostringstream positionOfOn;
                        positionOfOn << "x=" << gridItem.getX() << " y=" << gridItem.getY() << " z=" << gridItem.getZ() ;

                        std::cout << "casting shadow from " << aboveItem.whichKindOfItem() << " \"" << aboveItem.getUniqueName() << "\"" << " at " << positionOfFrom.str() <<
                                        " on " << gridItem.whichKindOfItem() << " \"" << gridItem.getUniqueName() << "\"" << " at " << positionOfOn.str() << std::endl ;
                # endif

                        ShadowCaster::castShadowOnItem (
                                gridItem,
                                /* x */ gridItem.getOffsetX()
                                                + ( ( static_cast< int >( gridItem.getRawImage().getWidth() )
                                                        - static_cast< int >( aboveItem.getImageOfShadow().getWidth() ) ) >> 1 ),
                                /* y */ gridItem.getOffsetY()
                                                + gridItem.getRawImage().getHeight() - tileSize - gridItem.getHeight()
                                                - ( static_cast< int >( aboveItem.getImageOfShadow().getHeight() ) >> 1 ),
                                /* shadow */ aboveItem.getImageOfShadow(),
                                /* shading */ room->getOpacityOfShadows()
                                /* transparency = 0 */
                        ) ;
                }
        }

        // scroll thru list of free items to shade grid item
        for ( std::vector< FreeItemPtr >::const_iterator f = room->freeItems.begin(); f != room->freeItems.end(); ++ f )
        {
                const FreeItem& freeItem = *( *f );

                if ( ! freeItem.hasShadow() ) continue ;

                if ( freeItem.getZ() > gridItem.getZ() )
                {
                        // range of tiles where item is
                        int xStart = freeItem.getX() / tileSize;
                        int xEnd = ( freeItem.getX() + freeItem.getWidthX() - 1 ) / tileSize;
                        int yStart = ( freeItem.getY() - freeItem.getWidthY() + 1 ) / tileSize;
                        int yEnd = freeItem.getY() / tileSize;

                        // shade with free item above
                        if ( gridItem.getCellX() >= xStart && gridItem.getCellX() <= xEnd && gridItem.getCellY() >= yStart && gridItem.getCellY() <= yEnd )
                        {
                        # if  defined( DEBUG_SHADOWS )  &&  DEBUG_SHADOWS
                                std::ostringstream positionOfFrom;
                                positionOfFrom << "x=" << freeItem.getX() << " y=" << freeItem.getY() << " z=" << freeItem.getZ() ;
                                std::ostringstream positionOfOn;
                                positionOfOn << "x=" << gridItem.getX() << " y=" << gridItem.getY() << " z=" << gridItem.getZ() ;

                                std::cout << "casting shadow from " << freeItem.whichKindOfItem() << " \"" << freeItem.getUniqueName() << "\"" << " at " << positionOfFrom.str() <<
                                                " on " << gridItem.whichKindOfItem() << " \"" << gridItem.getUniqueName() << "\"" << " at " << positionOfOn.str() << std::endl ;
                        # endif

                                ShadowCaster::castShadowOnItem (
                                        gridItem,
                                        /* x */ ( ( freeItem.getX() - freeItem.getY() ) << 1 )
                                                        + static_cast< int >( freeItem.getWidthX() + freeItem.getWidthY() )
                                                        - ( static_cast< int >( freeItem.getImageOfShadow().getWidth() ) >> 1 ) - 1,
                                        /* y */ freeItem.getX() + freeItem.getY()
                                                        + ( ( static_cast< int >( freeItem.getWidthX() ) - static_cast< int >( freeItem.getWidthY() ) + 1 ) >> 1 )
                                                        - ( static_cast< int >( freeItem.getImageOfShadow().getHeight() ) >> 1 )
                                                        - gridItem.getZ() - gridItem.getHeight(),
                                        /* shadow */ freeItem.getImageOfShadow(),
                                        /* shading */ room->getOpacityOfShadows(),
                                        /* transparency */ freeItem.getTransparency()
                                ) ;
                        }
                }
        }
}

void Mediator::castShadowOnFreeItem( FreeItem& freeItem )
{
        if ( room->getOpacityOfShadows() >= 256 ) return ;

        int tileSize = room->getSizeOfOneTile();

        // range of columns met with item
        int xStart = freeItem.getX() / tileSize;
        int xEnd = ( freeItem.getX() + freeItem.getWidthX() - 1 ) / tileSize;
        int yStart = ( freeItem.getY() - freeItem.getWidthY() + 1 ) / tileSize;
        int yEnd = freeItem.getY() / tileSize;

        // shadow from grid items above
        for ( int yCell = yStart; yCell <= yEnd; yCell++ )
        {
                for ( int xCell = xStart; xCell <= xEnd; xCell++ )
                {
                        int column = yCell * room->getTilesX() + xCell;

                        for ( std::vector< GridItemPtr >::const_iterator g = room->gridItems[ column ].begin (); g != room->gridItems[ column ].end (); ++ g )
                        {
                                const GridItem& gridItem = *( *g ) ;

                                if ( ! gridItem.hasShadow() ) continue ;

                                if ( gridItem.getZ() > freeItem.getZ() )
                                {
                                # if  defined( DEBUG_SHADOWS )  &&  DEBUG_SHADOWS
                                        std::ostringstream positionOfFrom;
                                        positionOfFrom << "x=" << gridItem.getX() << " y=" << gridItem.getY() << " z=" << gridItem.getZ() ;
                                        std::ostringstream positionOfOn;
                                        positionOfOn << "x=" << freeItem.getX() << " y=" << freeItem.getY() << " z=" << freeItem.getZ() ;

                                        std::cout << "casting shadow from " << gridItem.whichKindOfItem() << " \"" << gridItem.getUniqueName() << "\"" << " at " << positionOfFrom.str() <<
                                                        " on " << freeItem.whichKindOfItem() << " \"" << freeItem.getUniqueName() << "\"" << " at " << positionOfOn.str() << std::endl ;
                                # endif

                                        ShadowCaster::castShadowOnItem (
                                                freeItem,
                                                /* x */ ( tileSize << 1 ) * ( xCell - yCell )
                                                                - ( static_cast< int >( gridItem.getImageOfShadow().getWidth() ) >> 1 ) + 1,
                                                /* y */ tileSize * ( xCell + yCell + 1 )
                                                                - freeItem.getZ() - freeItem.getHeight()
                                                                - ( static_cast< int >( gridItem.getImageOfShadow().getHeight() ) >> 1 ) - 1,
                                                /* shadow */ gridItem.getImageOfShadow(),
                                                /* shading */ room->getOpacityOfShadows()
                                                /* transparency = 0 */
                                        ) ;
                                }
                        }
                }
        }

        // scroll through list of free items
        for ( std::vector< FreeItemPtr >::const_iterator f = room->freeItems.begin (); f != room->freeItems.end (); ++ f )
        {
                const FreeItem& aboveItem = *( *f ) ;

                if ( ! aboveItem.hasShadow() ) continue ;

                if ( aboveItem.getUniqueName() != freeItem.getUniqueName() )
                {
                        // shadow with free item above
                        if ( freeItem.getZ() < aboveItem.getZ() &&
                                freeItem.getX() < aboveItem.getX() + static_cast< int >( aboveItem.getWidthX() ) &&
                                        aboveItem.getX() < freeItem.getX() + static_cast< int >( freeItem.getWidthX() ) &&
                                freeItem.getY() > aboveItem.getY() - static_cast< int >( aboveItem.getWidthY() ) &&
                                        aboveItem.getY() > freeItem.getY() - static_cast< int >( freeItem.getWidthY() ) )
                        {
                        # if  defined( DEBUG_SHADOWS )  &&  DEBUG_SHADOWS
                                std::ostringstream positionOfFrom;
                                positionOfFrom << "x=" << aboveItem.getX() << " y=" << aboveItem.getY() << " z=" << aboveItem.getZ() ;
                                std::ostringstream positionOfOn;
                                positionOfOn << "x=" << freeItem.getX() << " y=" << freeItem.getY() << " z=" << freeItem.getZ() ;

                                std::cout << "casting shadow from " << aboveItem.whichKindOfItem() << " \"" << aboveItem.getUniqueName() << "\"" << " at " << positionOfFrom.str() <<
                                        " on " << freeItem.whichKindOfItem() << " \"" << freeItem.getUniqueName() << "\"" << " at " << positionOfOn.str() << std::endl ;
                        # endif

                                ShadowCaster::castShadowOnItem (
                                        freeItem,
                                        /* x */ ( ( aboveItem.getX() - aboveItem.getY() ) << 1 )
                                                        + static_cast< int >( aboveItem.getWidthX() + aboveItem.getWidthY() )
                                                        - ( static_cast< int >( aboveItem.getImageOfShadow().getWidth() ) >> 1 ) - 1,
                                        /* y */ aboveItem.getX() + aboveItem.getY() - freeItem.getZ() - freeItem.getHeight()
                                                        + ( ( static_cast< int >( aboveItem.getWidthX() ) - static_cast< int >( aboveItem.getWidthY() )
                                                                - static_cast< int >( aboveItem.getImageOfShadow().getHeight() ) ) >> 1 ),
                                        /* shadow */ aboveItem.getImageOfShadow(),
                                        /* shading */ room->getOpacityOfShadows(),
                                        /* transparency */ freeItem.getTransparency()
                                );
                        }
                }
        }
}

void Mediator::maskFreeItem( FreeItem& freeItem )
{
        // mask only with next items
        // for sorted list there’s no masking with previous items

        // look for item in list
        std::vector< FreeItemPtr >::const_iterator fi = room->freeItems.begin ();
        while ( fi != room->freeItems.end () )
        {
                if ( freeItem.getUniqueName() == ( *fi )->getUniqueName() ) break ;
                ++ fi ;
        }
        if ( fi == room->freeItems.end () ) /* there’s no such item in list */ return ;

        while ( ++ fi != room->freeItems.end () ) // when there’s any next item
        {
                FreeItem& itemToMaskWith = *( *fi ) ;

                if ( freeItem.isPartOfDoor() && itemToMaskWith.isPartOfDoor() ) continue ;

                if ( /* one of two is marked to mask and other of two isn’t transparent */
                        ( ! freeItem.getWantMask().isFalse() && itemToMaskWith.getTransparency() == 0 ) ||
                        ( ! itemToMaskWith.getWantMask().isFalse() && freeItem.getTransparency() == 0 ) )
                {
                        if ( itemToMaskWith.doGraphicsOverlap( freeItem ) )
                        {
                                if ( itemToMaskWith.isBehind( freeItem ) )
                                {
                                # if  defined( DEBUG_MASKS )  &&  DEBUG_MASKS
                                        std::cout << "masking \"" << itemToMaskWith.getUniqueName() << "\" behind \"" << freeItem.getUniqueName() << "\"" << std::endl ;
                                # endif

                                        Masker::maskFreeItemBehindItem( itemToMaskWith, freeItem );
                                }
                                else
                                // freeItem is behind itemToMaskWith
                                {
                                # if  defined( DEBUG_MASKS )  &&  DEBUG_MASKS
                                        std::cout << "masking \"" << freeItem.getUniqueName() << "\"  behind \"" << itemToMaskWith.getUniqueName() << "\"" << std::endl ;
                                # endif

                                        Masker::maskFreeItemBehindItem( freeItem, itemToMaskWith );
                                }
                        }
                }
        }

        if ( ! freeItem.getWantMask().isFalse() )
        {
                int xStart = freeItem.getX() / room->getSizeOfOneTile() ;
                int xEnd = ( ( freeItem.getX() + freeItem.getWidthX() - 1 ) / room->getSizeOfOneTile() ) + 1 ;
                int yStart = ( freeItem.getY() / room->getSizeOfOneTile() ) + 1 ;
                int yEnd = ( freeItem.getY() - freeItem.getWidthY() + 1 ) / room ->getSizeOfOneTile() ;

                do {
                        unsigned int i = 0;

                        while ( ( xStart + i < room->getTilesX() ) && ( yStart + i < room->getTilesY() ) )
                        {
                                int column = room->getTilesX() * ( yStart + i ) + xStart + i;

                                // proceed with grid items
                                for ( std::vector< GridItemPtr >::const_iterator g = room->gridItems[ column ].begin (); g != room->gridItems[ column ].end (); ++ g )
                                {
                                        const GridItem& gridItem = *( *g ) ;

                                        if ( gridItem.doGraphicsOverlap( freeItem ) )
                                        {
                                                int gridItemX = static_cast< int >( ( xStart + i ) * room->getSizeOfOneTile() ) ;
                                                int gridItemYMinusWidthY = static_cast< int >( ( yStart + i ) * room->getSizeOfOneTile() ) - 1 ;

                                                if ( gridItemX >= ( freeItem.getX() + static_cast< int >( freeItem.getWidthX() ) ) ||
                                                        ( gridItemYMinusWidthY >= freeItem.getY() ) ||
                                                        ( gridItem.getZ() >= freeItem.getZ() + static_cast< int >( freeItem.getHeight() ) ) )
                                                // grid item is not behind free item
                                                {
                                                # if  defined( DEBUG_MASKS )  &&  DEBUG_MASKS
                                                        std::cout << "masking \"" << freeItem.getUniqueName() << "\" behind \"" << gridItem.getUniqueName() << "\"" << std::endl ;
                                                # endif

                                                        Masker::maskFreeItemBehindItem( freeItem, gridItem );
                                                }
                                        }
                                }

                                i++;
                        }

                        if ( yStart != yEnd )
                                yStart -- ;
                        else
                                xStart ++ ;
                }
                while ( xStart <= xEnd ) ;
        }
}

ItemPtr Mediator::findItemByUniqueName( const std::string & uniqueName )
{
        // first look for a free item
        for ( unsigned int f = 0 ; f < room->freeItems.size () ; ++ f )
        {
                FreeItemPtr item = room->freeItems[ f ];
                if ( item != nilPointer && item->getUniqueName() == uniqueName )
                {
                        return ItemPtr( item ) ;
                }
        }

        // then look for a grid item
        for ( unsigned int column = 0; column < room->gridItems.size(); ++ column ) // two-dimensional
        {
                for ( std::vector< GridItemPtr >::iterator g = room->gridItems[ column ].begin (); g != room->gridItems[ column ].end (); ++ g )
                {
                        if ( *g != nilPointer && ( *g )->getUniqueName() == uniqueName )
                        {
                                return ItemPtr( *g ) ;
                        }
                }
        }

        return ItemPtr() ;
}

ItemPtr Mediator::findItemByLabel( const std::string& label )
{
        // look for free item
        for ( std::vector< FreeItemPtr >::iterator f = room->freeItems.begin (); f != room->freeItems.end (); ++ f )
        {
                if ( *f != nilPointer && ( *f )->getLabel() == label )
                {
                        return ItemPtr( *f ) ;
                }
        }

        // look for grid item
        for ( unsigned int column = 0; column < room->gridItems.size(); ++ column )
        {
                for ( std::vector< GridItemPtr >::iterator g = room->gridItems[ column ].begin (); g != room->gridItems[ column ].end (); ++ g )
                {
                        if ( *g != nilPointer && ( *g )->getLabel() == label )
                        {
                                return ItemPtr( *g ) ;
                        }
                }
        }

        return ItemPtr() ;
}

ItemPtr Mediator::findItemByBehavior( const std::string& behavior )
{
        // look for free item
        for ( std::vector< FreeItemPtr >::iterator f = room->freeItems.begin (); f != room->freeItems.end (); ++ f )
        {
                if ( *f != nilPointer && ( *f )->getBehavior() != nilPointer && ( *f )->getBehavior()->getNameOfBehavior () == behavior )
                {
                        return ItemPtr( *f ) ;
                }
        }

        // look for grid item
        for ( unsigned int column = 0; column < room->gridItems.size(); ++ column )
        {
                for ( std::vector< GridItemPtr >::iterator g = room->gridItems[ column ].begin (); g != room->gridItems[ column ].end (); ++ g )
                {
                        if ( *g != nilPointer && ( *g )->getBehavior() != nilPointer && ( *g )->getBehavior()->getNameOfBehavior () == behavior )
                        {
                                return ItemPtr( *g ) ;
                        }
                }
        }

        return ItemPtr() ;
}

bool Mediator::lookForCollisionsOf( const std::string & uniqueNameOfItem )
{
        const ItemPtr item = findItemByUniqueName( uniqueNameOfItem ) ;
        if ( item == nilPointer ) return false ;
        if ( item->isIgnoringCollisions () ) return false ;

        bool collisionFound = false;

        // traverse the list of free items looking for collisions
        for ( std::vector< FreeItemPtr >::iterator f = room->freeItems.begin (); f != room->freeItems.end (); ++ f )
        {
                const FreeItem& freeItem = *( *f ) ;

                if ( freeItem.getUniqueName() != item->getUniqueName() && freeItem.isNotIgnoringCollisions () )
                {
                        if ( item->intersectsWith( freeItem ) )
                        {
                                collisions.push_back( freeItem.getUniqueName() );
                                collisionFound = true;
                        }
                }
        }

        // for a grid item
        if ( item->whichKindOfItem() == "grid item" )
        {
                int column = dynamic_cast< GridItem& >( *item ).getColumnOfGrid() ;

                // scroll through lists of grid items looking for collisions
                for ( std::vector< GridItemPtr >::iterator g = room->gridItems[ column ].begin ();
                                g != room->gridItems[ column ].end (); ++ g )
                {
                        const GridItem& gridItem = *( *g ) ;

                        if ( gridItem.getUniqueName() != item->getUniqueName() )
                        {
                                if ( item->intersectsWith( gridItem ) )
                                {
                                        collisions.push_back( gridItem.getUniqueName() );
                                        collisionFound = true;
                                }
                        }
                }
        }
        // for a free item
        else if ( item->whichKindOfItem() == "free item" || item->whichKindOfItem() == "avatar item" )
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

                // look for collisions in the range of columns where the item is
                for ( int i = xStart; i < xEnd; i++ )
                {
                        for ( int j = yStart; j < yEnd; j++ )
                        {
                                std::vector< GridItemPtr > items = room->gridItems[ room->getTilesX() * j + i ] ;

                                for ( std::vector< GridItemPtr >::const_iterator g = items.begin (); g != items.end (); ++ g )
                                {
                                        const GridItem& gridItem = *( *g ) ;

                                        if ( ( item->getZ() < gridItem.getZ() + static_cast< int >( gridItem.getHeight() ) ) &&
                                                ( gridItem.getZ() < item->getZ() + static_cast< int >( item->getHeight() ) ) )
                                        {
                                                collisions.push_back( gridItem.getUniqueName() );
                                                collisionFound = true;
                                        }
                                }
                        }
                }
        }

        return collisionFound;
}

int Mediator::findHighestZ( const Item& item )
{
        int z = 0 ;

        // go thru the list of free items looking for the highest in column
        for ( std::vector< FreeItemPtr >::const_iterator f = room->freeItems.begin ();
                        f != room->freeItems.end (); ++ f )
        {
                const FreeItem & freeItem = *( *f ) ;

                if ( freeItem.getUniqueName() != item.getUniqueName() && freeItem.isNotIgnoringCollisions () )
                {
                        // look for intersection on X and Y with higher Z
                        if ( ( freeItem.getX() + static_cast< int >( freeItem.getWidthX() ) > item.getX() ) &&
                                        ( freeItem.getX() < item.getX() + static_cast< int >( item.getWidthX() ) ) &&
                                ( freeItem.getY() > item.getY() - static_cast< int >( item.getWidthX() ) ) &&
                                        ( freeItem.getY() - static_cast< int >( freeItem.getWidthY() ) < item.getY() ) &&
                                ( freeItem.getZ() + static_cast< int >( freeItem.getHeight() ) > item.getZ() ) )
                        {
                                z = freeItem.getZ() + static_cast< int >( freeItem.getHeight() );
                        }
                }
        }

        if ( item.whichKindOfItem() == "grid item" )
        {
                int column = dynamic_cast< const GridItem& >( item ).getColumnOfGrid ();

                // look for highest Z in the column
                for ( std::vector< GridItemPtr >::const_iterator g = room->gridItems[ column ].begin ();
                                g != room->gridItems[ column ].end (); ++ g )
                {
                        const GridItem & gridItem = *( *g ) ;

                        if ( gridItem.getZ() + static_cast< int >( gridItem.getHeight() ) > z )
                        {
                                z = gridItem.getZ() + gridItem.getHeight();
                        }
                }
        }
        else if ( item.whichKindOfItem() == "free item" || item.whichKindOfItem() == "avatar item" )
        {
                const FreeItem& freeItem = dynamic_cast< const FreeItem& >( item );

                // get columns where item is
                int xStart = freeItem.getX() / room->getSizeOfOneTile();
                int xEnd = ( freeItem.getX() + freeItem.getWidthX() - 1 ) / room->getSizeOfOneTile() + 1;
                int yStart = ( freeItem.getY() - freeItem.getWidthY() + 1 ) / room->getSizeOfOneTile();
                int yEnd = freeItem.getY() / room->getSizeOfOneTile() + 1;

                // walk thru items in columns where item is
                for ( int i = xStart; i < xEnd; i++ )
                {
                        for ( int j = yStart; j < yEnd; j++ )
                        {
                                int column = room->getTilesX() * j + i ;
                                for ( std::vector< GridItemPtr >::const_iterator g = room->gridItems[ column ].begin ();
                                                g != room->gridItems[ column ].end (); ++ g )
                                {
                                        const GridItem& gridItem = *( *g ) ;

                                        if ( gridItem.getZ() + static_cast< int >( gridItem.getHeight() ) > z )
                                        {
                                                z = gridItem.getZ() + gridItem.getHeight();
                                        }
                                }
                        }
                }
        }

        return z;
}

void Mediator::pushCollision( const std::string& name )
{
# if defined( DEBUG_COLLISIONS ) && DEBUG_COLLISIONS
        std::cout << ")( pushing collision with \"" << name << "\"" << std::endl ;
# endif

        lockCollisionsMutex ();
        collisions.push_front( name );
        unlockCollisionsMutex ();
}

std::string Mediator::popCollision()
{
        std::string name ;

        lockCollisionsMutex ();

        if ( ! collisions.empty() )
        {
                name = collisions.back ();
                collisions.pop_back( );
        }

        unlockCollisionsMutex ();

# if defined( DEBUG_COLLISIONS ) && DEBUG_COLLISIONS
        std::cout << ") ( popped collision with \"" << name << "\"" << std::endl ;
# endif

        return name ;
}

ItemPtr Mediator::collisionWithByLabel( const std::string& label )
{
        for ( unsigned int i = 0; i < collisions.size(); i++ )
        {
                ItemPtr item = findItemByUniqueName( collisions[ i ] );

                if ( item != nilPointer && item->getLabel() == label )
                {
                        return item;
                }
        }

        return ItemPtr ();
}

ItemPtr Mediator::collisionWithByBehavior( const std::string& behavior )
{
        for ( unsigned int i = 0; i < collisions.size(); i++ )
        {
                ItemPtr item = findItemByUniqueName( collisions[ i ] );

                if ( item != nilPointer && item->getBehavior() != nilPointer &&
                        item->getBehavior()->getNameOfBehavior () == behavior )
                {
                        return item;
                }
        }

        return ItemPtr ();
}

ItemPtr Mediator::collisionWithBadBoy()
{
        for ( unsigned int i = 0; i < collisions.size(); i++ )
        {
                ItemPtr item = findItemByUniqueName( collisions[ i ] );

                if ( item != nilPointer && item->getBehavior() != nilPointer && item->isMortal()
                        && std::find( badBoys.begin(), badBoys.end(), item->getBehavior()->getNameOfBehavior () ) != badBoys.end() )
                {
                        return item;
                }
        }

        return ItemPtr ();
}

void Mediator::setActiveCharacter ( const AvatarItemPtr& character )
{
        if ( this->activeCharacter != character )
        {
                this->activeCharacter = character ;

                if ( character != nilPointer )
                        this->labelOfActiveCharacter = character->getOriginalLabel ();
                else
                        this->labelOfActiveCharacter.clear();

                std::cout << "character \"" << labelOfActiveCharacter << "\" is yet active in room \"" << room->getNameOfRoomDescriptionFile() << "\"" << std::endl ;
        }
}

bool Mediator::pickNextCharacter ()
{
        AvatarItemPtr previousCharacter = activeCharacter;

        // search for next character
        std::vector< AvatarItemPtr > charactersInRoom = room->getCharactersYetInRoom() ;
        std::vector< AvatarItemPtr >::iterator i = charactersInRoom.begin () ;
        while ( i != charactersInRoom.end () )
        {
                if ( *i != nilPointer && ( *i )->getOriginalLabel() == activeCharacter->getOriginalLabel() ) break;
                ++ i ;
        }
        ++ i;
        setActiveCharacter( i != charactersInRoom.end () ? *i : *charactersInRoom.begin () );

        // see if characters may join here
        if ( previousCharacter->getOriginalLabel() != activeCharacter->getOriginalLabel() )
        {
                const int delta = room->getSizeOfOneTile() >> 1;

                if ( ( previousCharacter->getOriginalLabel() == "head" && activeCharacter->getOriginalLabel() == "heels" ) ||
                                ( previousCharacter->getOriginalLabel() == "heels" && activeCharacter->getOriginalLabel() == "head" ) )
                {
                        if ( ( previousCharacter->getX() + delta >= activeCharacter->getX() )
                                && ( previousCharacter->getX() + previousCharacter->getWidthX() - delta <= activeCharacter->getX() + activeCharacter->getWidthX() )
                                && ( previousCharacter->getY() + delta >= activeCharacter->getY() )
                                && ( previousCharacter->getY() + previousCharacter->getWidthY() - delta <= activeCharacter->getY() + activeCharacter->getWidthY() )
                                && ( ( previousCharacter->getOriginalLabel() == "head" && previousCharacter->getZ() - Isomot::LayerHeight == activeCharacter->getZ() ) ||
                                        ( previousCharacter->getOriginalLabel() == "heels" && activeCharacter->getZ() - Isomot::LayerHeight == previousCharacter->getZ() ) ) )
                        {
                                lockFreeItemsMutex ();

                                AvatarItemPtr reference = previousCharacter->getOriginalLabel() == "heels" ? previousCharacter : activeCharacter;
                                this->lastActiveCharacterBeforeJoining = previousCharacter->getOriginalLabel() == "heels" ? "heels" : "head";

                                int x = reference->getX();
                                int y = reference->getY();
                                int z = reference->getZ();
                                std::string orientation = reference->getOrientation() ;

                                // item that Heels may have in handbag
                                AvatarItemPtr heels = reference;
                                const DescriptionOfItem* descriptionOfItemInBag = heels->getDescriptionOfTakenItem ();
                                std::string behaviorOfItemInBag = heels->getBehaviorOfTakenItem( );

                                // remove simple characters
                                this->room->removeCharacterFromRoom( *previousCharacter, false );
                                this->room->removeCharacterFromRoom( *activeCharacter, false );

                                // create composite character
                                setActiveCharacter( RoomBuilder::createCharacterInRoom( this->room, "headoverheels", false, x, y, z, orientation ) );

                                // transfer item in handbag
                                if ( descriptionOfItemInBag != nilPointer )
                                {
                                        std::cout << "transfer item \"" << descriptionOfItemInBag->getLabel() << "\" to character \"" << activeCharacter->getLabel() << "\"" << std::endl ;
                                        activeCharacter->placeItemInBag( descriptionOfItemInBag->getLabel(), behaviorOfItemInBag );
                                }

                                unlockFreeItemsMutex ();

                                std::cout << "join both characters into \"" << activeCharacter->getOriginalLabel() << "\""
                                                << " in room " << room->getNameOfRoomDescriptionFile() << std::endl ;
                                return true;
                        }
                }
        }
        // is it composite character
        else if ( activeCharacter->getOriginalLabel() == "headoverheels" )
        {
                std::cout << "split \"" << activeCharacter->getOriginalLabel() << "\" in room " << room->getNameOfRoomDescriptionFile() << std::endl ;

                int x = activeCharacter->getX();
                int y = activeCharacter->getY();
                int z = activeCharacter->getZ();
                std::string orientation = activeCharacter->getOrientation() ;

                lockFreeItemsMutex ();

                // get the description of item in the handbag
                const DescriptionOfItem* descriptionOfItemInBag = activeCharacter->getDescriptionOfTakenItem ();
                std::string behaviorOfItemInBag = activeCharacter->getBehaviorOfTakenItem( );

                // remove the composite character
                this->room->removeCharacterFromRoom( *activeCharacter, false );

                // create the simple characters

                AvatarItemPtr characterHeels = RoomBuilder::createCharacterInRoom( this->room, "heels", false, x, y, z, orientation );

                if ( descriptionOfItemInBag != nilPointer )
                {
                        std::cout << "transfer item \"" << descriptionOfItemInBag->getLabel() << "\" to character \"" << characterHeels->getLabel() << "\"" << std::endl ;
                        characterHeels->placeItemInBag( descriptionOfItemInBag->getLabel(), behaviorOfItemInBag );
                }

                AvatarItemPtr characterHead = RoomBuilder::createCharacterInRoom( this->room, "head", false, x, y, z + Isomot::LayerHeight, orientation );

                setActiveCharacter( ( this->lastActiveCharacterBeforeJoining == "head" ) ? characterHeels : characterHead );
                previousCharacter = ( activeCharacter->getOriginalLabel() == "head" ) ? characterHeels : characterHead;

                unlockFreeItemsMutex ();
        }

        if ( previousCharacter->getOriginalLabel() == activeCharacter->getOriginalLabel() )
        {
                return false;
        }

        std::cout << "swop character \"" << previousCharacter->getOriginalLabel() << "\" to character \"" << activeCharacter->getOriginalLabel() << "\""
                        << " in room " << room->getNameOfRoomDescriptionFile() << std::endl ;
        return true;
}

void Mediator::toggleSwitchInRoom ()
{
        this->switchInRoomIsOn = ! this->switchInRoomIsOn ;

        // look for volatile and mortal free items to freeze them
        for ( std::vector< FreeItemPtr >::const_iterator f = room->freeItems.begin ();
                        f != room->freeItems.end (); ++ f )
        {
                const FreeItem& freeItem = *( *f ) ;

                if ( freeItem.getBehavior() != nilPointer )
                {
                        std::string behavior = freeItem.getBehavior()->getNameOfBehavior ();

                        if ( behavior == "behavior of disappearance on touch" || behavior == "behavior of disappearance on jump into" ||
                                        std::find( badBoys.begin (), badBoys.end (), behavior ) != badBoys.end () )
                        {
                                freeItem.getBehavior()->changeActivityOfItem(
                                        this->switchInRoomIsOn ?
                                                Activity::Freeze :
                                                Activity::WakeUp );
                        }
                }
        }

        // look for volatile grid items to freeze them
        for ( unsigned int column = 0; column < room->gridItems.size(); ++ column )
        {
                for ( std::vector< GridItemPtr >::const_iterator g = room->gridItems[ column ].begin ();
                                g != room->gridItems[ column ].end (); ++ g )
                {
                        const GridItem& gridItem = *( *g ) ;

                        if ( gridItem.getBehavior() != nilPointer )
                        {
                                std::string behavior = gridItem.getBehavior()->getNameOfBehavior ();

                                if ( behavior == "behavior of disappearance on touch" || behavior == "behavior of disappearance on jump into" )
                                {
                                        gridItem.getBehavior()->changeActivityOfItem(
                                                this->switchInRoomIsOn ?
                                                        Activity::Freeze :
                                                        Activity::Wait );
                                }
                        }
                }
        }

        std::cout << "toggled switch in room " << getRoom()->getNameOfRoomDescriptionFile() << std::endl ;
}

AvatarItemPtr Mediator::getWaitingCharacter() const
{
        std::vector< AvatarItemPtr > charactersInRoom = room->getCharactersYetInRoom() ;

        for ( std::vector< AvatarItemPtr >::iterator p = charactersInRoom.begin (); p != charactersInRoom.end (); ++p )
        {
                if ( ( *p )->getUniqueName() != activeCharacter->getUniqueName() )
                {
                        return *p ;
                }
        }

        return AvatarItemPtr ();
}

}

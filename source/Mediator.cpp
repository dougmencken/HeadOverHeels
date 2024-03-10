
#include "Mediator.hpp"

#include "RoomBuilder.hpp"
#include "GameManager.hpp"
#include "DescriptionOfItem.hpp"
#include "FloorTile.hpp"
#include "ShadowCaster.hpp"
#include "Masker.hpp"
#include "Behavior.hpp"

#include "sleep.hpp"

#ifdef DEBUG
#  define DEBUG_COLLISIONS      0
#  define DEBUG_SHADOWS         0
#  define DEBUG_MASKS           0
#endif


Mediator::Mediator( Room* whichRoom )
        : room( whichRoom )
        , threadRunning( false )
        , needToSortGridItems( false )
        , needToSortFreeItems( false )
        , switchInRoomIsOn( false )
        , currentlyActiveCharacter( nilPointer )
{
        pthread_mutex_init( &gridItemsMutex, nilPointer );
        pthread_mutex_init( &freeItemsMutex, nilPointer );
        pthread_mutex_init( &collisionsMutex, nilPointer );

        // these can be frozen with a doughnut or switch
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
        for ( unsigned int column = 0; column < room->gridItems.size(); ++ column ) {
                for ( std::vector< GridItemPtr >::iterator g = room->gridItems[ column ].begin (); g != room->gridItems[ column ].end (); ++ g )
                {
                        if ( ! ( *g )->updateItem() )
                                vanishedGridItems.push_back( ( *g )->getUniqueName() );
                }
        }

        for ( std::vector< std::string >::const_iterator i = vanishedGridItems.begin () ; i != vanishedGridItems.end () ; ++ i )
        {
                std::cout << "grid item \"" << *i << "\" is to be gone" << std::endl ;
                room->removeGridItemByUniqueName( *i );
        }

        unlockGridItemsMutex ();

        if ( this->needToSortGridItems ) {
                this->room->sortGridItems() ;
                this->needToSortGridItems = false ;
        }

        // let the player play
        if ( this->currentlyActiveCharacter != nilPointer )
                this->currentlyActiveCharacter->behaveCharacter ();

        lockFreeItemsMutex ();

        // free items to remove after update
        std::vector< std::string > vanishedFreeItems ;

        // update free items
        for ( std::vector< FreeItemPtr >::iterator f = room->freeItems.begin (); f != room->freeItems.end (); ++ f )
        {
                if ( ! ( *f )->updateItem() )
                        vanishedFreeItems.push_back( ( *f )->getUniqueName() );
        }

        for ( std::vector< std::string >::const_iterator i = vanishedFreeItems.begin () ; i != vanishedFreeItems.end () ; ++ i )
        {
                std::cout << "free item \"" << *i << "\" disappeared at the last update" << std::endl ;
                room->removeFreeItemByUniqueName( *i );
        }

        unlockFreeItemsMutex ();

        if ( this->needToSortFreeItems ) {
                this->room->sortFreeItems() ;
                this->needToSortFreeItems = false ;
        }

        ///// here??
        std::vector< AvatarItemPtr > charactersInRoom = room->getCharactersYetInRoom () ;
        for ( unsigned int i = 0 ; i < charactersInRoom.size () ; ++ i )
        {
                const AvatarItem & character = *( charactersInRoom[ i ] );

                // when the inactive character falls down to the room below
                // then make it active to let it fall
                if ( character.getWayOfExit() == "below" && ! character.isActiveCharacter() )
                {
                        std::cout << "inactive character \"" << character.getKind () << "\" falls down to another room, swap characters to make it active" << std::endl ;
                        this->currentlyActiveCharacter->setWayOfExit( "" );
                        this->pickNextCharacter () ;
                }
        }
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

void Mediator::wantToMaskWithFreeItem( const FreeItem & item )
{
        wantToMaskWithFreeItemImageAt( item, item.getImageOffsetX (), item.getImageOffsetY () );
}

void Mediator::wantToMaskWithFreeItemImageAt( const FreeItem & item, int x, int y )
{
        for ( std::vector< FreeItemPtr >::iterator f = room->freeItems.begin (); f != room->freeItems.end (); ++ f )
        {
                FreeItem& thatFreeItem = *( *f );

                if ( thatFreeItem.getUniqueName() != item.getUniqueName() &&
                        thatFreeItem.getUniqueName() + " copy" != item.getUniqueName() )
                {
                        if ( item.doGraphicsOverlapAt( thatFreeItem, x, y ) )
                        {
                                thatFreeItem.setWantMaskTrue();
                        }
                }
        }
}

void Mediator::wantToMaskWithGridItem( const GridItem & gridItem )
{
        wantToMaskWithGridItemAt( gridItem,
                                        gridItem.getX (), gridItem.getY (), gridItem.getZ (),
                                                std::pair< int, int >( gridItem.getImageOffsetX (), gridItem.getImageOffsetY () ) );
}

void Mediator::wantToMaskWithGridItemAt( const GridItem & gridItem, int x, int y, int z, std::pair< int, int > offset )
{
        for ( std::vector< FreeItemPtr >::iterator f = room->freeItems.begin (); f != room->freeItems.end (); ++ f )
        {
                FreeItem& freeItem = *( *f );

                if ( gridItem.doGraphicsOverlapAt( freeItem, offset.first, offset.second ) )
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
        if ( room->getTransparencyOfShadows() >= 256 ) return ;

        shadeFreeItemsBeneathItemAt( item, item.getX (), item.getY (), item.getZ () );

        unsigned int column = item.getColumnOfGrid ();

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

        // shade the floor in this column, if any
        FloorTile * floorTile = room->getFloorTileAtColumn( column );
        if ( floorTile != nilPointer ) {
                floorTile->freshShadyImage(); // begin shading with the fresh image of tile
                floorTile->setWantShadow( true );
        }
}

void Mediator::wantShadowFromFreeItem( const FreeItem& item )
{
        wantShadowFromFreeItemAt( item, item.getX (), item.getY (), item.getZ () );
}

void Mediator::wantShadowFromFreeItemAt( const FreeItem& item, int x, int y, int z )
{
        if ( room->getTransparencyOfShadows() >= 256 ) return ;

        shadeFreeItemsBeneathItemAt( item, x, y, z );

        // the range of tiles (columns, cells) where this item is
        int xStart = x / room->getSizeOfOneTile ();
        int xEnd = ( x + item.getWidthX() - 1 ) / room->getSizeOfOneTile () + 1 ;
        int yStart = ( y - item.getWidthY() + 1 ) / room->getSizeOfOneTile ();
        int yEnd = y / room->getSizeOfOneTile () + 1 ;

        for ( int i = xStart; i < xEnd; ++ i ) {
                for ( int j = yStart; j < yEnd; ++ j )
                {
                        unsigned int column = room->getTilesX() * j + i ;

                        // mark to shade grid items in the column
                        for ( std::vector< GridItemPtr >::iterator g = room->gridItems[ column ]. begin ();
                                                                   g != room->gridItems[ column ]. end ();
                                                                ++ g ) {
                                GridItem & gridItem = *( *g );

                                if ( z > gridItem.getZ () ) // below the free item
                                {
                                        gridItem.freshProcessedImage();
                                        gridItem.setWantShadow( true );
                                }
                        }

                        // shade the floor in this column, if any
                        FloorTile * floorTile = room->getFloorTileAtColumn( column );
                        if ( floorTile != nilPointer ) {
                                floorTile->freshShadyImage (); // begin shading with the fresh image of tile
                                floorTile->setWantShadow( true );
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
                        int freeXend = freeX + freeItem.getWidthX() ;
                        int freeYend = freeY - freeItem.getWidthY() ;
                        int itemXend = itemX + item.getWidthX() ;
                        int itemYend = itemY - item.getWidthY() ;

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
        if ( room->getTransparencyOfShadows() >= 256 ) return ;

        int xCell = floorTile.getCellX ();
        int yCell = floorTile.getCellY ();
        unsigned int column = floorTile.getIndexOfColumn ();
        unsigned int tileSize = room->getSizeOfOneTile ();

        // shade with every grid item above
        for ( std::vector< GridItemPtr >::const_iterator g = room->gridItems[ column ].begin (); g != room->gridItems[ column ].end (); ++ g )
        {
                const GridItem& gridItem = *( *g );

                if ( ! gridItem.hasShadow() ) continue ;

        # if  defined( DEBUG_SHADOWS )  &&  DEBUG_SHADOWS
                std::cout << "casting shadow from " << gridItem.whichItemClass() << " \"" << gridItem.getUniqueName() << "\"" <<
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
                        /* shading */ room->getTransparencyOfShadows()
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
                        std::cout << "casting shadow from " << freeItem.whichItemClass() << " \"" << freeItem.getUniqueName() << "\"" <<
                                " at x=" << freeItem.getX() << " y=" << freeItem.getY() << " z=" << freeItem.getZ() <<
                                " on floor tile at" <<
                                " x=" << tileSize << "*" << xCell << "=" << xCell * tileSize <<
                                " y=" << tileSize << "*" << yCell << "=" << yCell * tileSize
                                  << std::endl ;
                # endif

                        ShadowCaster::castShadowOnFloor (
                                floorTile,
                                /* x */ ( ( freeItem.getX() - freeItem.getY() ) << 1 ) + room->getX0()
                                                + freeItem.getWidthX() + freeItem.getWidthY()
                                                - ( static_cast< int >( freeItem.getImageOfShadow().getWidth() ) >> 1 ) - 1,
                                /* y */ freeItem.getX() + freeItem.getY() + room->getY0()
                                                + ( ( freeItem.getWidthX() - freeItem.getWidthY() + 1 ) >> 1 )
                                                - ( static_cast< int >( freeItem.getImageOfShadow().getHeight() ) >> 1 ),
                                /* shadow */ freeItem.getImageOfShadow(),
                                /* shading */ room->getTransparencyOfShadows(),
                                /* transparency */ freeItem.getTransparency()
                        ) ;
                }
        }
}

void Mediator::castShadowOnGridItem( GridItem& gridItem )
{
        if ( room->getTransparencyOfShadows() >= 256 ) return ;

        int tileSize = room->getSizeOfOneTile ();
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

                        std::cout << "casting shadow from " << aboveItem.whichItemClass() << " \"" << aboveItem.getUniqueName() << "\"" << " at " << positionOfFrom.str() <<
                                        " on " << gridItem.whichItemClass() << " \"" << gridItem.getUniqueName() << "\"" << " at " << positionOfOn.str() << std::endl ;
                # endif

                        int  widthOfItemImage = gridItem.getRawImage().getWidth() ;
                        int heightOfItemImage = gridItem.getRawImage().getHeight() ;

                        int  widthOfAboveShadow = aboveItem.getImageOfShadow().getWidth() ;
                        int heightOfAboveShadow = aboveItem.getImageOfShadow().getHeight() ;

                        ShadowCaster::castShadowOnItem (
                                gridItem,
                                /* x */ gridItem.getImageOffsetX()
                                                + ( ( widthOfItemImage - widthOfAboveShadow ) >> 1 ),
                                /* y */ gridItem.getImageOffsetY()
                                                + heightOfItemImage - tileSize - gridItem.getHeight()
                                                - ( heightOfAboveShadow >> 1 ),
                                /* shadow */ aboveItem.getImageOfShadow(),
                                /* shading */ room->getTransparencyOfShadows()
                                /* transparency = 0 */
                        ) ;
                }
        }

        // scroll thru the list of free items to shade the grid item
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

                                std::cout << "casting shadow from " << freeItem.whichItemClass() << " \"" << freeItem.getUniqueName() << "\"" << " at " << positionOfFrom.str() <<
                                                " on " << gridItem.whichItemClass() << " \"" << gridItem.getUniqueName() << "\"" << " at " << positionOfOn.str() << std::endl ;
                        # endif

                                int  widthOfAboveShadow = freeItem.getImageOfShadow().getWidth() ;
                                int heightOfAboveShadow = freeItem.getImageOfShadow().getHeight() ;

                                ShadowCaster::castShadowOnItem (
                                        gridItem,
                                        /* x */ ( ( freeItem.getX() - freeItem.getY() ) << 1 )
                                                        + freeItem.getWidthX() + freeItem.getWidthY()
                                                        - ( widthOfAboveShadow >> 1 ) - 1,
                                        /* y */ freeItem.getX() + freeItem.getY()
                                                        + ( ( freeItem.getWidthX() - freeItem.getWidthY() + 1 ) >> 1 )
                                                        - ( heightOfAboveShadow >> 1 )
                                                        - gridItem.getZ() - gridItem.getHeight(),
                                        /* shadow */ freeItem.getImageOfShadow(),
                                        /* shading */ room->getTransparencyOfShadows(),
                                        /* transparency */ freeItem.getTransparency()
                                ) ;
                        }
                }
        }
}

void Mediator::castShadowOnFreeItem( FreeItem& freeItem )
{
        if ( room->getTransparencyOfShadows() >= 256 ) return ;

        int tileSize = room->getSizeOfOneTile ();

        // the range of cells where this item is
        int xStart = freeItem.getX() / tileSize;
        int xEnd = ( freeItem.getX() + freeItem.getWidthX() - 1 ) / tileSize;
        int yStart = ( freeItem.getY() - freeItem.getWidthY() + 1 ) / tileSize;
        int yEnd = freeItem.getY() / tileSize;

        // cast shadow from the grid items above
        for ( int yCell = yStart; yCell <= yEnd; yCell++ ) {
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

                                        std::cout << "casting shadow from " << gridItem.whichItemClass() << " \"" << gridItem.getUniqueName() << "\"" << " at " << positionOfFrom.str() <<
                                                        " on " << freeItem.whichItemClass() << " \"" << freeItem.getUniqueName() << "\"" << " at " << positionOfOn.str() << std::endl ;
                                # endif

                                        int  widthOfAboveShadow = gridItem.getImageOfShadow().getWidth() ;
                                        int heightOfAboveShadow = gridItem.getImageOfShadow().getHeight() ;

                                        ShadowCaster::castShadowOnItem (
                                                freeItem,
                                                /* x */ ( tileSize << 1 ) * ( xCell - yCell )
                                                                - ( widthOfAboveShadow >> 1 ) + 1,
                                                /* y */ tileSize * ( xCell + yCell + 1 )
                                                                - freeItem.getZ() - freeItem.getHeight()
                                                                - ( heightOfAboveShadow >> 1 ) - 1,
                                                /* shadow */ gridItem.getImageOfShadow(),
                                                /* shading */ room->getTransparencyOfShadows()
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
                        // shadow from a free item above
                        if ( freeItem.getZ() < aboveItem.getZ()
                                && freeItem.getX() < aboveItem.getX() + aboveItem.getWidthX()
                                        && aboveItem.getX() < freeItem.getX() + freeItem.getWidthX()
                                && freeItem.getY() > aboveItem.getY() - aboveItem.getWidthY()
                                        && aboveItem.getY() > freeItem.getY() - freeItem.getWidthY() )
                        {
                        # if  defined( DEBUG_SHADOWS )  &&  DEBUG_SHADOWS
                                std::ostringstream positionOfFrom;
                                positionOfFrom << "x=" << aboveItem.getX() << " y=" << aboveItem.getY() << " z=" << aboveItem.getZ() ;
                                std::ostringstream positionOfOn;
                                positionOfOn << "x=" << freeItem.getX() << " y=" << freeItem.getY() << " z=" << freeItem.getZ() ;

                                std::cout << "casting shadow from " << aboveItem.whichItemClass() << " \"" << aboveItem.getUniqueName() << "\"" << " at " << positionOfFrom.str() <<
                                        " on " << freeItem.whichItemClass() << " \"" << freeItem.getUniqueName() << "\"" << " at " << positionOfOn.str() << std::endl ;
                        # endif

                                int  widthOfAboveShadow = aboveItem.getImageOfShadow().getWidth() ;
                                int heightOfAboveShadow = aboveItem.getImageOfShadow().getHeight() ;

                                ShadowCaster::castShadowOnItem (
                                        freeItem,
                                        /* x */ ( ( aboveItem.getX() - aboveItem.getY() ) << 1 )
                                                        + aboveItem.getWidthX() + aboveItem.getWidthY()
                                                                - ( widthOfAboveShadow >> 1 ) - 1,
                                        /* y */ aboveItem.getX() + aboveItem.getY() - freeItem.getZ() - freeItem.getHeight()
                                                        + ( ( aboveItem.getWidthX() - aboveItem.getWidthY()
                                                                - heightOfAboveShadow ) >> 1 ),
                                        /* shadow */ aboveItem.getImageOfShadow(),
                                        /* shading */ room->getTransparencyOfShadows(),
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

                if ( /* one of the two is marked to mask and the other isn’t transparent */
                        ( ! freeItem.getWantMask().isFalse() && itemToMaskWith.getTransparency() == 0 )
                                || ( ! itemToMaskWith.getWantMask().isFalse() && freeItem.getTransparency() == 0 ) )
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
                int xEnd = 1 + ( freeItem.getX() + freeItem.getWidthX() - 1 ) / room->getSizeOfOneTile() ;
                int yStart = 1 + freeItem.getY() / room->getSizeOfOneTile ();
                int yEnd = ( freeItem.getY() - freeItem.getWidthY() + 1 ) / room->getSizeOfOneTile() ;

                do {
                        unsigned int i = 0;

                        while ( ( xStart + i < room->getTilesX() ) && ( yStart + i < room->getTilesY() ) )
                        {
                                int column = room->getTilesX() * ( yStart + i ) + xStart + i;

                                // behind grid items
                                for ( std::vector< GridItemPtr >::const_iterator g = room->gridItems[ column ].begin (); g != room->gridItems[ column ].end (); ++ g )
                                {
                                        const GridItem& gridItem = *( *g ) ;

                                        if ( gridItem.doGraphicsOverlap( freeItem ) )
                                        {
                                                int gridItemX = static_cast< int >( ( xStart + i ) * room->getSizeOfOneTile() ) ;
                                                int gridItemYMinusWidthY = static_cast< int >( ( yStart + i ) * room->getSizeOfOneTile() ) - 1 ;

                                                if ( gridItemX >= ( freeItem.getX() + freeItem.getWidthX() ) ||
                                                        ( gridItemYMinusWidthY >= freeItem.getY() ) ||
                                                        ( gridItem.getZ() >= freeItem.getZ() + freeItem.getHeight() ) )
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

ItemPtr Mediator::findItemOfKind( const std::string & kind )
{
        // look for a free item
        for ( std::vector< FreeItemPtr >::iterator f = room->freeItems.begin (); f != room->freeItems.end (); ++ f )
        {
                if ( *f != nilPointer && ( *f )->getKind () == kind )
                {
                        return ItemPtr( *f ) ;
                }
        }

        // look for a grid item
        for ( unsigned int column = 0; column < room->gridItems.size(); ++ column )
        {
                for ( std::vector< GridItemPtr >::iterator g = room->gridItems[ column ].begin (); g != room->gridItems[ column ].end (); ++ g )
                {
                        if ( *g != nilPointer && ( *g )->getKind () == kind )
                        {
                                return ItemPtr( *g ) ;
                        }
                }
        }

        return ItemPtr() ;
}

ItemPtr Mediator::findItemBehavingAs( const std::string & behavior )
{
        // look for a free item
        for ( std::vector< FreeItemPtr >::iterator f = room->freeItems.begin (); f != room->freeItems.end (); ++ f )
        {
                if ( *f != nilPointer && ( *f )->getBehavior() != nilPointer && ( *f )->getBehavior()->getNameOfBehavior () == behavior )
                {
                        return ItemPtr( *f ) ;
                }
        }

        // look for a grid item
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

bool Mediator::collectCollisionsWith( const std::string & uniqueNameOfItem )
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
                        if ( item->crossesWith( freeItem ) )
                        {
                                collisions.push_back( freeItem.getUniqueName() );
                                collisionFound = true;
                        }
                }
        }

        // for a grid item
        if ( item->whichItemClass() == "grid item" )
        {
                int column = dynamic_cast< GridItem& >( *item ).getColumnOfGrid() ;

                // scroll through the lists of grid items looking for collisions
                for ( std::vector< GridItemPtr >::iterator g = room->gridItems[ column ].begin ();
                                g != room->gridItems[ column ].end (); ++ g )
                {
                        const GridItem& gridItem = *( *g ) ;

                        if ( gridItem.getUniqueName() != item->getUniqueName() )
                        {
                                if ( item->crossesWith( gridItem ) )
                                {
                                        collisions.push_back( gridItem.getUniqueName() );
                                        collisionFound = true;
                                }
                        }
                }
        }
        // for a free item
        else if ( item->whichItemClass() == "free item" || item->whichItemClass() == "avatar item" )
        {
                int xStart = item->getX() / room->getSizeOfOneTile ();
                int xEnd = 1 + ( item->getX() + item->getWidthX() - 1 ) / room->getSizeOfOneTile ();
                int yStart = ( item->getY() - item->getWidthY() + 1 ) / room->getSizeOfOneTile ();
                int yEnd = 1 + item->getY() / room->getSizeOfOneTile () ;

                // when an item collides with a wall
                if ( xStart < 0 ) xStart = 0;
                if ( xEnd > static_cast< int >( room->getTilesX () ) ) xEnd = room->getTilesX();
                if ( yStart < 0 ) yStart = 0;
                if ( yEnd > static_cast< int >( room->getTilesY () ) ) yEnd = room->getTilesY();

                // look for collisions within the range of cells where the item is
                for ( int i = xStart; i < xEnd; i++ ) {
                        for ( int j = yStart; j < yEnd; j++ )
                        {
                                std::vector< GridItemPtr > items = room->gridItems[ room->getTilesX() * j + i ] ;

                                for ( std::vector< GridItemPtr >::const_iterator g = items.begin (); g != items.end (); ++ g )
                                {
                                        const GridItem& gridItem = *( *g ) ;

                                        if ( ( item->getZ() < gridItem.getZ() + gridItem.getHeight() ) &&
                                                ( gridItem.getZ() < item->getZ() + item->getHeight() ) )
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
                        if ( freeItem.getX() + freeItem.getWidthX() > item.getX()
                                        && freeItem.getX() < item.getX() + item.getWidthX()
                                && freeItem.getY() > item.getY() - item.getWidthX()
                                        && freeItem.getY() - freeItem.getWidthY() < item.getY()
                                && freeItem.getZ() + freeItem.getHeight() > item.getZ() )
                        {
                                z = freeItem.getZ() + freeItem.getHeight() ;
                        }
                }
        }

        if ( item.whichItemClass() == "grid item" )
        {
                int column = dynamic_cast< const GridItem& >( item ).getColumnOfGrid ();

                // look for the highest Z in the column
                for ( std::vector< GridItemPtr >::const_iterator g = room->gridItems[ column ].begin ();
                                g != room->gridItems[ column ].end (); ++ g )
                {
                        const GridItem & gridItem = *( *g ) ;
                        int zPlusHeight = gridItem.getZ() + gridItem.getHeight() ;

                        if ( zPlusHeight > z ) z = zPlusHeight ;
                }
        }
        else if ( item.whichItemClass() == "free item" || item.whichItemClass() == "avatar item" )
        {
                const FreeItem & freeItem = dynamic_cast< const FreeItem& >( item );

                // get cells where the item is
                int xStart = freeItem.getX() / room->getSizeOfOneTile();
                int xEnd = 1 + ( freeItem.getX() + freeItem.getWidthX() - 1 ) / room->getSizeOfOneTile() ;
                int yStart = ( freeItem.getY() - freeItem.getWidthY() + 1 ) / room->getSizeOfOneTile() ;
                int yEnd = 1 + freeItem.getY() / room->getSizeOfOneTile() ;

                // look for items within these cells
                for ( int i = xStart; i < xEnd; ++ i ) {
                        for ( int j = yStart; j < yEnd; ++ j )
                        {
                                int column = room->getTilesX() * j + i ;
                                for ( std::vector< GridItemPtr >::const_iterator g = room->gridItems[ column ].begin ();
                                                g != room->gridItems[ column ].end (); ++ g )
                                {
                                        const GridItem & gridItem = *( *g ) ;
                                        int zPlusHeight = gridItem.getZ() + gridItem.getHeight() ;

                                        if ( zPlusHeight > z ) z = zPlusHeight ;
                                }
                        }
                }
        }

        return z;
}

void Mediator::addCollisionWith( const std::string & what )
{
# if defined( DEBUG_COLLISIONS ) && DEBUG_COLLISIONS
        std::cout << ")( pushing collision with \"" << what << "\"" << std::endl ;
# endif

        lockCollisionsMutex ();
        collisions.push_front( what );
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

ItemPtr Mediator::collisionWithSomeKindOf( const std::string & kind )
{
        for ( unsigned int i = 0; i < collisions.size(); i ++ ) {
                ItemPtr item = findItemByUniqueName( collisions[ i ] );

                if ( item != nilPointer && item->getKind () == kind )
                        return item ;
        }

        return ItemPtr ();
}

ItemPtr Mediator::collisionWithBehavingAs( const std::string & behavior )
{
        for ( unsigned int i = 0; i < collisions.size(); i ++ ) {
                ItemPtr item = findItemByUniqueName( collisions[ i ] );

                if ( item != nilPointer && item->getBehavior() != nilPointer &&
                                item->getBehavior()->getNameOfBehavior () == behavior )
                        return item ;
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

void Mediator::setActiveCharacter ( const AvatarItemPtr & character )
{
        if ( this->currentlyActiveCharacter != character )
        {
                this->currentlyActiveCharacter = character ;

                std::cout << "character \"" << getNameOfActiveCharacter() << "\" is yet active in room \"" << room->getNameOfRoomDescriptionFile() << "\"" << std::endl ;
        }
}

bool Mediator::pickNextCharacter ()
{
        AvatarItemPtr previousCharacter( getActiveCharacter() );

        // look for the next character
        std::vector< AvatarItemPtr > charactersInRoom = room->getCharactersYetInRoom() ;
        AvatarItemPtr nextCharacter ( nilPointer );
        for ( unsigned int i = 0 ; i < charactersInRoom.size () ; ++ i )
        {
                AvatarItemPtr character = charactersInRoom[ i ];
                if ( character != nilPointer && character->getOriginalKind() != previousCharacter->getOriginalKind() ) {
                        nextCharacter = character ;
                        break ;
                }
        }
        if ( nextCharacter != nilPointer ) setActiveCharacter( nextCharacter );

        const AvatarItemPtr & currentCharacter = getActiveCharacter() ;

        if ( previousCharacter->getOriginalKind() != currentCharacter->getOriginalKind() )
        {
                // see if the characters may join here

                const int delta = room->getSizeOfOneTile() >> 1 ;

                if ( ( previousCharacter->getOriginalKind() == "head" && currentCharacter->getOriginalKind() == "heels" ) ||
                                ( previousCharacter->getOriginalKind() == "heels" && currentCharacter->getOriginalKind() == "head" ) )
                {
                        if ( ( previousCharacter->getX() + delta >= currentCharacter->getX() )
                                && ( previousCharacter->getX() + previousCharacter->getWidthX() - delta <= currentCharacter->getX() + currentCharacter->getWidthX() )
                                && ( previousCharacter->getY() + delta >= currentCharacter->getY() )
                                && ( previousCharacter->getY() + previousCharacter->getWidthY() - delta <= currentCharacter->getY() + currentCharacter->getWidthY() )
                                && ( ( previousCharacter->getOriginalKind() == "head" && previousCharacter->getZ() - Room::LayerHeight == currentCharacter->getZ() )
                                        || ( previousCharacter->getOriginalKind() == "heels" && currentCharacter->getZ() - Room::LayerHeight == previousCharacter->getZ() ) ) )
                        {
                                lockFreeItemsMutex ();

                                AvatarItemPtr reference = ( previousCharacter->getOriginalKind() == "heels" ) ? previousCharacter : currentCharacter ;
                                this->lastActiveCharacterBeforeJoining = ( previousCharacter->getOriginalKind() == "heels" ) ? "heels" : "head" ;

                                int x = reference->getX();
                                int y = reference->getY();
                                int z = reference->getZ();
                                const std::string & heading = reference->getHeading() ;

                                // an item that Heels may have in the handbag
                                AvatarItemPtr heels = reference;
                                const DescriptionOfItem* descriptionOfItemInBag = heels->getDescriptionOfTakenItem ();
                                std::string behaviorOfItemInBag = heels->getBehaviorOfTakenItem( );

                                // remove the simple characters
                                this->room->removeCharacterFromRoom( *previousCharacter, false );
                                this->room->removeCharacterFromRoom( *currentCharacter, false );

                                // create the composite character
                                setActiveCharacter( RoomBuilder::createCharacterInRoom( this->room, "headoverheels", false, x, y, z, heading ) );

                                // transfer an item in the handbag
                                if ( descriptionOfItemInBag != nilPointer )
                                {
                                        std::cout << "item \"" << descriptionOfItemInBag->getKind ()
                                                        << "\" goes into the bag of \"" << getActiveCharacter()->getKind () << "\"" << std::endl ;
                                        getActiveCharacter()->placeItemInBag( descriptionOfItemInBag->getKind (), behaviorOfItemInBag );
                                }

                                unlockFreeItemsMutex ();

                                std::cout << "join both characters into \"" << getActiveCharacter()->getOriginalKind () << "\""
                                                << " in room " << room->getNameOfRoomDescriptionFile() << std::endl ;
                                return true;
                        }
                }
        }
        // is it composite character
        else if ( currentCharacter->getOriginalKind() == "headoverheels" )
        {
                std::cout << "split \"" << currentCharacter->getOriginalKind() << "\" in room " << room->getNameOfRoomDescriptionFile() << std::endl ;

                int x = currentCharacter->getX ();
                int y = currentCharacter->getY ();
                int z = currentCharacter->getZ ();
                const std::string & heading = currentCharacter->getHeading () ;

                lockFreeItemsMutex ();

                // get the description of item in the handbag
                const DescriptionOfItem* descriptionOfItemInBag = currentCharacter->getDescriptionOfTakenItem ();
                std::string behaviorOfItemInBag = currentCharacter->getBehaviorOfTakenItem( );

                // remove the composite character
                this->room->removeCharacterFromRoom( *currentCharacter, false );

                // create the simple characters

                AvatarItemPtr characterHeels = RoomBuilder::createCharacterInRoom( this->room, "heels", false, x, y, z, heading );

                if ( descriptionOfItemInBag != nilPointer )
                {
                        std::cout << "transfer item \"" << descriptionOfItemInBag->getKind ()
                                        << "\" to the character \"" << characterHeels->getKind () << "\"" << std::endl ;
                        characterHeels->placeItemInBag( descriptionOfItemInBag->getKind (), behaviorOfItemInBag );
                }

                AvatarItemPtr characterHead = RoomBuilder::createCharacterInRoom( this->room, "head", false, x, y, z + Room::LayerHeight, heading );

                setActiveCharacter( ( this->lastActiveCharacterBeforeJoining == "head" ) ? characterHeels : characterHead );

                unlockFreeItemsMutex ();
        }

        if ( previousCharacter->getOriginalKind() == getActiveCharacter()->getOriginalKind() )
                return false ;

        std::cout << "swop character \"" << previousCharacter->getOriginalKind() << "\" to character \"" << getActiveCharacter()->getOriginalKind() << "\""
                        << " in room " << room->getNameOfRoomDescriptionFile() << std::endl ;
        return true ;
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
                                freeItem.getBehavior()->setCurrentActivity(
                                        this->switchInRoomIsOn ?
                                                activities::Activity::Freeze :
                                                activities::Activity::WakeUp );
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
                                        gridItem.getBehavior()->setCurrentActivity(
                                                this->switchInRoomIsOn ?
                                                        activities::Activity::Freeze :
                                                        activities::Activity::Waiting );
                                }
                        }
                }
        }

        std::cout << "toggled switch in room " << getRoom()->getNameOfRoomDescriptionFile() << std::endl ;
}

AvatarItemPtr Mediator::getWaitingCharacter() const
{
        std::vector< AvatarItemPtr > charactersInRoom = room->getCharactersYetInRoom() ;

        for ( std::vector< AvatarItemPtr >::iterator p = charactersInRoom.begin (); p != charactersInRoom.end (); ++ p )
        {
                if ( ( *p )->getUniqueName() != getActiveCharacter()->getUniqueName() )
                {
                        return *p ;
                }
        }

        return AvatarItemPtr ();
}

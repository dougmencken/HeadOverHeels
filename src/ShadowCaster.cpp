
#include "ShadowCaster.hpp"

#include "Item.hpp"
#include "FreeItem.hpp"
#include "GridItem.hpp"
#include "FloorTile.hpp"

#include <cmath>


namespace isomot
{

/* static */
void ShadowCaster::castShadowOnItem( Item* item, int x, int y, BITMAP* shadow, unsigned short shading, unsigned char transparency )
{
        // fully transparent stuff doesn’t drop any shadow
        if ( transparency >= 100 ) return;

        bool isFreeItem = ( item->whichKindOfItem() == "free item" || item->whichKindOfItem() == "player item" );
        bool isGridItem = ( item->whichKindOfItem() == "grid item" );

        if ( ! isFreeItem && ! isGridItem ) return;

        BITMAP* rawImage = item->getRawImage() ;

        int colorDepthRaw = bitmap_color_depth( rawImage ) ;
        int colorDepthShadow = bitmap_color_depth( shadow ) ;

        int width = item->getWidthX() ;
        int deltaW = 0 ;  // widths of grid item are always equal

        if ( isFreeItem )
        {
                if ( item->getWidthY() > item->getWidthX() )
                        width = item->getWidthY();

                deltaW = static_cast< int >( item->getWidthX() ) - static_cast< int >( item->getWidthY() );
        }

        int inix = x - item->getOffsetX();                      // initial X
        if ( inix < 0 ) inix = 0;
        int iniy = y - item->getOffsetY();                      // initial Y
        if ( iniy < 0 ) iniy = 0;
        int endx = x - item->getOffsetX() + shadow->w;          // final X
        if ( endx > rawImage->w ) endx = rawImage->w;
        int endy = y - item->getOffsetY() + shadow->h;          // final Y
        if ( endy > rawImage->h ) endy = rawImage->h;
        int my = rawImage->h - width - item->getHeight();       // intermediate Y
        if ( isGridItem ) my ++;                                // for better shadow on sides of grid item
        if ( endy < my ) my = endy;
        if ( endy > my + width ) endy = my + width;

        // continue when initial coordinates are less than final coordinates
        // id est if ( iniy < endy && inix < endx )
        if ( iniy >= endy || inix >= endx ) return;

        int iRow = 0;           // row in rows of pixels in image and shady image of this item
        int sRow = 0;           // row in rows of pixels in shadow shading this item
        int sPixel = 0;         // pixel in a row of shadow shading this item

        // indices to walk thru color components in a row of image & shady image
        int iRpx = 0;           // red
        int iGpx = 0;           // green
        int iBpx = 0;           // blue

        int n2i = inix + item->getOffsetX() - x;

        BITMAP* shadyImage = nilPointer; // graphics of shaded item

        if ( isFreeItem )
        {
                shadyImage = dynamic_cast< FreeItem* >( item )->getShadedNonmaskedImage();
        }
        else if ( isGridItem )
        {
                shadyImage = item->getProcessedImage();
        }

        if ( shadyImage == nilPointer )
        {
                // initially, image of shaded item is just copy of unshaded item’s image
                shadyImage = create_bitmap_ex( colorDepthRaw, rawImage->w, rawImage->h );
        }

        if ( item->whichShade() == WantReshadow )
        {
                blit( rawImage, shadyImage, 0, 0, 0, 0, rawImage->w, rawImage->h );
                item->setWhichShade( AlreadyShady );
        }

        char iInc = ( colorDepthRaw == 32 ? 4 : 3 );            // increment for iRpx, iGpx and iBpx
        char sInc = ( colorDepthShadow == 32 ? 4 : 3 );         // increment for sPixel

        // degree of opacity from 0, full opacity, to 255, full transparency
        short opacity = static_cast< short >( ( ( 256.0 - shading ) / 100 ) * transparency + shading );

        endx *= iInc;
        inix *= iInc;
        n2i *= sInc;
        #if IS_BIG_ENDIAN
                inix += colorDepthRaw == 32 ? 1 : 0 ;
                n2i += colorDepthShadow == 32 ? 1 : 0 ;
        #endif

        int ltpx = 0;   // first component of first pixel, the one furthest to the left, where lateral shading begins
        int ltpx1 = 0;  // second component of first pixel, the one furthest to the left, where lateral shading begins
        int rtpx = 0;   // first component of last pixel, the one furthest to the right, where lateral shading ends
        int rtpx1 = 0;  // second component of last pixel, the one furthest to the right, where lateral shading ends

        // when opacity is power of 2 in interval [ 2 ... 128 ]
        if ( static_cast< int >( std::pow( 2, std::log10( opacity ) / std::log10( 2 ) ) ) == opacity )
        {
                char pxDiv = 7;  // 2 ^ pxDiv is divisor for shaded pixels

                // tune divisor by opacity of shadow
                while ( opacity != 2 )
                {
                        opacity = opacity >> 1;
                        pxDiv--;
                }

                // shade top surface of item
                // rows of three images are crossed within calculated limits
                for ( iRow = iniy, sRow = iniy + item->getOffsetY() - y; iRow < my; iRow++, sRow++ )
                {
                        unsigned char* sln = shadow->line[ sRow ];
                        unsigned char* iln = rawImage->line[ iRow ];
                        unsigned char* rln = shadyImage->line[ iRow ];

                        // walk thru pixels of each row within calculated limits
                        for ( iRpx = inix, iGpx = inix + 1, iBpx = inix + 2, sPixel = n2i;
                                        iRpx < endx;
                                        iRpx += iInc, iGpx += iInc, iBpx += iInc, sPixel += sInc )
                        {
                                // when pixel of three images isn’t key color
                                // then divide pixel of result by 2 ^ pxDiv, darkening it
                                if ( ( ! isKeyColor( iln[ iRpx ], iln[ iGpx ], iln[ iBpx ] ) ) &&
                                        ( iln[ iRpx ] == rln[ iRpx ] && iln[ iGpx ] == rln[ iGpx ] && iln[ iBpx ] == rln[ iBpx ] ) &&
                                        ( ! isKeyColor( sln[ sPixel ], sln[ sPixel + 1 ], sln[ sPixel + 2 ] ) ) )
                                {
                                        rln[ iRpx ] = iln[ iRpx ] >> pxDiv;
                                        rln[ iGpx ] = iln[ iGpx ] >> pxDiv;
                                        rln[ iBpx ] = iln[ iBpx ] >> pxDiv;
                                }
                        }
                }
                // so far item is shaded horizontally to line ‘my’

                // shade sides of item
                ltpx = ( rawImage->w >> 1 ) - ( width << 1 ) + deltaW + ( ( iRow - my ) << 1 );
                rtpx = ( rawImage->w >> 1 ) + ( width << 1 ) + deltaW - ( ( iRow - my ) << 1 ) - 2;
                ltpx *= iInc;
                rtpx *= iInc;
        #if IS_BIG_ENDIAN
                ltpx += colorDepthRaw == 32 ? 1 : 0 ;
                rtpx += colorDepthRaw == 32 ? 1 : 0 ;
        #endif

                // shadow left and right parts of item
                for ( ltpx1 = ltpx + iInc, rtpx1 = rtpx + iInc;
                                iRow < endy;
                                iRow++, sRow++, ltpx += 2 * iInc, ltpx1 += 2 * iInc, rtpx -= 2 * iInc, rtpx1 -= 2 * iInc )
                {
                        unsigned char* sln = shadow->line[ sRow ];
                        unsigned char* iln = rawImage->line[ iRow ];
                        unsigned char* rln = shadyImage->line[ iRow ];

                        if ( inix < ltpx )
                        {
                                inix = ltpx;
                                n2i = inix + ( item->getOffsetX() - x ) * sInc;
                        }

                        if ( endx > rtpx + 2 * iInc )
                        {
                                endx = rtpx + 2 * iInc;
                        }

                        for ( iRpx = inix, iGpx = inix + 1, iBpx = inix + 2, sPixel = n2i;
                                        iRpx < endx;
                                        iRpx += iInc, iGpx += iInc, iBpx += iInc, sPixel += sInc )
                        {
                                if ( ! isKeyColor( sln[ sPixel ], sln[ sPixel + 1 ], sln[ sPixel + 2 ] ) )
                                {
                                        if ( ( ! isKeyColor( iln[ iRpx ], iln[ iGpx ], iln[ iBpx ] ) ) &&
                                                ( iln[ iRpx ] == rln[ iRpx ] && iln[ iGpx ] == rln[ iGpx ] && iln[ iBpx ] == rln[ iBpx ] ) )
                                        {
                                                rln[ iRpx ] = iln[ iRpx ] >> pxDiv;
                                                rln[ iGpx ] = iln[ iGpx ] >> pxDiv;
                                                rln[ iBpx ] = iln[ iBpx ] >> pxDiv;
                                        }

                                        if ( iRpx == ltpx || iRpx == ltpx1 || iRpx == rtpx || iRpx == rtpx1 )
                                        {
                                                for ( int yy = iRow + 1; yy < rawImage->h; yy++ )
                                                {
                                                        unsigned char* lraw = rawImage->line[ yy ];
                                                        unsigned char* lshady = shadyImage->line[ yy ];

                                                        if ( ( ! isKeyColor( lraw[ iRpx ], lraw[ iGpx ], lraw[ iBpx ] ) ) &&
                                                                ( lraw[ iRpx ] == lshady[ iRpx ] && lraw[ iGpx ] == lshady[ iGpx ] && lraw[ iBpx ] == lshady[ iBpx ] ) )
                                                        {
                                                                lshady[ iRpx ] = lraw[ iRpx ] >> pxDiv;
                                                                lshady[ iGpx ] = lraw[ iGpx ] >> pxDiv;
                                                                lshady[ iBpx ] = lraw[ iBpx ] >> pxDiv;
                                                        }
                                                }
                                        }
                                }
                        }
                }
        }
        // opacity is not power of two
        else
        {
                // there’s some transparency
                if ( opacity != 0 )
                {
                        // rows of three images are crossed within calculated limits
                        for ( iRow = iniy, sRow = iniy + item->getOffsetY() - y; iRow < my; iRow++, sRow++ )
                        {
                                unsigned short color;
                                unsigned char* sln = shadow->line[ sRow ];
                                unsigned char* iln = rawImage->line[ iRow ];
                                unsigned char* rln = shadyImage->line[ iRow ];

                                // walk thru pixels of each row within calculated limits
                                for ( iRpx = inix, iGpx = inix + 1, iBpx = inix + 2, sPixel = n2i;
                                                iRpx < endx;
                                                iRpx += iInc, iGpx += iInc, iBpx += iInc, sPixel += sInc )
                                {
                                        // when pixel of three images isn’t key color
                                        // then decrement value of result’s pixel to darken it
                                        if ( ( ! isKeyColor( iln[ iRpx ], iln[ iGpx ], iln[ iBpx ] ) ) &&
                                                ( iln[ iRpx ] == rln[ iRpx ] && iln[ iGpx ] == rln[ iGpx ] && iln[ iBpx ] == rln[ iBpx ] ) &&
                                                ( ! isKeyColor( sln[ sPixel ], sln[ sPixel + 1 ], sln[ sPixel + 2 ] ) ) )
                                        {
                                                color = iln[ iRpx ] * opacity;
                                                rln[ iRpx ] = static_cast< unsigned char >( color >> 8 );
                                                color = iln[ iGpx ] * opacity;
                                                rln[ iGpx ] = static_cast< unsigned char >( color >> 8 );
                                                color = iln[ iBpx ] * opacity;
                                                rln[ iBpx ] = static_cast< unsigned char >( color >> 8 );
                                        }
                                }
                        }

                        ltpx = ( rawImage->w >> 1 ) - ( width << 1 ) + deltaW + ( ( iRow - my ) << 1 );
                        rtpx = ( rawImage->w >> 1 ) + ( width << 1 ) + deltaW - ( ( iRow - my ) << 1 ) - 2;
                        ltpx *= iInc;
                        rtpx *= iInc;
                #if IS_BIG_ENDIAN
                        ltpx += colorDepthRaw == 32 ? 1 : 0 ;
                        rtpx += colorDepthRaw == 32 ? 1 : 0 ;
                #endif

                        // shadow left and right parts of item
                        for ( ltpx1 = ltpx + iInc, rtpx1 = rtpx + iInc;
                                        iRow < endy;
                                        iRow++, sRow++, ltpx += 2 * iInc, ltpx1 += 2 * iInc, rtpx -= 2 * iInc, rtpx1 -= 2 * iInc )
                        {
                                unsigned char* sln = shadow->line[ sRow ];
                                unsigned char* iln = rawImage->line[ iRow ];
                                unsigned char* rln = shadyImage->line[ iRow ];

                                if ( inix < ltpx )
                                {
                                        inix = ltpx;
                                        n2i = inix + ( item->getOffsetX() - x ) * sInc;
                                }

                                if ( endx > rtpx + 2 * iInc )
                                {
                                        endx = rtpx + 2 * iInc;
                                }

                                for ( iRpx = inix, iGpx = inix + 1, iBpx = inix + 2, sPixel = n2i;
                                                iRpx < endx;
                                                iRpx += iInc, iGpx += iInc, iBpx += iInc, sPixel += sInc )
                                {
                                        if ( ! isKeyColor( sln[ sPixel ], sln[ sPixel + 1 ], sln[ sPixel + 2 ] ) )
                                        {
                                                if ( ( ! isKeyColor( iln[ iRpx ], iln[ iGpx ], iln[ iBpx ] ) ) &&
                                                        ( iln[ iRpx ] == rln[ iRpx ] && iln[ iGpx ] == rln[ iGpx ] && iln[ iBpx ] == rln[ iBpx ] ) )
                                                {
                                                        unsigned short color;
                                                        color = iln[ iRpx ] * opacity;
                                                        rln[ iRpx ] = static_cast< unsigned char >( color >> 8 );
                                                        color = iln[ iGpx ] * opacity;
                                                        rln[ iGpx ] = static_cast< unsigned char >( color >> 8 );
                                                        color = iln[ iBpx ] * opacity;
                                                        rln[ iBpx ] = static_cast< unsigned char >( color >> 8 );

                                                        if ( iRpx == ltpx || iRpx == ltpx1 || iRpx == rtpx || iRpx == rtpx1 )
                                                        {
                                                                for ( int yy = iRow + 1; yy < rawImage->h; yy++ )
                                                                {
                                                                        unsigned char* lraw = rawImage->line[ yy ];
                                                                        unsigned char* lshady = shadyImage->line[ yy ];

                                                                        if ( ( ! isKeyColor( lraw[ iRpx ], lraw[ iGpx ], lraw[ iBpx ] ) ) &&
                                                                                ( lraw[ iRpx ] == lshady[ iRpx ] && lraw[ iGpx ] == lshady[ iGpx ] && lraw[ iBpx ] == lshady[ iBpx ] ) )
                                                                        {
                                                                                color = lraw[ iRpx ] * opacity;
                                                                                lshady[ iRpx ] = static_cast< unsigned char >( color >> 8 );
                                                                                color = lraw[ iGpx ] * opacity;
                                                                                lshady[ iGpx ] = static_cast< unsigned char >( color >> 8 );
                                                                                color = lraw[ iBpx ] * opacity;
                                                                                lshady[ iBpx ] = static_cast< unsigned char >( color >> 8 );
                                                                        }
                                                                }
                                                        }
                                                }
                                        }
                                }
                        }
                }
                // zero opacity is full opacity
                else
                {
                        // rows of three images are crossed within calculated limits
                        for ( iRow = iniy, sRow = iniy + item->getOffsetY() - y; iRow < my; iRow++, sRow++ )
                        {
                                unsigned char* sln = shadow->line[ sRow ];
                                unsigned char* iln = rawImage->line[ iRow ];
                                unsigned char* rln = shadyImage->line[ iRow ];

                                // walk thru pixels of each row within calculated limits
                                for ( iRpx = inix, iGpx = inix + 1, iBpx = inix + 2, sPixel = n2i;
                                                iRpx < endx;
                                                iRpx += iInc, iGpx += iInc, iBpx += iInc, sPixel += sInc )
                                {
                                        // when pixel of three images isn’t key color
                                        // then pixel of result is zero, pure black
                                        if ( ( ! isKeyColor( iln[ iRpx ], iln[ iGpx ], iln[ iBpx ] ) ) &&
                                                ( iln[ iRpx ] == rln[ iRpx ] && iln[ iGpx ] == rln[ iGpx ] && iln[ iBpx ] == rln[ iBpx ] ) &&
                                                ( ! isKeyColor( sln[ sPixel ], sln[ sPixel + 1 ], sln[ sPixel + 2 ] ) ) )
                                        {
                                                rln[ iRpx ] = rln[ iGpx ] = rln[ iBpx ] = 0;
                                        }
                                }
                        }

                        ltpx = ( rawImage->w >> 1 ) - ( width << 1 ) + deltaW + ( ( iRow - my ) << 1 );
                        rtpx = ( rawImage->w >> 1 ) + ( width << 1 ) + deltaW - ( ( iRow - my ) << 1 ) - 2;
                        ltpx *= iInc;
                        rtpx *= iInc;
                #if IS_BIG_ENDIAN
                        ltpx += colorDepthRaw == 32 ? 1 : 0 ;
                        rtpx += colorDepthRaw == 32 ? 1 : 0 ;
                #endif

                        // shadow left and right parts of item
                        for ( ltpx1 = ltpx + iInc, rtpx1 = rtpx + iInc;
                                        iRow < endy;
                                        iRow++, sRow++, ltpx += 2 * iInc, ltpx1 += 2 * iInc, rtpx -= 2 * iInc, rtpx1 -= 2 * iInc )
                        {
                                unsigned char* sln = shadow->line[ sRow ];
                                unsigned char* iln = rawImage->line[ iRow ];
                                unsigned char* rln = shadyImage->line[ iRow ];

                                if ( inix < ltpx )
                                {
                                        inix = ltpx;
                                        n2i = inix + ( item->getOffsetX() - x ) * sInc;
                                }

                                if ( endx > rtpx + 2 * iInc )
                                {
                                        endx = rtpx + 2 * iInc;
                                }

                                for ( iRpx = inix, iGpx = inix + 1, iBpx = inix + 2, sPixel = n2i;
                                                iRpx < endx;
                                                iRpx += iInc, iGpx += iInc, iBpx += iInc, sPixel += sInc )
                                {
                                        if ( ! isKeyColor( sln[ sPixel ], sln[ sPixel + 1 ], sln[ sPixel + 2 ] ) )
                                        {
                                                if ( ( ! isKeyColor( iln[ iRpx ], iln[ iGpx ], iln[ iBpx ] ) ) &&
                                                        ( iln[ iRpx ] == rln[ iRpx ] && iln[ iGpx ] == rln[ iGpx ] && iln[ iBpx ] == rln[ iBpx ] ) )
                                                {
                                                        rln[ iRpx ] = rln[ iGpx ] = rln[ iBpx ] = 0;

                                                        if ( iRpx == ltpx || iRpx == ltpx1 || iRpx == rtpx || iRpx == rtpx1 )
                                                        {
                                                                for ( int yy = iRow + 1; yy < rawImage->h; yy++ )
                                                                {
                                                                        unsigned char* lraw = rawImage->line[ yy ];
                                                                        unsigned char* lshady = shadyImage->line[ yy ];

                                                                        if ( ( ! isKeyColor( lraw[ iRpx ], lraw[ iGpx ], lraw[ iBpx ] ) ) &&
                                                                                ( lraw[ iRpx ] == lshady[ iRpx ] && lraw[ iGpx ] == lshady[ iGpx ] && lraw[ iBpx ] == lshady[ iBpx ] ) )
                                                                        {
                                                                                lshady[ iRpx ] = lshady[ iGpx ] = lshady[ iBpx ] = 0;
                                                                        }
                                                                }
                                                        }
                                                }
                                        }
                                }
                        }
                }
        }

        if ( isFreeItem )
        {
                dynamic_cast< FreeItem* >( item )->setShadedNonmaskedImage( shadyImage );
        }
        else if ( isGridItem )
        {
                item->setProcessedImage( shadyImage );
        }
}

/* static */
void ShadowCaster::castShadowOnFloor( FloorTile* tile, int x, int y, BITMAP* shadow, unsigned short shading, unsigned char transparency )
{
        // fully transparent stuff doesn’t drop any shadow
        if ( transparency >= 100 ) return;

        BITMAP* tileImage = tile->getRawImage() ;

        int colorDepthTile = bitmap_color_depth( tileImage ) ;
        int colorDepthShadow = bitmap_color_depth( shadow ) ;

        int inix = x - tile->getOffsetX();                      // initial X
        if ( inix < 0 ) inix = 0;
        int iniy = y - tile->getOffsetY();                      // initial Y
        if ( iniy < 0 ) iniy = 0;
        int endx = x - tile->getOffsetX() + shadow->w;          // final X
        if ( endx > tileImage->w ) endx = tileImage->w;
        int endy = y - tile->getOffsetY() + shadow->h;          // final Y
        if ( endy > tileImage->h ) endy = tileImage->h;

        // continue when initial coordinates are less than final coordinates
        // id est if ( iniy < endy && inix < endx )
        if ( iniy >= endy || inix >= endx ) return;

        int iRow = 0;        // row in rows of pixels in image and shady image of this tile
        int sRow = 0;        // row in rows of pixels in shadow shading this tile
        int sPixel = 0;      // pixel in a row of shadow shading this tile

        // indices to walk thru color components in a row of image & shady image
        int iRpx = 0;        // red
        int iGpx = 0;        // green
        int iBpx = 0;        // blue

        int n2i = inix + tile->getOffsetX() - x;

        BITMAP* shadyImage = tile->getShadyImage();             // graphics of shaded item

        if ( shadyImage == nilPointer )
        {
                shadyImage = create_bitmap_ex( colorDepthTile, tileImage->w, tileImage->h );
        }

        if ( tile->getWantShadow() )
        {
                // when there’s only one shading item, begin with fresh image of tile
                blit( tileImage, shadyImage, 0, 0, 0, 0, tileImage->w, tileImage->h );
                tile->setWantShadow( false );
        }

        char iInc = ( colorDepthTile == 32 ? 4 : 3 );           // increment for iRpx, iGpx and iBpx
        char sInc = ( colorDepthShadow == 32 ? 4 : 3 );         // increment for sPixel

        // degree of opacity from 0, full opacity, to 255, full transparency
        short opacity = static_cast< short >( ( ( 256.0 - shading ) / 100 ) * transparency + shading );

        endx *= iInc;
        inix *= iInc;
        n2i *= sInc;
        #if IS_BIG_ENDIAN
                inix += colorDepthTile == 32 ? 1 : 0 ;
                n2i += colorDepthShadow == 32 ? 1 : 0;
        #endif

        // when opacity is power of 2 in interval [ 2 ... 128 ]
        if ( static_cast< int >( std::pow( 2, std::log10( opacity ) / std::log10( 2 ) ) ) == opacity )
        {
                char pxDiv = 7;  // 2 ^ pxDiv is divisor for shaded pixels

                // tune divisor by opacity of shadow
                while ( opacity != 2 )
                {
                        opacity = opacity >> 1;
                        pxDiv--;
                }

                // rows of three images are crossed within calculated limits
                for ( iRow = iniy, sRow = iniy + tile->getOffsetY() - y; iRow < endy; iRow++, sRow++ )
                {
                        unsigned char* sln = shadow->line[ sRow ];
                        unsigned char* iln = tileImage->line[ iRow ];
                        unsigned char* rln = shadyImage->line[ iRow ];

                        // walk thru pixels of each row within calculated limits
                        for ( iRpx = inix, iGpx = inix + 1, iBpx = inix + 2, sPixel = n2i ;
                                        iRpx < endx ;
                                        iRpx += iInc, iGpx += iInc, iBpx += iInc, sPixel += sInc )
                        {
                                // when pixel of three images isn’t key color
                                // then divide pixel of result by 2 ^ pxDiv, darkening it
                                if ( ( ! isKeyColor( iln[ iRpx ], iln[ iGpx ], iln[ iBpx ] ) )
                                        && ( iln[ iRpx ] == rln[ iRpx ] && iln[ iGpx ] == rln[ iGpx ] && iln[ iBpx ] == rln[ iBpx ] )
                                        && ( ! isKeyColor( sln[ sPixel ], sln[ sPixel + 1 ], sln[ sPixel + 2 ] ) ) )
                                {
                                        rln[ iRpx ] = iln[ iRpx ] >> pxDiv;
                                        rln[ iGpx ] = iln[ iGpx ] >> pxDiv;
                                        rln[ iBpx ] = iln[ iBpx ] >> pxDiv;
                                }
                        }
                }
        }
        // opacity is not power of two
        else
        {
                // there’s some transparency
                if ( opacity != 0 )
                {
                        // rows of three images are crossed within calculated limits
                        for ( iRow = iniy, sRow = iniy + tile->getOffsetY() - y; iRow < endy; iRow++, sRow++ )
                        {
                                unsigned short color;
                                unsigned char* sln = shadow->line[ sRow ];
                                unsigned char* iln = tileImage->line[ iRow ];
                                unsigned char* rln = shadyImage->line[ iRow ];

                                // walk thru pixels of each row within calculated limits
                                for ( iRpx = inix, iGpx = inix + 1, iBpx = inix + 2, sPixel = n2i ;
                                                iRpx < endx ;
                                                iRpx += iInc, iGpx += iInc, iBpx += iInc, sPixel += sInc )
                                {
                                        // when pixel of three images isn’t key color
                                        // then decrement value of result’s pixel to darken it
                                        if ( ( ! isKeyColor( iln[ iRpx ], iln[ iGpx ], iln[ iBpx ] ) )
                                                && ( iln[ iRpx ] == rln[ iRpx ] && iln[ iGpx ] == rln[ iGpx ] && iln[ iBpx ] == rln[ iBpx ] )
                                                && ( ! isKeyColor( sln[ sPixel ], sln[ sPixel + 1 ], sln[ sPixel + 2 ] ) ) )
                                        {
                                                color = iln[ iRpx ] * opacity;
                                                rln[ iRpx ] = static_cast< unsigned char >( color >> 8 );
                                                color = iln[ iGpx ] * opacity;
                                                rln[ iGpx ] = static_cast< unsigned char >( color >> 8 );
                                                color = iln[ iBpx ] * opacity;
                                                rln[ iBpx ] = static_cast< unsigned char >( color >> 8 );
                                        }
                                }
                        }
                }
                // zero opacity is full opacity
                else
                {
                        // rows of three images are crossed within calculated limits
                        for ( iRow = iniy, sRow = iniy + tile->getOffsetY() - y; iRow < endy; iRow++, sRow++ )
                        {
                                unsigned char* sln = shadow->line[ sRow ];
                                unsigned char* iln = tileImage->line[ iRow ];
                                unsigned char* rln = shadyImage->line[ iRow ];

                                // walk thru pixels of each row within calculated limits
                                for ( iRpx = inix, iGpx = inix + 1, iBpx = inix + 2, sPixel = n2i ;
                                                iRpx < endx ;
                                                iRpx += iInc, iGpx += iInc, iBpx += iInc, sPixel += sInc )
                                {
                                        // when pixel of three images isn’t key color
                                        // then pixel of result is zero, pure black
                                        if ( ( ! isKeyColor( iln[ iRpx ], iln[ iGpx ], iln[ iBpx ] ) )
                                                && ( iln[ iRpx ] == rln[ iRpx ] && iln[ iGpx ] == rln[ iGpx ] && iln[ iBpx ] == rln[ iBpx ] )
                                                && ( ! isKeyColor( sln[ sPixel ], sln[ sPixel + 1 ], sln[ sPixel + 2 ] ) ) )
                                        {
                                                rln[ iRpx ] = rln[ iRpx + 1 ] = rln[ iRpx + 2 ] = 0;
                                        }
                                }
                        }
                }
        }

        tile->setShadyImage( shadyImage );
}

}

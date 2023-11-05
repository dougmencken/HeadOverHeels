
#include "ShadowCaster.hpp"

#include "Color.hpp"
#include "FreeItem.hpp"
#include "GridItem.hpp"

#include <cmath>


/* static */
void ShadowCaster::castShadowOnItem( Item& item, int x, int y, const Picture& shadow, unsigned short shading, unsigned char transparency )
{
        if ( shading >= 256 ) return ;

        // fully transparent stuff doesn’t drop any shadow
        if ( transparency >= 100 ) return ;

        bool isFreeItem = ( item.whichItemClass() == "free item" || item.whichItemClass() == "avatar item" );
        bool isGridItem = ( item.whichItemClass() == "grid item" );

        if ( ! isFreeItem && ! isGridItem ) return;

        const Picture& rawImage = item.getRawImage() ;

        int height = static_cast< int >( item.getHeight() );
        int width = static_cast< int >( item.getWidthX() );
        int deltaW = 0 ;  // widths of grid item are always equal

        if ( isFreeItem )
        {
                if ( item.getWidthY() > item.getWidthX() )
                        width = static_cast< int >( item.getWidthY() );

                deltaW = static_cast< int >( item.getWidthX() ) - static_cast< int >( item.getWidthY() );
        }

        int iniX = x - item.getOffsetX();               // initial X
        int endX = iniX + shadow.getWidth();            // final X
        int iniY = y - item.getOffsetY();               // initial Y
        int endY = iniY + shadow.getHeight();           // final Y
        if ( iniX < 0 ) iniX = 0;
        if ( iniY < 0 ) iniY = 0;
        if ( endX > static_cast< int >( rawImage.getWidth() ) ) endX = rawImage.getWidth();
        if ( endY > static_cast< int >( rawImage.getHeight() ) ) endY = rawImage.getHeight();

        int mY = rawImage.getHeight() - width - height ;        // intermediate Y
        if ( isGridItem ) mY ++ ;                               // for better shadow on sides of grid item
        if ( endY < mY ) mY = endY ;
        if ( endY > mY + width ) endY = mY + width ;

        // continue when initial coordinates are less than final coordinates
        // id est if ( iniY < endY && iniX < endX )
        if ( iniY >= endY || iniX >= endX ) return;

        // graphics of shaded item
        Picture& shadyImage = isFreeItem ?
                                dynamic_cast< FreeItem& >( item ).getShadedNonmaskedImage()
                                : item.getProcessedImage() ;

        if ( item.getWantShadow() )
        {
                // for first or only one shading item begin with fresh image
                shadyImage.fillWithColor( Color() );
                allegro::bitBlit( rawImage.getAllegroPict(), shadyImage.getAllegroPict() );

                item.setWantShadow( false );
        }

        shadow.getAllegroPict().lock( true, false );
        rawImage.getAllegroPict().lock( true, false );
        shadyImage.getAllegroPict().lock( true, true );

        int itemRow = 0 ;       // row of pixels in image and shady image of this item
        int itemPixel = 0 ;     // pixel in row of image & shady image
        int shadowRow = 0 ;     // row of pixels in shadow shading this item
        int shadowPixel = 0 ;   // pixel in row of shadow shading this item

        int deltaX = iniX + item.getOffsetX() - x ;
        int deltaY = iniY + item.getOffsetY() - y ;

        const int iniltpx = ( rawImage.getWidth() >> 1 ) - ( width << 1 ) + deltaW ;
        const int inirtpx = ( rawImage.getWidth() >> 1 ) + ( width << 1 ) + deltaW - 2 ;

        int ltpx1 = iniltpx ;   // first component of first pixel, the one furthest to the left, where shading of sides begins
        int ltpx2 = ltpx1 + 1 ; // second component of first pixel, the one furthest to the left, where shading of sides begins
        int rtpx1 = inirtpx ;   // first component of last pixel, the one furthest to the right, where shading of sides ends
        int rtpx2 = rtpx1 + 1 ; // second component of last pixel, the one furthest to the right, where shading of sides ends

        // degree of opacity from 0, full opacity, to 255, full transparency
        short opacity = static_cast< short >( shading + ( ( 256.0 - shading ) * transparency / 100 ) );

        // when opacity is power of 2 in interval [ 2 ... 128 ]
        if ( opacity > 0 && static_cast< int >( std::pow( 2, std::log10( opacity ) / std::log10( 2 ) ) ) == opacity )
        {
                short pixDiv = 7 ; // 2 ^ pixDiv is divisor for shaded pixels

                // tune divisor by opacity of shadow
                while ( opacity != 2 )
                {
                        opacity = opacity >> 1 ;
                        pixDiv -- ;
                }

                // shade top surface of item

                // images are crossed within calculated limits
                for ( itemRow = iniY, shadowRow = deltaY ; itemRow < mY ; itemRow ++, shadowRow ++ )
                {
                        // walk thru pixels of each row within calculated limits
                        for ( itemPixel = iniX, shadowPixel = deltaX ; itemPixel < endX ; itemPixel ++, shadowPixel ++ )
                        {
                                AllegroColor shadowColor = shadow.getPixelAt( shadowPixel, shadowRow );
                                AllegroColor itemColor = rawImage.getPixelAt( itemPixel, itemRow );
                                AllegroColor resultColor = shadyImage.getPixelAt( itemPixel, itemRow );

                                // when pixel of shadow isn’t key color & pixel of item isn’t key color
                                // and pixel of result isn’t changed before
                                // then divide pixel of result by 2 ^ pixDiv, darkening it
                                if ( ! shadowColor.isKeyColor() && ! itemColor.isKeyColor() && resultColor.equalsRGB( itemColor ) )
                                {
                                        unsigned char resultRed = itemColor.getRed() >> pixDiv ;
                                        unsigned char resultGreen = itemColor.getGreen() >> pixDiv ;
                                        unsigned char resultBlue = itemColor.getBlue() >> pixDiv ;
                                        shadyImage.putPixelAt( itemPixel, itemRow, Color( resultRed, resultGreen, resultBlue, itemColor.getAlpha() ) );
                                }
                        }
                }
                // so far item is shaded horizontally to line ‘mY’

                // shade left and right sides of item

                for ( ; itemRow < endY ; itemRow ++, shadowRow ++, ltpx1 += 2, ltpx2 += 2, rtpx1 -= 2, rtpx2 -= 2 )
                {
                        if ( iniX < ltpx1 )
                        {
                                iniX = ltpx1 ;
                                deltaX = iniX + item.getOffsetX() - x ;
                        }

                        if ( endX > rtpx1 + 2 )
                        {
                                endX = rtpx1 + 2 ;
                        }

                        for ( itemPixel = iniX, shadowPixel = deltaX ; itemPixel < endX ; itemPixel ++, shadowPixel ++ )
                        {
                                AllegroColor shadowColor = shadow.getPixelAt( shadowPixel, shadowRow );
                                AllegroColor itemColor = rawImage.getPixelAt( itemPixel, itemRow );
                                AllegroColor resultColor = shadyImage.getPixelAt( itemPixel, itemRow );

                                if ( ! shadowColor.isKeyColor() )
                                {
                                        if ( ! itemColor.isKeyColor() && resultColor.equalsRGB( itemColor ) )
                                        {
                                                unsigned char resultRed = itemColor.getRed() >> pixDiv ;
                                                unsigned char resultGreen = itemColor.getGreen() >> pixDiv ;
                                                unsigned char resultBlue = itemColor.getBlue() >> pixDiv ;
                                                shadyImage.putPixelAt( itemPixel, itemRow, Color( resultRed, resultGreen, resultBlue, itemColor.getAlpha() ) );
                                        }

                                        if ( itemPixel == ltpx1 || itemPixel == ltpx2 || itemPixel == rtpx1 || itemPixel == rtpx2 )
                                        {
                                                for ( unsigned int yy = itemRow + 1 ; yy < rawImage.getHeight() ; yy++ )
                                                {
                                                        AllegroColor rawColor = rawImage.getPixelAt( itemPixel, yy );
                                                        AllegroColor shadyColor = shadyImage.getPixelAt( itemPixel, yy );

                                                        if ( ! rawColor.isKeyColor() && shadyColor.equalsRGB( rawColor ) )
                                                        {
                                                                unsigned char shadyRed = rawColor.getRed() >> pixDiv ;
                                                                unsigned char shadyGreen = rawColor.getGreen() >> pixDiv ;
                                                                unsigned char shadyBlue = rawColor.getBlue() >> pixDiv ;
                                                                shadyImage.putPixelAt( itemPixel, yy, Color( shadyRed, shadyGreen, shadyBlue, rawColor.getAlpha() ) );
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
                // images are crossed within calculated limits
                for ( itemRow = iniY, shadowRow = deltaY ; itemRow < mY ; itemRow ++, shadowRow ++ )
                {
                        // walk thru pixels of each row within calculated limits
                        for ( itemPixel = iniX, shadowPixel = deltaX ; itemPixel < endX ; itemPixel ++, shadowPixel ++ )
                        {
                                AllegroColor shadowColor = shadow.getPixelAt( shadowPixel, shadowRow );
                                AllegroColor itemColor = rawImage.getPixelAt( itemPixel, itemRow );
                                AllegroColor resultColor = shadyImage.getPixelAt( itemPixel, itemRow );

                                // when pixel of shadow isn’t key color & pixel of item isn’t key color
                                // and pixel of result isn’t changed before
                                // then decrement color of result’s pixel to darken it
                                if ( ! shadowColor.isKeyColor() && ! itemColor.isKeyColor() && resultColor.equalsRGB( itemColor ) )
                                {
                                        // there’s some transparency
                                        if ( opacity != 0 )
                                        {
                                                unsigned char resultRed = ( itemColor.getRed() * opacity ) >> 8 ;
                                                unsigned char resultGreen = ( itemColor.getGreen() * opacity ) >> 8 ;
                                                unsigned char resultBlue = ( itemColor.getBlue() * opacity ) >> 8 ;
                                                shadyImage.putPixelAt( itemPixel, itemRow, Color( resultRed, resultGreen, resultBlue, itemColor.getAlpha() ) );
                                        }
                                        // zero opacity is full opacity
                                        else
                                        {
                                                // the pixel of result is pure black
                                                shadyImage.putPixelAt( itemPixel, itemRow, Color::blackColor() );
                                        }
                                }
                        }
                }

                // shade left and right sides of item

                for ( ; itemRow < endY ; itemRow ++, shadowRow ++, ltpx1 += 2, ltpx2 += 2, rtpx1 -= 2, rtpx2 -= 2 )
                {
                        if ( iniX < ltpx1 )
                        {
                                iniX = ltpx1;
                                deltaX = iniX + item.getOffsetX() - x ;
                        }

                        if ( endX > rtpx1 + 2 )
                        {
                                endX = rtpx1 + 2;
                        }

                        for ( itemPixel = iniX, shadowPixel = deltaX ; itemPixel < endX ; itemPixel ++, shadowPixel ++ )
                        {
                                AllegroColor shadowColor = shadow.getPixelAt( shadowPixel, shadowRow );
                                AllegroColor itemColor = rawImage.getPixelAt( itemPixel, itemRow );
                                AllegroColor resultColor = shadyImage.getPixelAt( itemPixel, itemRow );

                                if ( ! shadowColor.isKeyColor() )
                                {
                                        if ( ! itemColor.isKeyColor() && resultColor.equalsRGB( itemColor ) )
                                        {
                                                // there’s some transparency
                                                if ( opacity != 0 )
                                                {
                                                        unsigned char resultRed = ( itemColor.getRed() * opacity ) >> 8 ;
                                                        unsigned char resultGreen = ( itemColor.getGreen() * opacity ) >> 8 ;
                                                        unsigned char resultBlue = ( itemColor.getBlue() * opacity ) >> 8 ;
                                                        shadyImage.putPixelAt( itemPixel, itemRow, Color( resultRed, resultGreen, resultBlue, itemColor.getAlpha() ) );
                                                }
                                                // zero opacity is full opacity
                                                else
                                                {
                                                        // the pixel of result is pure black
                                                        shadyImage.putPixelAt( itemPixel, itemRow, Color::blackColor() );
                                                }
                                        }

                                        if ( itemPixel == ltpx1 || itemPixel == ltpx2 || itemPixel == rtpx1 || itemPixel == rtpx2 )
                                        {
                                                for ( unsigned int yy = itemRow + 1; yy < rawImage.getHeight(); yy++ )
                                                {
                                                        AllegroColor rawColor = rawImage.getPixelAt( itemPixel, yy );
                                                        AllegroColor shadyColor = shadyImage.getPixelAt( itemPixel, yy );

                                                        if ( ! rawColor.isKeyColor() && shadyColor.equalsRGB( rawColor ) )
                                                        {
                                                                // there’s some transparency
                                                                if ( opacity != 0 )
                                                                {
                                                                        unsigned char shadyRed = ( rawColor.getRed() * opacity ) >> 8 ;
                                                                        unsigned char shadyGreen = ( rawColor.getGreen() * opacity ) >> 8 ;
                                                                        unsigned char shadyBlue = ( rawColor.getBlue() * opacity ) >> 8 ;
                                                                        shadyImage.putPixelAt( itemPixel, yy, Color( shadyRed, shadyGreen, shadyBlue, rawColor.getAlpha() ) );
                                                                }
                                                                // zero opacity is full opacity
                                                                else
                                                                {
                                                                        // the pixel of result is pure black
                                                                        shadyImage.putPixelAt( itemPixel, yy, Color::blackColor() );
                                                                }
                                                        }
                                                }
                                        }
                                }
                        }
                }
        }

        shadyImage.getAllegroPict().unlock();
        rawImage.getAllegroPict().unlock();
        shadow.getAllegroPict().unlock();

        /* shadyImage.setName( "+" + shadyImage.getName() ); */

        if ( isFreeItem ) item.freshProcessedImage();
}

/* static */
void ShadowCaster::castShadowOnFloor( FloorTile& tile, int x, int y, const Picture& shadow, unsigned short shading, unsigned char transparency )
{
        if ( shading >= 256 ) return ;

        // fully transparent stuff doesn’t drop any shadow
        if ( transparency >= 100 ) return ;

        const Picture& tileImage = tile.getRawImage() ;

        int iniX = x - tile.getOffsetX();       // initial X
        int iniY = y - tile.getOffsetY();       // initial Y
        int endX = iniX + shadow.getWidth();    // final X
        int endY = iniY + shadow.getHeight();   // final Y
        if ( iniX < 0 ) iniX = 0 ;
        if ( iniY < 0 ) iniY = 0 ;
        if ( endX > static_cast< int >( tileImage.getWidth() ) ) endX = tileImage.getWidth();
        if ( endY > static_cast< int >( tileImage.getHeight() ) ) endY = tileImage.getHeight();

        // continue when initial coordinates are less than final coordinates
        // id est if ( iniY < endY && iniX < endX )
        if ( iniY >= endY || iniX >= endX ) return;

        if ( tile.getWantShadow() )
        {
                // for first or only one shading item begin with fresh image of tile
                ////tile.freshShadyImage ();
                tile.setWantShadow( false );
        }

        Picture& shadyImage = tile.getShadyImage(); // graphics of shaded tile

        shadow.getAllegroPict().lock( true, false );
        tileImage.getAllegroPict().lock( true, false );
        shadyImage.getAllegroPict().lock( true, true );

        int tileRow = 0 ;       // row of pixels in image and shady image of this tile
        int tilePixel = 0 ;     // pixel in row of image & shady image
        int shadowRow = 0 ;     // row of pixels in shadow shading this tile
        int shadowPixel = 0 ;   // pixel in row of shadow shading this tile

        const int deltaX = iniX + tile.getOffsetX() - x ;
        const int deltaY = iniY + tile.getOffsetY() - y ;

        // degree of opacity from 0, full opacity, to 255, full transparency
        short opacity = static_cast< short >( shading + ( ( 256.0 - shading ) * transparency / 100 ) );

        // when opacity is power of 2 in interval [ 2 ... 128 ]
        if ( opacity > 0 && static_cast< int >( std::pow( 2, std::log10( opacity ) / std::log10( 2 ) ) ) == opacity )
        {
                short pixDiv = 7 ; // 2 ^ pixDiv is divisor for shaded pixels

                // tune divisor by opacity of shadow
                while ( opacity != 2 )
                {
                        opacity = opacity >> 1 ;
                        pixDiv -- ;
                }

                // images are crossed within calculated limits
                for ( tileRow = iniY, shadowRow = deltaY ; tileRow < endY ; tileRow ++, shadowRow ++ )
                {
                        // walk thru pixels of each row within calculated limits
                        for ( tilePixel = iniX, shadowPixel = deltaX ; tilePixel < endX ; tilePixel ++, shadowPixel ++ )
                        {
                                AllegroColor shadowColor = shadow.getPixelAt( shadowPixel, shadowRow );
                                AllegroColor tileColor = tileImage.getPixelAt( tilePixel, tileRow );
                                AllegroColor resultColor = shadyImage.getPixelAt( tilePixel, tileRow );

                                // when pixel of shadow isn’t key color & pixel of tile isn’t key color
                                // and pixel of result isn’t changed before
                                // then divide pixel of result by 2 ^ pixDiv, darkening it
                                if ( ! shadowColor.isKeyColor() && ! tileColor.isKeyColor() && resultColor.equalsRGB( tileColor ) )
                                {
                                        unsigned char resultRed = tileColor.getRed() >> pixDiv ;
                                        unsigned char resultGreen = tileColor.getGreen() >> pixDiv ;
                                        unsigned char resultBlue = tileColor.getBlue() >> pixDiv ;
                                        shadyImage.putPixelAt( tilePixel, tileRow, Color( resultRed, resultGreen, resultBlue, tileColor.getAlpha() ) );
                                }
                        }
                }
        }
        // opacity is not power of two
        else
        {
                // images are crossed within calculated limits
                for ( tileRow = iniY, shadowRow = deltaY ; tileRow < endY ; tileRow ++, shadowRow ++ )
                {
                        // walk thru pixels of each row within calculated limits
                        for ( tilePixel = iniX, shadowPixel = deltaX ; tilePixel < endX ; tilePixel ++, shadowPixel ++ )
                        {
                                AllegroColor shadowColor = shadow.getPixelAt( shadowPixel, shadowRow );
                                AllegroColor tileColor = tileImage.getPixelAt( tilePixel, tileRow );
                                AllegroColor resultColor = shadyImage.getPixelAt( tilePixel, tileRow );

                                // when pixel of shadow isn’t key color & pixel of tile isn’t key color
                                // and pixel of result isn’t changed before
                                // then lower color of result’s pixel to darken it
                                if ( ! shadowColor.isKeyColor() && ! tileColor.isKeyColor() && resultColor.equalsRGB( tileColor ) )
                                {
                                        // there’s some transparency
                                        if ( opacity != 0 )
                                        {
                                                unsigned char resultRed = ( tileColor.getRed() * opacity ) >> 8 ;
                                                unsigned char resultGreen = ( tileColor.getGreen() * opacity ) >> 8 ;
                                                unsigned char resultBlue = ( tileColor.getBlue() * opacity ) >> 8 ;
                                                shadyImage.putPixelAt( tilePixel, tileRow, Color( resultRed, resultGreen, resultBlue, tileColor.getAlpha() ) );
                                        }
                                        // zero opacity is full opacity
                                        else
                                        {
                                                // the pixel of result is pure black
                                                shadyImage.putPixelAt( tilePixel, tileRow, Color::blackColor() );
                                        }
                                }
                        }
                }
        }

        shadyImage.getAllegroPict().unlock();
        tileImage.getAllegroPict().unlock();
        shadow.getAllegroPict().unlock();

        /* shadyImage.setName( "+" + shadyImage.getName() ); */
}

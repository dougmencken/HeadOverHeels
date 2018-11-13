
#include "ShadowCaster.hpp"

#include "Color.hpp"
#include "FreeItem.hpp"
#include "GridItem.hpp"

#include <cmath>


namespace iso
{

/* static */
void ShadowCaster::castShadowOnItem( Item& item, int x, int y, const Picture* shadow, unsigned short shading, unsigned char transparency )
{
        if ( shading >= 256 ) return ;

        // fully transparent stuff doesn’t drop any shadow
        if ( transparency >= 100 ) return;

        bool isFreeItem = ( item.whichKindOfItem() == "free item" || item.whichKindOfItem() == "player item" );
        bool isGridItem = ( item.whichKindOfItem() == "grid item" );

        if ( ! isFreeItem && ! isGridItem ) return;

        const Picture* rawImage = item.getRawImage() ;

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
        int endX = iniX + shadow->getWidth();           // final X
        int iniY = y - item.getOffsetY();               // initial Y
        int endY = iniY + shadow->getHeight();          // final Y
        if ( iniX < 0 ) iniX = 0;
        if ( iniY < 0 ) iniY = 0;
        if ( endX > static_cast< int >( rawImage->getWidth() ) ) endX = rawImage->getWidth();
        if ( endY > static_cast< int >( rawImage->getHeight() ) ) endY = rawImage->getHeight();

        int mY = rawImage->getHeight() - width - height;        // intermediate Y
        if ( isGridItem ) mY ++;                                // for better shadow on sides of grid item
        if ( endY < mY ) mY = endY;
        if ( endY > mY + width ) endY = mY + width;

        // continue when initial coordinates are less than final coordinates
        // id est if ( iniY < endY && iniX < endX )
        if ( iniY >= endY || iniX >= endX ) return;

        Picture* shadyImage = nilPointer; // graphics of shaded item

        if ( isFreeItem )
        {
                shadyImage = dynamic_cast< FreeItem& >( item ).getShadedNonmaskedImage();
        }
        else if ( isGridItem )
        {
                shadyImage = item.getProcessedImage();
        }

        if ( shadyImage == nilPointer )
        {
                shadyImage = new Picture( rawImage->getWidth(), rawImage->getHeight() );
                item.setWantShadow( true );
        }

        if ( item.getWantShadow() )
        {
                // initially, image of shaded item is just copy of unshaded item’s image
                allegro::bitBlit( rawImage->getAllegroPict(), shadyImage->getAllegroPict() );
                item.setWantShadow( false );
        }

        shadow->getAllegroPict().lock( true, false );
        rawImage->getAllegroPict().lock( true, false );
        shadyImage->getAllegroPict().lock( true, true );

        int iRow = 0;           // row of pixels in image and shady image of this item
        int iPixel = 0;         // pixel in row of image & shady image
        int sRow = 0;           // row of pixels in shadow shading this item
        int sPixel = 0;         // pixel in row of shadow shading this item

        int deltaX = iniX + item.getOffsetX() - x ;
        int deltaY = iniY + item.getOffsetY() - y ;

        const int iniltpx = ( rawImage->getWidth() >> 1 ) - ( width << 1 ) + deltaW ;
        const int inirtpx = ( rawImage->getWidth() >> 1 ) + ( width << 1 ) + deltaW - 2 ;

        int ltpx1 = iniltpx ;   // first component of first pixel, the one furthest to the left, where shading of sides begins
        int ltpx2 = ltpx1 + 1 ; // second component of first pixel, the one furthest to the left, where shading of sides begins
        int rtpx1 = inirtpx ;   // first component of last pixel, the one furthest to the right, where shading of sides ends
        int rtpx2 = rtpx1 + 1 ; // second component of last pixel, the one furthest to the right, where shading of sides ends

        // degree of opacity from 0, full opacity, to 255, full transparency
        short opacity = static_cast< short >( shading + ( ( 256.0 - shading ) * transparency / 100 ) );

        // when opacity is power of 2 in interval [ 2 ... 128 ]
        if ( opacity > 0 && static_cast< int >( std::pow( 2, std::log10( opacity ) / std::log10( 2 ) ) ) == opacity )
        {
                short pxDiv = 7;  // 2 ^ pxDiv is divisor for shaded pixels

                // tune divisor by opacity of shadow
                while ( opacity != 2 )
                {
                        opacity = opacity >> 1;
                        pxDiv--;
                }

                // shade top surface of item

                // images are crossed within calculated limits
                for ( iRow = iniY, sRow = deltaY ; iRow < mY ; iRow++, sRow++ )
                {
                        // walk thru pixels of each row within calculated limits
                        for ( iPixel = iniX, sPixel = deltaX ; iPixel < endX ; iPixel ++, sPixel ++ )
                        {
                                AllegroColor sColor = shadow->getPixelAt( sPixel, sRow );
                                AllegroColor iColor = rawImage->getPixelAt( iPixel, iRow );
                                AllegroColor rColor = shadyImage->getPixelAt( iPixel, iRow );

                                // when pixel of shadow isn’t key color & pixel of item isn’t key color
                                // and pixel of result isn’t changed before
                                // then divide pixel of result by 2 ^ pxDiv, darkening it
                                if ( ! sColor.isKeyColor() && ! iColor.isKeyColor() && rColor.equalsRGB( iColor ) )
                                {
                                        unsigned char rRed = iColor.getRed() >> pxDiv ;
                                        unsigned char rGreen = iColor.getGreen() >> pxDiv ;
                                        unsigned char rBlue = iColor.getBlue() >> pxDiv ;
                                        shadyImage->putPixelAt( iPixel, iRow, Color( rRed, rGreen, rBlue, iColor.getAlpha() ) );
                                }
                        }
                }
                // so far item is shaded horizontally to line ‘mY’

                // shade left and right sides of item

                for ( ; iRow < endY ; iRow ++, sRow ++, ltpx1 += 2, ltpx2 += 2, rtpx1 -= 2, rtpx2 -= 2 )
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

                        for ( iPixel = iniX, sPixel = deltaX ; iPixel < endX ; iPixel ++, sPixel ++ )
                        {
                                AllegroColor sColor = shadow->getPixelAt( sPixel, sRow );
                                AllegroColor iColor = rawImage->getPixelAt( iPixel, iRow );
                                AllegroColor rColor = shadyImage->getPixelAt( iPixel, iRow );

                                if ( ! sColor.isKeyColor() )
                                {
                                        if ( ! iColor.isKeyColor() && rColor.equalsRGB( iColor ) )
                                        {
                                                unsigned char rRed = iColor.getRed() >> pxDiv ;
                                                unsigned char rGreen = iColor.getGreen() >> pxDiv ;
                                                unsigned char rBlue = iColor.getBlue() >> pxDiv ;
                                                shadyImage->putPixelAt( iPixel, iRow, Color( rRed, rGreen, rBlue, iColor.getAlpha() ) );
                                        }

                                        if ( iPixel == ltpx1 || iPixel == ltpx2 || iPixel == rtpx1 || iPixel == rtpx2 )
                                        {
                                                for ( unsigned int yy = iRow + 1 ; yy < rawImage->getHeight() ; yy++ )
                                                {
                                                        AllegroColor rawColor = rawImage->getPixelAt( iPixel, yy );
                                                        AllegroColor shadyColor = shadyImage->getPixelAt( iPixel, yy );

                                                        if ( ! rawColor.isKeyColor() && shadyColor.equalsRGB( rawColor ) )
                                                        {
                                                                unsigned char shadyRed = rawColor.getRed() >> pxDiv ;
                                                                unsigned char shadyGreen = rawColor.getGreen() >> pxDiv ;
                                                                unsigned char shadyBlue = rawColor.getBlue() >> pxDiv ;
                                                                shadyImage->putPixelAt( iPixel, yy, Color( shadyRed, shadyGreen, shadyBlue, rawColor.getAlpha() ) );
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
                for ( iRow = iniY, sRow = deltaY ; iRow < mY ; iRow++, sRow++ )
                {
                        // walk thru pixels of each row within calculated limits
                        for ( iPixel = iniX, sPixel = deltaX ; iPixel < endX ; iPixel ++, sPixel ++ )
                        {
                                AllegroColor sColor = shadow->getPixelAt( sPixel, sRow );
                                AllegroColor iColor = rawImage->getPixelAt( iPixel, iRow );
                                AllegroColor rColor = shadyImage->getPixelAt( iPixel, iRow );

                                // when pixel of shadow isn’t key color & pixel of item isn’t key color
                                // and pixel of result isn’t changed before
                                // then decrement color of result’s pixel to darken it
                                if ( ! sColor.isKeyColor() && ! iColor.isKeyColor() && rColor.equalsRGB( iColor ) )
                                {
                                        // there’s some transparency
                                        if ( opacity != 0 )
                                        {
                                                unsigned char rRed = ( iColor.getRed() * opacity ) >> 8 ;
                                                unsigned char rGreen = ( iColor.getGreen() * opacity ) >> 8 ;
                                                unsigned char rBlue = ( iColor.getBlue() * opacity ) >> 8 ;
                                                shadyImage->putPixelAt( iPixel, iRow, Color( rRed, rGreen, rBlue, iColor.getAlpha() ) );
                                        }
                                        // zero opacity is full opacity
                                        else
                                        {
                                                // the pixel of result is pure black
                                                shadyImage->putPixelAt( iPixel, iRow, Color::blackColor() );
                                        }
                                }
                        }
                }

                // shade left and right sides of item

                for ( ; iRow < endY ; iRow ++, sRow ++, ltpx1 += 2, ltpx2 += 2, rtpx1 -= 2, rtpx2 -= 2 )
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

                        for ( iPixel = iniX, sPixel = deltaX ; iPixel < endX ; iPixel ++, sPixel ++ )
                        {
                                AllegroColor sColor = shadow->getPixelAt( sPixel, sRow );
                                AllegroColor iColor = rawImage->getPixelAt( iPixel, iRow );
                                AllegroColor rColor = shadyImage->getPixelAt( iPixel, iRow );

                                if ( ! sColor.isKeyColor() )
                                {
                                        if ( ! iColor.isKeyColor() && rColor.equalsRGB( iColor ) )
                                        {
                                                // there’s some transparency
                                                if ( opacity != 0 )
                                                {
                                                        unsigned char rRed = ( iColor.getRed() * opacity ) >> 8 ;
                                                        unsigned char rGreen = ( iColor.getGreen() * opacity ) >> 8 ;
                                                        unsigned char rBlue = ( iColor.getBlue() * opacity ) >> 8 ;
                                                        shadyImage->putPixelAt( iPixel, iRow, Color( rRed, rGreen, rBlue, iColor.getAlpha() ) );
                                                }
                                                // zero opacity is full opacity
                                                else
                                                {
                                                        // the pixel of result is pure black
                                                        shadyImage->putPixelAt( iPixel, iRow, Color::blackColor() );
                                                }
                                        }

                                        if ( iPixel == ltpx1 || iPixel == ltpx2 || iPixel == rtpx1 || iPixel == rtpx2 )
                                        {
                                                for ( unsigned int yy = iRow + 1; yy < rawImage->getHeight(); yy++ )
                                                {
                                                        AllegroColor rawColor = rawImage->getPixelAt( iPixel, yy );
                                                        AllegroColor shadyColor = shadyImage->getPixelAt( iPixel, yy );

                                                        if ( ! rawColor.isKeyColor() && shadyColor.equalsRGB( rawColor ) )
                                                        {
                                                                // there’s some transparency
                                                                if ( opacity != 0 )
                                                                {
                                                                        unsigned char shadyRed = ( rawColor.getRed() * opacity ) >> 8 ;
                                                                        unsigned char shadyGreen = ( rawColor.getGreen() * opacity ) >> 8 ;
                                                                        unsigned char shadyBlue = ( rawColor.getBlue() * opacity ) >> 8 ;
                                                                        shadyImage->putPixelAt( iPixel, yy, Color( shadyRed, shadyGreen, shadyBlue, rawColor.getAlpha() ) );
                                                                }
                                                                // zero opacity is full opacity
                                                                else
                                                                {
                                                                        // the pixel of result is pure black
                                                                        shadyImage->putPixelAt( iPixel, yy, Color::blackColor() );
                                                                }
                                                        }
                                                }
                                        }
                                }
                        }
                }
        }

        shadyImage->getAllegroPict().unlock();
        rawImage->getAllegroPict().unlock();
        shadow->getAllegroPict().unlock();

        if ( isFreeItem )
        {
                dynamic_cast< FreeItem& >( item ).setShadedNonmaskedImage( shadyImage );
        }
        else if ( isGridItem )
        {
                item.setProcessedImage( shadyImage );
        }
}

/* static */
void ShadowCaster::castShadowOnFloor( FloorTile& tile, int x, int y, const Picture& shadow, unsigned short shading, unsigned char transparency )
{
        if ( shading >= 256 ) return ;

        // fully transparent stuff doesn’t drop any shadow
        if ( transparency >= 100 ) return;

        const Picture& tileImage = tile.getRawImage() ;

        int iniX = x - tile.getOffsetX();       // initial X
        int iniY = y - tile.getOffsetY();       // initial Y
        int endX = iniX + shadow.getWidth();    // final X
        int endY = iniY + shadow.getHeight();   // final Y
        if ( iniX < 0 ) iniX = 0;
        if ( iniY < 0 ) iniY = 0;
        if ( endX > static_cast< int >( tileImage.getWidth() ) ) endX = tileImage.getWidth();
        if ( endY > static_cast< int >( tileImage.getHeight() ) ) endY = tileImage.getHeight();

        // continue when initial coordinates are less than final coordinates
        // id est if ( iniY < endY && iniX < endX )
        if ( iniY >= endY || iniX >= endX ) return;

        Picture& shadyImage = tile.getShadyImage(); // graphics of shaded item

        if ( tile.getWantShadow() )
        {
                // when there’s only one shading item, begin with fresh image of tile
                allegro::bitBlit( tileImage.getAllegroPict(), shadyImage.getAllegroPict() );
                tile.setWantShadow( false );
        }

        shadow.getAllegroPict().lock( true, false );
        tileImage.getAllegroPict().lock( true, false );
        shadyImage.getAllegroPict().lock( true, true );

        int iRow = 0;           // row of pixels in image and shady image of this tile
        int iPixel = 0;         // pixel in row of image & shady image
        int sRow = 0;           // row of pixels in shadow shading this tile
        int sPixel = 0;         // pixel in row of shadow shading this tile

        const int deltaX = iniX + tile.getOffsetX() - x ;
        const int deltaY = iniY + tile.getOffsetY() - y ;

        // degree of opacity from 0, full opacity, to 255, full transparency
        short opacity = static_cast< short >( shading + ( ( 256.0 - shading ) * transparency / 100 ) );

        // when opacity is power of 2 in interval [ 2 ... 128 ]
        if ( opacity > 0 && static_cast< int >( std::pow( 2, std::log10( opacity ) / std::log10( 2 ) ) ) == opacity )
        {
                short pxDiv = 7;  // 2 ^ pxDiv is divisor for shaded pixels

                // tune divisor by opacity of shadow
                while ( opacity != 2 )
                {
                        opacity = opacity >> 1;
                        pxDiv--;
                }

                // images are crossed within calculated limits
                for ( iRow = iniY, sRow = deltaY ; iRow < endY ; iRow++, sRow++ )
                {
                        // walk thru pixels of each row within calculated limits
                        for ( iPixel = iniX, sPixel = deltaX ; iPixel < endX ; iPixel ++, sPixel ++ )
                        {
                                AllegroColor sColor = shadow.getPixelAt( sPixel, sRow );
                                AllegroColor iColor = tileImage.getPixelAt( iPixel, iRow );
                                AllegroColor rColor = shadyImage.getPixelAt( iPixel, iRow );

                                // when pixel of shadow isn’t key color & pixel of tile isn’t key color
                                // and pixel of result isn’t changed before
                                // then divide pixel of result by 2 ^ pxDiv, darkening it
                                if ( ! sColor.isKeyColor() && ! iColor.isKeyColor() && rColor.equalsRGB( iColor ) )
                                {
                                        unsigned char rRed = iColor.getRed() >> pxDiv ;
                                        unsigned char rGreen = iColor.getGreen() >> pxDiv ;
                                        unsigned char rBlue = iColor.getBlue() >> pxDiv ;
                                        shadyImage.putPixelAt( iPixel, iRow, Color( rRed, rGreen, rBlue, iColor.getAlpha() ) );
                                }
                        }
                }
        }
        // opacity is not power of two
        else
        {
                // images are crossed within calculated limits
                for ( iRow = iniY, sRow = deltaY ; iRow < endY ; iRow++, sRow++ )
                {
                        // walk thru pixels of each row within calculated limits
                        for ( iPixel = iniX, sPixel = deltaX ; iPixel < endX ; iPixel ++, sPixel ++ )
                        {
                                AllegroColor sColor = shadow.getPixelAt( sPixel, sRow );
                                AllegroColor iColor = tileImage.getPixelAt( iPixel, iRow );
                                AllegroColor rColor = shadyImage.getPixelAt( iPixel, iRow );

                                // when pixel of shadow isn’t key color & pixel of tile isn’t key color
                                // and pixel of result isn’t changed before
                                // then lower color of result’s pixel to darken it
                                if ( ! sColor.isKeyColor() && ! iColor.isKeyColor() && rColor.equalsRGB( iColor ) )
                                {
                                        // there’s some transparency
                                        if ( opacity != 0 )
                                        {
                                                unsigned char rRed = ( iColor.getRed() * opacity ) >> 8 ;
                                                unsigned char rGreen = ( iColor.getGreen() * opacity ) >> 8 ;
                                                unsigned char rBlue = ( iColor.getBlue() * opacity ) >> 8 ;
                                                shadyImage.putPixelAt( iPixel, iRow, Color( rRed, rGreen, rBlue, iColor.getAlpha() ) );
                                        }
                                        // zero opacity is full opacity
                                        else
                                        {
                                                // the pixel of result is pure black
                                                shadyImage.putPixelAt( iPixel, iRow, Color::blackColor() );
                                        }
                                }
                        }
                }
        }

        shadyImage.getAllegroPict().unlock();
        tileImage.getAllegroPict().unlock();
        shadow.getAllegroPict().unlock();
}

}

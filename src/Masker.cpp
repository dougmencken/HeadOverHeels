
#include "Masker.hpp"

#include "Color.hpp"
#include "Picture.hpp"


namespace iso
{

/* static */
void Masker::maskFreeItemBehindItem( FreeItem& itemToMask, const Item& upwardItem )
{
        // mask shaded image or raw image when item is not yet shaded or shadows are off
        const Picture* imageToMask = itemToMask.getShadedNonmaskedImage() ;
        if ( imageToMask == nilPointer ) imageToMask = itemToMask.getRawImage() ;
        if ( imageToMask == nilPointer ) return ;

        const Picture* upwardImage = upwardItem.getRawImage() ;
        if ( upwardImage == nilPointer ) return ;

        int iniX = upwardItem.getOffsetX() - itemToMask.getOffsetX();   // initial X
        int endX = iniX + upwardImage->getWidth();                      // final X
        int iniY = upwardItem.getOffsetY() - itemToMask.getOffsetY();   // initial Y
        int endY = iniY + upwardImage->getHeight();                     // final Y

        if ( iniX < 0 ) iniX = 0;
        if ( iniY < 0 ) iniY = 0;
        if ( endX > static_cast< int >( imageToMask->getWidth() ) ) endX = imageToMask->getWidth();
        if ( endY > static_cast< int >( imageToMask->getHeight() ) ) endY = imageToMask->getHeight();

        Picture* maskedImage = itemToMask.getProcessedImage();

        if ( maskedImage == nilPointer )
        {
                maskedImage = new Picture( imageToMask->getWidth(), imageToMask->getHeight() );
        }

        if ( itemToMask.getWantMask().isTrue() )
        {
                // in principle, image of masked item is image of unmasked item, shaded or not
                allegro::bitBlit( imageToMask->getAllegroPict(), maskedImage->getAllegroPict() );
                itemToMask.setWantMaskIndeterminate();
        }

        const int deltaX = iniX + itemToMask.getOffsetX() - upwardItem.getOffsetX() ;
        const int deltaY = iniY + itemToMask.getOffsetY() - upwardItem.getOffsetY() ;

        int cRow = 0;           // row of pixels in imageToMask
        int cPixel = 0;         // pixel in row of imageToMask
        int iRow = 0;           // row of pixels in upwardImage
        int iPixel = 0;         // pixel in row of upwardImage

        upwardImage->getAllegroPict().lock( true, false );
        maskedImage->getAllegroPict().lock( false, true );

        for ( cRow = iniY, iRow = deltaY ; cRow < endY ; cRow++, iRow++ )
        {
                for ( cPixel = iniX, iPixel = deltaX ; cPixel < endX ; cPixel++, iPixel++ )
                {
                        if ( ! upwardImage->getPixelAt( iPixel, iRow ).isKeyColor() )
                        {
                                maskedImage->putPixelAt( cPixel, cRow, Color() );
                        }
                }
        }

        maskedImage->getAllegroPict().unlock();
        upwardImage->getAllegroPict().unlock();

        itemToMask.setProcessedImage( maskedImage );
}

}

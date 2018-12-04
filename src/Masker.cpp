
#include "Masker.hpp"

#include "Color.hpp"
#include "Picture.hpp"


namespace iso
{

/* static */
void Masker::maskFreeItemBehindItem( FreeItem& itemToMask, const Item& upwardItem )
{
        const Picture& upwardImage = upwardItem.getRawImage() ;

        int iniX = upwardItem.getOffsetX() - itemToMask.getOffsetX();   // initial X
        int endX = iniX + upwardImage.getWidth();                       // final X
        int iniY = upwardItem.getOffsetY() - itemToMask.getOffsetY();   // initial Y
        int endY = iniY + upwardImage.getHeight();                      // final Y

        Picture& maskedImage = itemToMask.getProcessedImage();

        if ( iniX < 0 ) iniX = 0;
        if ( iniY < 0 ) iniY = 0;
        if ( endX > static_cast< int >( maskedImage.getWidth() ) ) endX = maskedImage.getWidth();
        if ( endY > static_cast< int >( maskedImage.getHeight() ) ) endY = maskedImage.getHeight();

        if ( itemToMask.getWantMask().isTrue() )
        {
                // for first or only one masking item begin with fresh image
                itemToMask.freshProcessedImage ();
                itemToMask.setWantMaskIndeterminate ();
        }

        const int deltaX = iniX + itemToMask.getOffsetX() - upwardItem.getOffsetX() ;
        const int deltaY = iniY + itemToMask.getOffsetY() - upwardItem.getOffsetY() ;

        int maskedRow = 0 ;     // row of pixels in maskedImage
        int maskedPixel = 0 ;   // pixel in row of maskedImage
        int upwardRow = 0 ;     // row of pixels in upwardImage
        int upwardPixel = 0 ;   // pixel in row of upwardImage

        upwardImage.getAllegroPict().lock( true, false );
        maskedImage.getAllegroPict().lock( false, true );

        for ( maskedRow = iniY, upwardRow = deltaY ; maskedRow < endY ; maskedRow ++, upwardRow ++ )
        {
                for ( maskedPixel = iniX, upwardPixel = deltaX ; maskedPixel < endX ; maskedPixel ++, upwardPixel ++ )
                {
                        if ( ! upwardImage.getPixelAt( upwardPixel, upwardRow ).isKeyColor() )
                        {
                                maskedImage.putPixelAt( maskedPixel, maskedRow, Color() );
                        }
                }
        }

        maskedImage.getAllegroPict().unlock();
        upwardImage.getAllegroPict().unlock();

        /* maskedImage.setName( "-" + maskedImage.getName() ); */
}

}

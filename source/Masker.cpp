
#include "Masker.hpp"

#include "Color.hpp"
#include "Picture.hpp"


/* static */
void Masker::maskFreeItemBehindItem( FreeItem& itemToMask, const Item& upwardItem )
{
        const Picture& upwardImage = upwardItem.getRawImage() ;

        int iniX = upwardItem.getImageOffsetX() - itemToMask.getImageOffsetX(); // initial X
        int endX = iniX + upwardImage.getWidth();                               // final X
        int iniY = upwardItem.getImageOffsetY() - itemToMask.getImageOffsetY(); // initial Y
        int endY = iniY + upwardImage.getHeight();                              // final Y

        Picture & maskedImage = itemToMask.getProcessedImage() ;

        if ( iniX < 0 ) iniX = 0;
        if ( iniY < 0 ) iniY = 0;
        if ( endX > static_cast< int >( maskedImage.getWidth() ) ) endX = maskedImage.getWidth();
        if ( endY > static_cast< int >( maskedImage.getHeight() ) ) endY = maskedImage.getHeight();

        if ( itemToMask.getWantMask().isTrue() )
        {
                // for the first or only one masking item begin with the fresh image
                itemToMask.freshProcessedImage ();
                itemToMask.setWantMaskIndeterminate ();
        }

        const int deltaX = iniX + itemToMask.getImageOffsetX() - upwardItem.getImageOffsetX() ;
        const int deltaY = iniY + itemToMask.getImageOffsetY() - upwardItem.getImageOffsetY() ;

        int maskedRow = 0 ;     // row of pixels in maskedImage
        int maskedPixel = 0 ;   // pixel in row of maskedImage
        int upwardRow = 0 ;     // row of pixels in upwardImage
        int upwardPixel = 0 ;   // pixel in row of upwardImage

        upwardImage.getAllegroPict().lockReadOnly() ;
        /////maskedImage.getAllegroPict().lockWriteOnly() ;

        for ( maskedRow = iniY, upwardRow = deltaY ; maskedRow < endY ; maskedRow ++, upwardRow ++ )
                for ( maskedPixel = iniX, upwardPixel = deltaX ; maskedPixel < endX ; maskedPixel ++, upwardPixel ++ )
                        if ( ! upwardImage.getPixelAt( upwardPixel, upwardRow ).isKeyColor () )
                                maskedImage.putPixelAt( maskedPixel, maskedRow, Color::keyColor () );

        /////maskedImage.getAllegroPict().unlock() ;
        upwardImage.getAllegroPict().unlock() ;

        /* maskedImage.setName( "-" + maskedImage.getName() ); */
}

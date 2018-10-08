
#include "Color.hpp"
#include "Ism.hpp"

#include <cmath> // for sqrt


Color * Color::theBlack = new Color( 0, 0, 0 ) ;

Color * Color::theDarkBlue = new Color( 0, 0, 127 ) ;

Color * Color::theBlue = new Color( 50, 50, 255 ) ;

Color * Color::theRed = new Color( 255, 50, 50 ) ;

Color * Color::theMagenta = new Color( 255, 50, 255 ) ;

Color * Color::theGreen = new Color( 50, 255, 50 ) ;

Color * Color::theCyan = new Color( 0, 228, 231 ) ;

Color * Color::theYellow = new Color( 255, 255, 50 ) ;

Color * Color::theWhite = new Color( 255, 255, 255 ) ;

Color * Color::theOrange = new Color( 239, 129, 0 ) ;

Color * Color::the50Gray = new Color( 127, 127, 127 ) ;

Color * Color::theTransparency = new Color( 255, 0, 255 ) ;


std::string Color::toString ()
{
        return   "color { r=" + isomot::numberToString( red ) +
                        " g=" + isomot::numberToString( green ) +
                        " b=" + isomot::numberToString( blue ) + " }" ;
}

/* public static */
Picture* Color::colorizePicture( Picture* picture, const Color& color )
{
        if ( color == Color::whiteColor() ) return picture ;

        return colorizePicture( picture, color.getRed (), color.getGreen (), color.getBlue () );
}

/* private static */
Picture* Color::colorizePicture( Picture* picture, unsigned char red, unsigned char green, unsigned char blue )
{
        if ( picture == nilPointer ) return nilPointer ;

        for ( unsigned int x = 0; x < picture->getWidth(); x++ )
        {
                for ( unsigned int y = 0; y < picture->getHeight(); y++ )
                {
                        if ( picture->getPixelAt( x, y ) == allegro::makeColor( 255, 255, 255 ) )
                        {
                                picture->setPixelAt( x, y, Color( red, green, blue ) );
                        }
                }
        }

        return picture ;
}

/* public static */
Picture* Color::multiplyWithColor( Picture* picture, const Color& color )
{
        if ( color == Color::whiteColor() ) return picture ;

        return multiplyWithColor( picture, color.getRed (), color.getGreen (), color.getBlue () );
}

/* private static */
Picture * Color::multiplyWithColor( Picture * picture, unsigned char red, unsigned char green, unsigned char blue )
{
        if ( picture == nilPointer ) return nilPointer ;

        for ( unsigned int x = 0; x < picture->getWidth(); x++ )
        {
                for ( unsigned int y = 0; y < picture->getHeight(); y++ )
                {
                        int color = picture->getPixelAt( x, y );
                        int r = allegro::getRed( color );
                        int g = allegro::getGreen( color );
                        int b = allegro::getBlue( color );

                        // don’t touch pixels with color of transparency
                        if ( r != 255 || g != 0 || b != 255 )
                        {
                                picture->setPixelAt( x, y, Color( r & red, g & green, b & blue ) );
                        }
                }
        }

        return picture ;
}

/* public static */
Picture * Color::pictureToGrayscale ( Picture * picture )
{
        if ( picture == nilPointer ) return nilPointer ;

        for ( unsigned int y = 0; y < picture->getHeight(); y++ )
        {
                for ( unsigned int x = 0; x < picture->getWidth(); x++ )
                {
                        // convert every color but the “ key ” one
                        if ( picture->getPixelAt( x, y ) != allegro::makeColor( 255, 0, 255 ) )
                        {
                                /* imagine color as vector: c { r, g, b }
                                   for each shade of gray r=g=b =w: b { w, w, w }
                                   vector has length: c•c = rr + gg + bb and b•b = ww + ww + ww = 3ww
                                   converted vector has the same length as original:
                                        for the same lengths sqrt ( c•c ) = sqrt ( b•b )
                                        sqrt( rr + gg + bb ) = sqrt( 3 ) * w
                                        w = sqrt( ( rr + gg + bb ) / 3 )
                                */
                                int color = picture->getPixelAt( x, y );
                                double red = static_cast< double >( allegro::getRed( color ) );
                                double green = static_cast< double >( allegro::getGreen( color ) );
                                double blue = static_cast< double >( allegro::getBlue( color ) );
                                double ww = ( red * red + green * green + blue * blue ) / 3.0;
                                unsigned char gray = static_cast< unsigned char >( std::sqrt( ww ) );
                                picture->setPixelAt( x, y, Color( gray, gray, gray ) );
                        }
                }
        }

        return picture ;
}

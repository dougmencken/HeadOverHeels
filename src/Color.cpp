
#include "Color.hpp"
#include "Ism.hpp"

#include <cmath> // for sqrt


const Color * Color::theBlack = new Color( 0, 0, 0, 0xff );

const Color * Color::theDarkBlue = new Color( 0, 0, 127, 0xff ) ;

const Color * Color::theBlue = new Color( 50, 50, 255, 0xff ) ;

const Color * Color::theRed = new Color( 255, 50, 50, 0xff ) ;

const Color * Color::theMagenta = new Color( 255, 50, 255, 0xff ) ;

const Color * Color::theGreen = new Color( 50, 255, 50, 0xff ) ;

const Color * Color::theCyan = new Color( 0, 228, 231, 0xff ) ;

const Color * Color::theYellow = new Color( 255, 255, 50, 0xff ) ;

const Color * Color::theWhite = new Color( 255, 255, 255, 0xff ) ;

const Color * Color::theOrange = new Color( 239, 129, 0, 0xff ) ;

const Color * Color::the50Gray = new Color( 127, 127, 127, 0xff ) ;


Color::Color( )
        : red( AllegroColor::keyColor().getRed() )
        , green( AllegroColor::keyColor().getGreen() )
        , blue( AllegroColor::keyColor().getBlue() )
        , alpha( AllegroColor::keyColor().getAlpha() )
{
}

std::string Color::toString () const
{
        return   "color { r=" + isomot::numberToString( red ) +
                        " g=" + isomot::numberToString( green ) +
                        " b=" + isomot::numberToString( blue ) +
                        " a=" + isomot::numberToString( alpha ) + " }" ;
}

/* public static */
void Color::colorizePicture( Picture* picture, const Color& color )
{
        if ( color == Color::whiteColor() ) return ;

        if ( picture == nilPointer ) return ;

        picture->getAllegroPict().lock( true, true );

        for ( unsigned int y = 0; y < picture->getHeight(); y++ )
        {
                for ( unsigned int x = 0; x < picture->getWidth(); x++ )
                {
                        AllegroColor pixel = picture->getPixelAt( x, y );

                        if ( pixel.getRed() == 255 && pixel.getGreen() == 255 && pixel.getBlue() == 255 )
                        {
                                picture->putPixelAt( x, y, color );
                        }
                }
        }

        picture->getAllegroPict().unlock();
}

/* public static */
void Color::multiplyWithColor( Picture* picture, const Color& color )
{
        if ( color == Color::whiteColor() ) return ;

        multiplyWithColor( picture, color.getRed (), color.getGreen (), color.getBlue () );
}

/* private static */
void Color::multiplyWithColor( Picture * picture, unsigned char red, unsigned char green, unsigned char blue )
{
        if ( picture == nilPointer ) return ;

        picture->getAllegroPict().lock( true, true );

        for ( unsigned int y = 0; y < picture->getHeight(); y++ )
        {
                for ( unsigned int x = 0; x < picture->getWidth(); x++ )
                {
                        AllegroColor color = picture->getPixelAt( x, y );
                        unsigned char r = color.getRed();
                        unsigned char g = color.getGreen();
                        unsigned char b = color.getBlue();

                        // don’t touch pixels with color of transparency
                        if ( ! color.isKeyColor() )
                        {
                                picture->putPixelAt( x, y, Color( r & red, g & green, b & blue, color.getAlpha() ) );
                        }
                }
        }

        picture->getAllegroPict().unlock();
}

/* public static */
void Color::pictureToGrayscale ( Picture * picture )
{
        if ( picture == nilPointer ) return ;

        picture->getAllegroPict().lock( true, true );

        for ( unsigned int y = 0; y < picture->getHeight(); y++ )
        {
                for ( unsigned int x = 0; x < picture->getWidth(); x++ )
                {
                        AllegroColor color = picture->getPixelAt( x, y );

                        // convert every color but the “ key ” one
                        if ( ! color.isKeyColor() )
                        {
                                /* imagine color as vector: c { r, g, b }
                                   for each shade of gray r=g=b =w: b { w, w, w }
                                   vector has length: c•c = rr + gg + bb and b•b = ww + ww + ww = 3ww
                                   converted vector has the same length as original:
                                        for the same lengths sqrt ( c•c ) = sqrt ( b•b )
                                        sqrt( rr + gg + bb ) = sqrt( 3 ) * w
                                        w = sqrt( ( rr + gg + bb ) / 3 )
                                */
                                double red = static_cast< double >( color.getRed() );
                                double green = static_cast< double >( color.getGreen() );
                                double blue = static_cast< double >( color.getBlue() );
                                double ww = ( red * red + green * green + blue * blue ) / 3.0;
                                unsigned char gray = static_cast< unsigned char >( std::sqrt( ww ) );
                                picture->putPixelAt( x, y, Color( gray, gray, gray, color.getAlpha() ) );
                        }
                }
        }

        picture->getAllegroPict().unlock();
}


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


Color::Color( unsigned char r, unsigned char g, unsigned char b )
        : red( r )
        , green( g )
        , blue( b )
{
}

/* public static */
allegro::Pict* Color::colorizePicture( allegro::Pict* picture, const Color* color )
{
        if ( color == Color::whiteColor() ) return picture ;

        return colorizePicture( picture, color->getRed (), color->getGreen (), color->getBlue () );
}

/* private static */
allegro::Pict* Color::colorizePicture( allegro::Pict* picture, unsigned char red, unsigned char green, unsigned char blue )
{
        if ( picture == nilPointer ) return nilPointer ;

        for ( int x = 0; x < picture->w; x++ )
        {
                for ( int y = 0; y < picture->h; y++ )
                {
                        if ( ( ( int* )picture->line[ y ] )[ x ] == makecol( 255, 255, 255 ) )
                        {
                                ( ( int* )picture->line[ y ] )[ x ] = makecol( red, green, blue );
                        }
                }
        }

        return picture ;
}

/* public static */
allegro::Pict* Color::multiplyWithColor( allegro::Pict* picture, const Color* color )
{
        if ( color == Color::whiteColor() ) return picture ;

        return multiplyWithColor( picture, color->getRed (), color->getGreen (), color->getBlue () );
}

/* private static */
allegro::Pict * Color::multiplyWithColor( allegro::Pict * picture, unsigned char red, unsigned char green, unsigned char blue )
{
        if ( picture == nilPointer ) return nilPointer ;

        for ( int x = 0; x < picture->w; x++ )
        {
                for ( int y = 0; y < picture->h; y++ )
                {
                        int color = getpixel( picture, x, y );
                        int r = getr( color );
                        int g = getg( color );
                        int b = getb( color );

                        // don’t touch pixels with color of transparency
                        if ( r != 255 || g != 0 || b != 255 )
                        {
                                ( ( int* )picture->line[ y ] )[ x ] = makecol( r & red, g & green, b & blue );
                        }
                }
        }

        return picture ;
}

/* public static */
allegro::Pict * Color::pictureToGrayscale ( allegro::Pict * picture )
{
        if ( picture == nilPointer ) return nilPointer ;

        for ( int y = 0; y < picture->h; y++ )
        {
                for ( int x = 0; x < picture->w; x++ )
                {
                        // convert every color but the “ key ” one
                        if ( ( ( int* )picture->line[ y ] )[ x ] != makecol( 255, 0, 255 ) )
                        {
                                /* imagine color as vector: c { r, g, b }
                                   for each shade of gray r=g=b =w: b { w, w, w }
                                   vector has length: c•c = rr + gg + bb and b•b = ww + ww + ww = 3ww
                                   converted vector has the same length as original:
                                        for the same lengths sqrt ( c•c ) = sqrt ( b•b )
                                        sqrt( rr + gg + bb ) = sqrt( 3 ) * w
                                        w = sqrt( ( rr + gg + bb ) / 3 )
                                */
                                int color = getpixel( picture, x, y );
                                double red = static_cast< double >( getr( color ) );
                                double green = static_cast< double >( getg( color ) );
                                double blue = static_cast< double >( getb( color ) );
                                double ww = ( red * red + green * green + blue * blue ) / 3.0;
                                unsigned char gray = static_cast< unsigned char >( std::sqrt( ww ) );
                                ( ( int* )picture->line[ y ] )[ x ] = makecol( gray, gray, gray );
                        }
                }
        }

        return picture ;
}

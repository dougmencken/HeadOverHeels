
#include "Color.hpp"

#include <cmath> // for sqrt


Color::Color( unsigned char r, unsigned char g, unsigned char b )
        : red( r )
        , green( g )
        , blue( b )
{
}

/* public static */
BITMAP* Color::colorizePicture( BITMAP* picture, Color* color )
{
        if ( color == Color::whiteColor() ) return picture ;

        return colorizePicture( picture, color->getRed (), color->getGreen (), color->getBlue () );
}

/* private static */
BITMAP* Color::colorizePicture( BITMAP* picture, unsigned char red, unsigned char green, unsigned char blue )
{
        if ( picture == 0 ) return 0 ;

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
BITMAP * Color::pictureToGrayscale ( BITMAP * picture )
{
        if ( picture == 0 ) return 0 ;

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

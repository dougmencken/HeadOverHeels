
#include "Color.hpp"

#include <cmath> // for sqrt


const Color Color::transparency ( AllegroColor::redOfKeyColor, AllegroColor::greenOfKeyColor, AllegroColor::blueOfKeyColor,
                                        AllegroColor::alphaOfKeyColor ) ;

const Color Color::theWhite ( 255, 255, 255, 0xff ) ;

const Color Color::theBlack ( 0, 0, 0, 0xff );

const Color Color::theBlue   ( 0, 0, 255, 0xff ) ;
const Color Color::theRed    ( 255, 0, 0, 0xff ) ;
const Color Color::theMagenta( 255, 1, 255, 0xff ) ;
const Color Color::theGreen  ( 0, 255, 0, 0xff ) ;
const Color Color::theCyan   ( 0, 255, 255, 0xff ) ;
const Color Color::theYellow ( 255, 255, 0, 0xff ) ;

const Color Color::theDarkBlue ( 0, 0, 127, 0xff ) ;

const Color Color::theOrange ( 255, 127, 0, 0xff ) ;

const Color Color::theGray50 ( 127, 127, 127, 0xff ) ;
const Color Color::theGray75white ( 191, 191, 191, 0xff ) ;
const Color Color::theGray25white ( 63, 63, 63, 0xff ) ;

const Color Color::theReducedWhite = Color::theGray75white ;

const Color Color::theReducedBlue ( 0, 0, 191, 0xff ) ;
const Color Color::theReducedRed ( 191, 0, 0, 0xff ) ;
const Color Color::theReducedMagenta ( 191, 0, 191, 0xff ) ;
const Color Color::theReducedGreen ( 0, 191, 0, 0xff ) ;
const Color Color::theReducedCyan ( 0, 191, 191, 0xff ) ;
const Color Color::theReducedYellow ( 191, 191, 0, 0xff ) ;

const Color Color::theLightBlue ( 127, 127, 255, 0xff ) ;
const Color Color::theLightRed ( 255, 127, 127, 0xff ) ;
const Color Color::theLightMagenta ( 255, 127, 255, 0xff ) ;
const Color Color::theLightGreen ( 127, 255, 127, 0xff ) ;
const Color Color::theLightCyan ( 127, 255, 255, 0xff ) ;
const Color Color::theLightYellow ( 255, 255, 127, 0xff ) ;


Color Color::multiply ( const Color & c ) const
{
        return Color( red * ( c.red / 255.0 ), green * ( c.green / 255.0 ), blue * ( c.blue / 255.0 ), alpha ) ;
}

Color Color::withAlteredAlpha ( unsigned char newAlpha ) const
{
        return Color( red, green, blue, newAlpha ) ;
}

std::string Color::toString () const
{
        return   "color { r=" + util::number2string( red ) +
                        " g=" + util::number2string( green ) +
                        " b=" + util::number2string( blue ) +
                        " a=" + util::number2string( alpha ) + " }" ;
}

/* public static */
const Color& Color::byName ( const std::string & color )
{
        if ( color == "white" ) return theWhite ;
        if ( color == "black" ) return theBlack ;

        if ( color == "blue" ) return theBlue ;
        if ( color == "red" ) return theRed ;
        if ( color == "magenta" ) return theMagenta ;
        if ( color == "green" ) return theGreen ;
        if ( color == "cyan" ) return theCyan ;
        if ( color == "yellow" ) return theYellow ;

        if ( color == "dark blue" || color == "darkblue" || color == "blue.dark" ) return theDarkBlue ;

        if ( color == "orange" ) return theOrange ;

        if ( color == "gray" || color == "gray50" || color == "50% gray" || color == "gray 50%" || color == "light black" || color == "black.light" ) return theGray50 ;
        if ( color == "gray75" || color == "gray 75% white" || color == "reduced white" || color == "white.reduced" ) return theGray75white ;
        if ( color == "gray25" || color == "gray 25% white" ) return theGray25white ;

        if ( color == "reduced black" || color == "black.reduced" ) return theBlack ;
        if ( color == "light white" || color == "white.light" ) return theWhite ;

        if ( color == "reduced blue" || color == "blue.reduced" ) return theReducedBlue ;
        if ( color == "reduced red" || color == "red.reduced" ) return theReducedRed ;
        if ( color == "reduced magenta" || color == "magenta.reduced" ) return theReducedMagenta ;
        if ( color == "reduced green" || color == "green.reduced" ) return theReducedGreen ;
        if ( color == "reduced cyan" || color == "cyan.reduced" ) return theReducedCyan ;
        if ( color == "reduced yellow" || color == "yellow.reduced" ) return theReducedYellow ;

        if ( color == "light blue" || color == "blue.light" ) return theLightBlue ;
        if ( color == "light red" || color == "red.light" ) return theLightRed ;
        if ( color == "light magenta" || color == "magenta.light" ) return theLightMagenta ;
        if ( color == "light green" || color == "green.light" ) return theLightGreen ;
        if ( color == "light cyan" || color == "cyan.light" ) return theLightCyan ;
        if ( color == "light yellow" || color == "yellow.light" ) return theLightYellow ;

        std::cout << "unknown color \"" << color << "\" in Color::byName" << std::endl ;
        assert( color == "unbeknown gray" );
        return theGray50 ;
}

/* public static */
void Color::replaceColor( Picture & picture, const Color & from, const Color & to )
{
        if ( to == from ) return ;

        picture.getAllegroPict().lockReadWrite() ;

        for ( unsigned int y = 0 ; y < picture.getHeight() ; y ++ ) {
                for ( unsigned int x = 0 ; x < picture.getWidth() ; x ++ )
                {
                        AllegroColor pixel = picture.getPixelAt( x, y );

                        if ( pixel.getRed() == from.getRed() && pixel.getGreen() == from.getGreen() && pixel.getBlue() == from.getBlue()
                                        && pixel.getAlpha() == from.getAlpha() )
                        {
                                picture.putPixelAt( x, y, to );
                        }
                }
        }

        picture.getAllegroPict().unlock() ;
}

/* public static */
void Color::replaceColorAnyAlpha( Picture & picture, const Color & from, const Color & to )
{
        if ( to == from ) return ;

        picture.getAllegroPict().lockReadWrite() ;

        for ( unsigned int y = 0 ; y < picture.getHeight() ; y ++ ) {
                for ( unsigned int x = 0 ; x < picture.getWidth() ; x ++ )
                {
                        AllegroColor pixel = picture.getPixelAt( x, y );

                        if ( pixel.getRed() == from.getRed() && pixel.getGreen() == from.getGreen() && pixel.getBlue() == from.getBlue() )
                        {
                                picture.putPixelAt( x, y, to );
                        }
                }
        }

        picture.getAllegroPict().unlock() ;
}

/* public static */
void Color::multiplyWithColor( Picture & picture, const Color & color )
{
        if ( color == Color::whiteColor() ) return ;

        multiplyWithColor( picture, color.getRed (), color.getGreen (), color.getBlue () );
}

/* private static */
void Color::multiplyWithColor( Picture & picture, unsigned char red, unsigned char green, unsigned char blue )
{
        picture.getAllegroPict().lockReadWrite() ;

        for ( unsigned int y = 0; y < picture.getHeight(); ++ y ) {
                for ( unsigned int x = 0; x < picture.getWidth(); ++ x )
                {
                        AllegroColor color = picture.getPixelAt( x, y );
                        unsigned char r = color.getRed();
                        unsigned char g = color.getGreen();
                        unsigned char b = color.getBlue();

                        if ( ! color.isKeyColor() ) // don’t touch pixels with the color of transparency
                                picture.putPixelAt( x, y, Color( r * ( red / 255.0 ), g * ( green / 255.0 ), b * ( blue / 255.0 ), color.getAlpha() ) );
                }
        }

        picture.getAllegroPict().unlock() ;
}

/* public static */
void Color::changeAlpha ( Picture & picture, unsigned char newAlpha )
{
        picture.getAllegroPict().lockReadWrite() ;

        for ( unsigned int y = 0; y < picture.getHeight(); ++ y ) {
                for ( unsigned int x = 0; x < picture.getWidth(); ++ x )
                {
                        Color color( picture.getPixelAt( x, y ) );

                        if ( color != Color::keyColor() ) // don’t touch pixels with the color of transparency
                                picture.putPixelAt( x, y, color.withAlteredAlpha( newAlpha ) );
                }
        }

        picture.getAllegroPict().unlock() ;
}

/* public static */
void Color::pictureToGrayscale ( Picture & picture )
{
        picture.getAllegroPict().lockReadWrite() ;

        for ( unsigned int y = 0; y < picture.getHeight(); y++ ) {
                for ( unsigned int x = 0; x < picture.getWidth(); x++ )
                {
                        AllegroColor color = picture.getPixelAt( x, y );

                        // convert every color but the “ key ” one
                        if ( ! color.isKeyColor() )
                        {
                                /* imagine the color as the linear geometric vector c { r, g, b }
                                   this color turns into the shade of gray r=g=b =w with vector b { w, w, w }
                                   the lengths of vectors are c•c = rr + gg + bb and b•b = ww + ww + ww = 3ww
                                   the converted vector has the same length as the original
                                   for the same lengths
                                        sqrt ( c•c ) = sqrt ( b•b )
                                        sqrt( rr + gg + bb ) = sqrt( 3 ) * w
                                        w = sqrt( ( rr + gg + bb ) / 3 )
                                */
                                double red = static_cast< double >( color.getRed() );
                                double green = static_cast< double >( color.getGreen() );
                                double blue = static_cast< double >( color.getBlue() );
                                double ww = ( red * red + green * green + blue * blue ) / 3.0;
                                unsigned char gray = static_cast< unsigned char >( std::sqrt( ww ) );
                                picture.putPixelAt( x, y, Color( gray, gray, gray, color.getAlpha() ) );
                        }
                }
        }

        picture.getAllegroPict().unlock() ;
}

/* public static */
void Color::invertColors( Picture & picture )
{
        picture.getAllegroPict().lockReadWrite() ;

        for ( unsigned int y = 0; y < picture.getHeight(); y++ ) {
                for ( unsigned int x = 0; x < picture.getWidth(); x++ )
                {
                        AllegroColor color = picture.getPixelAt( x, y );

                        if ( ! color.isKeyColor() ) // don’t invert the color of transparency
                                picture.putPixelAt( x, y, Color( 255 - color.getRed(), 255 - color.getGreen(), 255 - color.getBlue(), color.getAlpha() ) );
                }
        }

        picture.getAllegroPict().unlock() ;
}

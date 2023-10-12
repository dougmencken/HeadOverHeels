
#include "Color.hpp"

#include <cmath> // for sqrt


const Color * Color::theWhite = new Color( 255, 255, 255, 0xff ) ;

const Color * Color::theBlack = new Color( 0, 0, 0, 0xff );

const Color * Color::theBlue = new Color( 0, 0, 255, 0xff ) ;
const Color * Color::theRed = new Color( 255, 0, 0, 0xff ) ;
const Color * Color::theMagenta = new Color( 255, 1, 255, 0xff ) ;
const Color * Color::theGreen = new Color( 0, 255, 0, 0xff ) ;
const Color * Color::theCyan = new Color( 0, 255, 255, 0xff ) ;
const Color * Color::theYellow = new Color( 255, 255, 0, 0xff ) ;

const Color * Color::theDarkBlue = new Color( 0, 0, 127, 0xff ) ;

const Color * Color::theOrange = new Color( 255, 127, 0, 0xff ) ;

const Color * Color::the50Gray = new Color( 127, 127, 127, 0xff ) ;
const Color * Color::the75Gray = new Color( 191, 191, 191, 0xff ) ;
const Color * Color::the25Gray = new Color( 63, 63, 63, 0xff ) ;

const Color * Color::theReducedWhite = Color::the75Gray ;

const Color * Color::theReducedBlue = new Color( 0, 0, 191, 0xff ) ;
const Color * Color::theReducedRed = new Color( 191, 0, 0, 0xff ) ;
const Color * Color::theReducedMagenta = new Color( 191, 0, 191, 0xff ) ;
const Color * Color::theReducedGreen = new Color( 0, 191, 0, 0xff ) ;
const Color * Color::theReducedCyan = new Color( 0, 191, 191, 0xff ) ;
const Color * Color::theReducedYellow = new Color( 191, 191, 0, 0xff ) ;

const Color * Color::theLightBlue = new Color( 127, 127, 255, 0xff ) ;
const Color * Color::theLightRed = new Color( 255, 127, 127, 0xff ) ;
const Color * Color::theLightMagenta = new Color( 255, 127, 255, 0xff ) ;
const Color * Color::theLightGreen = new Color( 127, 255, 127, 0xff ) ;
const Color * Color::theLightCyan = new Color( 127, 255, 255, 0xff ) ;
const Color * Color::theLightYellow = new Color( 255, 255, 127, 0xff ) ;


Color::Color( )
        : red( AllegroColor::redOfKeyColor )
        , green( AllegroColor::greenOfKeyColor )
        , blue( AllegroColor::blueOfKeyColor )
        , alpha( AllegroColor::alphaOfKeyColor )
{
}

Color Color::operator * ( const Color & c ) const
{
        return Color( red * ( c.red / 255.0 ), green * ( c.green / 255.0 ), blue * ( c.blue / 255.0 ), alpha ) ;
}

std::string Color::toString () const
{
        return   "color { r=" + util::number2string( red ) +
                        " g=" + util::number2string( green ) +
                        " b=" + util::number2string( blue ) +
                        " a=" + util::number2string( alpha ) + " }" ;
}

/* public static */
const Color& Color::byName ( const std::string& color )
{
        if ( color == "white" ) return *theWhite ;
        if ( color == "black" ) return *theBlack ;

        if ( color == "blue" ) return *theBlue ;
        if ( color == "red" ) return *theRed ;
        if ( color == "magenta" ) return *theMagenta ;
        if ( color == "green" ) return *theGreen ;
        if ( color == "cyan" ) return *theCyan ;
        if ( color == "yellow" ) return *theYellow ;

        if ( color == "dark blue" || color == "darkblue" || color == "blue.dark" ) return *theDarkBlue ;

        if ( color == "orange" ) return *theOrange ;

        if ( color == "gray50" || color == "50% gray" || color == "gray 50%" || color == "gray" || color == "light black" || color == "black.light" ) return *the50Gray ;
        if ( color == "gray75" || color == "75% gray" || color == "gray 75%" || color == "reduced white" || color == "white.reduced" ) return *the75Gray ;
        if ( color == "gray25" || color == "25% gray" || color == "gray 25%" ) return *the25Gray ;

        if ( color == "reduced black" || color == "black.reduced" ) return *theBlack ;
        if ( color == "light white" || color == "white.light" ) return *theWhite ;

        if ( color == "reduced blue" || color == "blue.reduced" ) return *theReducedBlue ;
        if ( color == "reduced red" || color == "red.reduced" ) return *theReducedRed ;
        if ( color == "reduced magenta" || color == "magenta.reduced" ) return *theReducedMagenta ;
        if ( color == "reduced green" || color == "green.reduced" ) return *theReducedGreen ;
        if ( color == "reduced cyan" || color == "cyan.reduced" ) return *theReducedCyan ;
        if ( color == "reduced yellow" || color == "yellow.reduced" ) return *theReducedYellow ;

        if ( color == "light blue" || color == "lightblue" || color == "blue.light" ) return *theLightBlue ;
        if ( color == "light red" || color == "lightred" || color == "red.light" ) return *theLightRed ;
        if ( color == "light magenta" || color == "lightmagenta" || color == "magenta.light" ) return *theLightMagenta ;
        if ( color == "light green" ||  color == "lightgreen" || color == "green.light" ) return *theLightGreen ;
        if ( color == "light cyan" || color == "lightcyan" || color == "cyan.light" ) return *theLightCyan ;
        if ( color == "light yellow" || color == "lightyellow" || color == "yellow.light" ) return *theLightYellow ;

        assert( color == "gray" );
        return *the50Gray ;
}

/* public static */
void Color::replaceColor( Picture& picture, const Color& from, const Color& to )
{
        if ( to == from ) return ;

        picture.getAllegroPict().lock( true, true );

        for ( unsigned int y = 0 ; y < picture.getHeight() ; y ++ )
        {
                for ( unsigned int x = 0 ; x < picture.getWidth() ; x ++ )
                {
                        AllegroColor pixel = picture.getPixelAt( x, y );

                        if ( pixel.getRed() == from.getRed() && pixel.getGreen() == from.getGreen() && pixel.getBlue() == from.getBlue() )
                        {
                                picture.putPixelAt( x, y, to );
                        }
                }
        }

        picture.getAllegroPict().unlock();
}

/* public static */
void Color::multiplyWithColor( Picture& picture, const Color& color )
{
        if ( color == Color::whiteColor() ) return ;

        multiplyWithColor( picture, color.getRed (), color.getGreen (), color.getBlue () );
}

/* private static */
void Color::multiplyWithColor( Picture& picture, unsigned char red, unsigned char green, unsigned char blue )
{
        picture.getAllegroPict().lock( true, true );

        for ( unsigned int y = 0; y < picture.getHeight(); y++ )
        {
                for ( unsigned int x = 0; x < picture.getWidth(); x++ )
                {
                        AllegroColor color = picture.getPixelAt( x, y );
                        unsigned char r = color.getRed();
                        unsigned char g = color.getGreen();
                        unsigned char b = color.getBlue();

                        // don’t touch pixels with color of transparency
                        if ( ! color.isKeyColor() )
                        {
                                picture.putPixelAt( x, y, Color( r * ( red / 255.0 ), g * ( green / 255.0 ), b * ( blue / 255.0 ), color.getAlpha() ) );
                        }
                }
        }

        picture.getAllegroPict().unlock();
}

/* public static */
void Color::pictureToGrayscale ( Picture& picture )
{
        picture.getAllegroPict().lock( true, true );

        for ( unsigned int y = 0; y < picture.getHeight(); y++ )
        {
                for ( unsigned int x = 0; x < picture.getWidth(); x++ )
                {
                        AllegroColor color = picture.getPixelAt( x, y );

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
                                picture.putPixelAt( x, y, Color( gray, gray, gray, color.getAlpha() ) );
                        }
                }
        }

        picture.getAllegroPict().unlock();
}

/* public static */
void Color::invertColors( Picture& picture )
{
        picture.getAllegroPict().lock( true, true );

        for ( unsigned int y = 0; y < picture.getHeight(); y++ )
        {
                for ( unsigned int x = 0; x < picture.getWidth(); x++ )
                {
                        AllegroColor color = picture.getPixelAt( x, y );

                        if ( ! color.isKeyColor() ) // don’t change transparent pixels
                                picture.putPixelAt( x, y, Color( 255 - color.getRed(), 255 - color.getGreen(), 255 - color.getBlue(), color.getAlpha() ) );
                }
        }

        picture.getAllegroPict().unlock();
}

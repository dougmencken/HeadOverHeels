
#include "Picture.hpp"

#include "util.hpp"
#include "ospaths.hpp"

#include <cmath> // for std::sqrt

#ifdef DEBUG
#  define DEBUG_PICTURES        0
#  include <iostream>
#endif


Picture::Picture( unsigned int width, unsigned int height )
        : apicture( allegro::Pict::newPict( width, height ) )
        , name( "Picture." + util::makeRandomString( 12 ) )
{
        fillWithColor( Color::keyColor() );

#if defined( DEBUG_PICTURES )  &&  DEBUG_PICTURES
        std::cout << "created Picture " << getName() << " with width " << width << " and height " << height << std::endl ;
#endif
}

Picture::Picture( unsigned int width, unsigned int height, const Color& color )
        : apicture( allegro::Pict::newPict( width, height ) )
        , name( "Picture." + util::makeRandomString( 12 ) )
{
        fillWithColor( color );

#if defined( DEBUG_PICTURES )  &&  DEBUG_PICTURES
        std::cout << "created Picture " << getName() << " with width " << width << " and height " << height << " filled with " << color.toString () << std::endl ;
#endif
}

Picture::Picture( const allegro::Pict & pict )
        : apicture( allegro::Pict::asCloneOf( pict.ptr() ) )
        , name( "Picture." + util::makeRandomString( 12 ) )
{
#if defined( DEBUG_PICTURES )  &&  DEBUG_PICTURES
        std::cout << "created Picture " << getName() << " as copy of allegro::Pict" << std::endl ;
#endif
}

Picture::Picture( const Picture & pic )
        : apicture( allegro::Pict::asCloneOf( pic.getAllegroPict().ptr() ) )
        , name( "copy of " + pic.name )
{
#if defined( DEBUG_PICTURES )  &&  DEBUG_PICTURES
        std::cout << "created Picture " << getName() << " as copy of Picture" << std::endl ;
#endif
}

Picture::~Picture( )
{
#if defined( DEBUG_PICTURES )  &&  DEBUG_PICTURES
        std::cout << "bye bye " << getName() << std::endl ;
#endif
}

void Picture::setName( const std::string& newName )
{
#if defined( DEBUG_PICTURES )  &&  DEBUG_PICTURES
        std::cout << "rename Picture \"" << getName() << "\" to \"" << newName << "\"" << std::endl ;
#endif

        name = newName ;
}

void Picture::putPixelAt( int x, int y, const Color& color ) const
{
        getAllegroPict().putPixelAt( x, y, color.toAllegroColor() ) ;
}

void Picture::drawPixelAt( int x, int y, const Color& color ) const
{
        getAllegroPict().drawPixelAt( x, y, color.toAllegroColor() ) ;
}

void Picture::fillWithColor( const Color& color )
{
        getAllegroPict().clearToColor( color.toAllegroColor () ) ;
}

void Picture::fillWithTransparencyChequerboard( const unsigned int sizeOfSquare )
{
        const unsigned int sizeOfSquareDoubled = sizeOfSquare << 1 ;

        unsigned int width = getWidth ();
        unsigned int height = getHeight ();

        const Color& white = Color::whiteColor() ;
        const Color& gray75white = Color::byName( "gray 75% white" );

        getAllegroPict().lockReadWrite() ;

        for ( unsigned int y = 0 ; y < height ; y ++ ) {
                for ( unsigned int x = 0 ; x < width ; x ++ )
                {
                        if ( getPixelAt( x, y ).isFullyTransparent () )
                        {
                                Color whichColor = white ;
                                if ( ( ( y % sizeOfSquare ) == ( y % sizeOfSquareDoubled ) && ( x % sizeOfSquare ) != ( x % sizeOfSquareDoubled ) ) ||
                                        ( ( y % sizeOfSquare ) != ( y % sizeOfSquareDoubled ) && ( x % sizeOfSquare ) == ( x % sizeOfSquareDoubled ) ) )
                                {
                                        whichColor = gray75white ;
                                }

                                putPixelAt( x, y, whichColor );
                        }
                }
        }

        getAllegroPict().unlock() ;
}

void Picture::replaceColor( const Color & from, const Color & to )
{
        if ( to == from ) return ;

        getAllegroPict().lockReadWrite() ;

        unsigned int theHeight = getHeight() ;
        unsigned int theWidth = getWidth() ;

        for ( unsigned int y = 0 ; y < theHeight ; y ++ ) {
                for ( unsigned int x = 0 ; x < theWidth ; x ++ )
                {
                        const Color & pixel = getPixelAt( x, y ) ;

                        if ( pixel.getRed() == from.getRed() && pixel.getGreen() == from.getGreen() && pixel.getBlue() == from.getBlue()
                                        && pixel.getAlpha() == from.getAlpha() )
                        {
                                putPixelAt( x, y, to );
                        }
                }
        }

        getAllegroPict().unlock() ;
}

void Picture::replaceColorAnyAlpha( const Color & from, const Color & to )
{
        if ( to == from ) return ;

        getAllegroPict().lockReadWrite() ;

        unsigned int theHeight = getHeight() ;
        unsigned int theWidth = getWidth() ;

        for ( unsigned int y = 0 ; y < theHeight ; y ++ ) {
                for ( unsigned int x = 0 ; x < theWidth ; x ++ )
                {
                        const Color & pixel = getPixelAt( x, y ) ;

                        if ( pixel.getRed() == from.getRed() && pixel.getGreen() == from.getGreen() && pixel.getBlue() == from.getBlue() )
                        {
                                putPixelAt( x, y, to );
                        }
                }
        }

        getAllegroPict().unlock() ;
}

void Picture::toGrayscale()
{
        getAllegroPict().lockReadWrite() ;

        unsigned int theWidth = getWidth() ;
        unsigned int theHeight = getHeight() ;

        for ( unsigned int y = 0 ; y < theHeight ; y++ ) {
                for ( unsigned int x = 0 ; x < theWidth ; x++ )
                {
                        const Color & color = getPixelAt( x, y ) ;

                        // convert every color but the perfect transparency
                        if ( ! color.isFullyTransparent() )
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

                                putPixelAt( x, y, Color( gray, gray, gray, color.getAlpha() ) );
                        }
                }
        }

        getAllegroPict().unlock() ;
}

void Picture::expandOrCropTo( unsigned int width, unsigned int height )
{
        allegro::Pict* resized = allegro::Pict::newPict( width, height );
        resized->clearToColor( AllegroColor::keyColor () ) ;

        allegro::bitBlit( *this->apicture, *resized );
        this->apicture = multiptr< allegro::Pict >( resized );
}

void Picture::flipHorizontal()
{
        unsigned int width = getWidth ();
        unsigned int height = getHeight ();

        allegro::Pict* flipped = allegro::Pict::newPict( width, height );

        getAllegroPict().lockReadOnly() ;
        /////flipped->lockWriteOnly() ;

        for ( unsigned int y = 0 ; y < height ; y ++ )
                for ( unsigned int x = 0 ; x < width ; x ++ )
                        flipped->putPixelAt( width - x - 1, y, getAllegroPict().getPixelAt( x, y ) );

        /////flipped->unlock() ;
        getAllegroPict().unlock() ;

        this->apicture = multiptr< allegro::Pict >( flipped );
}

void Picture::flipVertical()
{
        unsigned int width = getWidth ();
        unsigned int height = getHeight ();

        allegro::Pict* flipped = allegro::Pict::newPict( width, height );

        getAllegroPict().lockReadOnly() ;
        /////flipped->lockWriteOnly() ;

        for ( unsigned int y = 0 ; y < height ; y ++ )
                for ( unsigned int x = 0 ; x < width ; x ++ )
                        flipped->putPixelAt( x, height - y - 1, getAllegroPict().getPixelAt( x, y ) );

        /////flipped->unlock() ;
        getAllegroPict().unlock() ;

        this->apicture = multiptr< allegro::Pict >( flipped );
}

void Picture::rotate90 ()
{
        unsigned int width = getWidth ();
        unsigned int height = getHeight ();

        allegro::Pict* rotated = allegro::Pict::newPict( height, width );

        getAllegroPict().lockReadOnly() ;
        /////rotated->lockWriteOnly() ;

        for ( unsigned int y = 0 ; y < height ; y ++ )
                for ( unsigned int x = 0 ; x < width ; x ++ )
                        rotated->putPixelAt( y, x, getAllegroPict().getPixelAt( x, y ) );

        /////rotated->unlock() ;
        getAllegroPict().unlock() ;

        this->apicture = multiptr< allegro::Pict >( rotated );
}

void Picture::rotate270 ()
{
        unsigned int width = getWidth ();
        unsigned int height = getHeight ();

        allegro::Pict* rotated = allegro::Pict::newPict( height, width );

        getAllegroPict().lockReadOnly() ;
        /////rotated->lockWriteOnly() ;

        for ( unsigned int y = 0 ; y < height ; y ++ )
                for ( unsigned int x = 0 ; x < width ; x ++ )
                        rotated->putPixelAt( height - y - 1, width - x - 1, getAllegroPict().getPixelAt( x, y ) );

        /////rotated->unlock() ;
        getAllegroPict().unlock() ;

        this->apicture = multiptr< allegro::Pict >( rotated );
}

void Picture::multiplyWithColor( const Color & multiplier )
{
        if ( multiplier == Color::whiteColor() ) return ;

        getAllegroPict().lockReadWrite() ;

        unsigned int theWidth = getWidth() ;
        unsigned int theHeight = getHeight() ;

        for ( unsigned int y = 0 ; y < theHeight ; ++ y ) {
                for ( unsigned int x = 0 ; x < theWidth ; ++ x )
                {
                        const Color & color = getPixelAt( x, y ) ;
                        if ( ! color.isFullyTransparent() ) // don’t touch pixels with the color of transparency
                                putPixelAt( x, y, color.multiply( multiplier ) );
                }
        }

        getAllegroPict().unlock() ;
}

void Picture::changeAlpha ( unsigned char newAlpha )
{
        getAllegroPict().lockReadWrite() ;

        unsigned int theHeight = getHeight() ;
        unsigned int theWidth = getWidth() ;

        for ( unsigned int y = 0 ; y < theHeight ; ++ y ) {
                for ( unsigned int x = 0 ; x < theWidth ; ++ x )
                {
                        const Color & color = getPixelAt( x, y ) ;

                        if ( ! color.isFullyTransparent() ) // don’t touch pixels with the color of transparency
                                putPixelAt( x, y, color.withAlteredAlpha( newAlpha ) );
                }
        }

        getAllegroPict().unlock() ;
}

void Picture::invertColors ()
{
        getAllegroPict().lockReadWrite() ;

        unsigned int theHeight = getHeight() ;
        unsigned int theWidth = getWidth() ;

        for ( unsigned int y = 0 ; y < theHeight ; y++ ) {
                for ( unsigned int x = 0 ; x < theWidth ; x++ )
                {
                        const Color & color = getPixelAt( x, y ) ;

                        if ( ! color.isFullyTransparent() ) // don’t invert the color of transparency
                                putPixelAt( x, y, Color( 255 - color.getRed(), 255 - color.getGreen(), 255 - color.getBlue(),
                                                         color.getAlpha() ) );
                }
        }

        getAllegroPict().unlock() ;
}

/* static */
Picture * Picture::summation ( const Picture & first, const Picture & second )
{
        Picture * result = nilPointer ;

        first.getAllegroPict().lockReadWrite() ;
        second.getAllegroPict().lockReadWrite() ;

        unsigned int minWidth = std::min( first.getWidth(), second.getWidth() );
        unsigned int minHeight = std::min( first.getHeight(), second.getHeight() );
        result = new Picture( minWidth, minHeight );

        for ( unsigned int y = 0 ; y < minHeight ; y ++ )
                for ( unsigned int x = 0 ; x < minWidth ; x ++ )
                        result->putPixelAt( x, y, first.getPixelAt( x, y ) + second.getPixelAt( x, y ) );

        first.getAllegroPict().unlock() ;
        second.getAllegroPict().unlock() ;

        return result ;
}

/* static */
Picture * Picture::difference ( const Picture & first, const Picture & second )
{
        Picture * result = nilPointer ;

        first.getAllegroPict().lockReadWrite() ;
        second.getAllegroPict().lockReadWrite() ;

        unsigned int minWidth = std::min( first.getWidth(), second.getWidth() );
        unsigned int minHeight = std::min( first.getHeight(), second.getHeight() );
        result = new Picture( minWidth, minHeight );

        for ( unsigned int y = 0 ; y < minHeight ; y ++ )
                for ( unsigned int x = 0 ; x < minWidth ; x ++ )
                        result->putPixelAt( x, y, first.getPixelAt( x, y ) - second.getPixelAt( x, y ) );

        first.getAllegroPict().unlock() ;
        second.getAllegroPict().unlock() ;

        return result ;
}

void Picture::saveAsPCX( const std::string & path )
{
        allegro::savePictAsPCX( ospaths::pathToFile( path, getName() ), getAllegroPict() );
}

void Picture::saveAsPNG( const std::string & path )
{
        allegro::savePictAsPNG( ospaths::pathToFile( path, getName() ), getAllegroPict() );
}

/* static */
Picture * Picture::loadPicture ( const std::string & pathToPicture )
{
        autouniqueptr< allegro::Pict > pict( allegro::Pict::fromPNGFile( pathToPicture ) );

        if ( pict != nilPointer && pict->isNotNil () )
                return new Picture( *pict ) ;

        IF_DEBUG( std::cout << "can’t load picture \"" << pathToPicture << "\"" << std::endl )
        return nilPointer ;
}

/* static */
std::vector< allegro::Pict * > Picture::loadAnimation ( const std::string & pathToGif )
{
        std::vector< allegro::Pict * > animation ;
        std::vector< int > durations ;
        allegro::loadGIFAnimation( pathToGif, animation, durations );

        return animation ;
}

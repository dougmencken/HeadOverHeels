
#include "Picture.hpp"
#include "Color.hpp"

#include "util.hpp"
#include "ospaths.hpp"

#ifdef DEBUG
#  define DEBUG_PICTURES        0
#  include <iostream>
#endif


Picture::Picture( unsigned int width, unsigned int height )
        : picture( allegro::Pict::newPict( width, height ) )
        , name( "Picture." + util::makeRandomString( 12 ) )
{
        fillWithColor( Color::keyColor() );

#if defined( DEBUG_PICTURES )  &&  DEBUG_PICTURES
        std::cout << "created Picture " << getName() << " with width " << width << " and height " << height << std::endl ;
#endif
}

Picture::Picture( unsigned int width, unsigned int height, const Color& color )
        : picture( allegro::Pict::newPict( width, height ) )
        , name( "Picture." + util::makeRandomString( 12 ) )
{
        fillWithColor( color );

#if defined( DEBUG_PICTURES )  &&  DEBUG_PICTURES
        std::cout << "created Picture " << getName() << " with width " << width << " and height " << height << " filled with " << color.toString () << std::endl ;
#endif
}

Picture::Picture( const allegro::Pict& pict )
        : picture( allegro::Pict::asCloneOf( pict.ptr() ) )
        , name( "Picture." + util::makeRandomString( 12 ) )
{
#if defined( DEBUG_PICTURES )  &&  DEBUG_PICTURES
        std::cout << "created Picture " << getName() << " as copy of allegro::Pict" << std::endl ;
#endif
}

Picture::Picture( const Picture& pic )
        : picture( allegro::Pict::asCloneOf( pic.getAllegroPict().ptr() ) )
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
        picture->putPixelAt( x, y, color.toAllegroColor() ) ;
}

void Picture::drawPixelAt( int x, int y, const Color& color ) const
{
        picture->drawPixelAt( x, y, color.toAllegroColor() ) ;
}

void Picture::fillWithColor( const Color& color )
{
        picture->clearToColor( color.toAllegroColor () ) ;
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
                        if ( getPixelAt( x, y ).isKeyColor() )
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

void Picture::colorizeWhite( const Color& color )
{
        Color::changeWhiteToColor( *this, color );
}

void Picture::colorizeBlack( const Color& color )
{
        Color::changeBlackToColor( *this, color );
}

void Picture::toGrayscale()
{
        Color::pictureToGrayscale( *this );
}

void Picture::expandOrCropTo( unsigned int width, unsigned int height )
{
        allegro::Pict* resized = allegro::Pict::newPict( width, height );
        resized->clearToColor( AllegroColor::keyColor () ) ;

        allegro::bitBlit( *picture, *resized );
        picture = multiptr< allegro::Pict >( resized );
}

void Picture::flipHorizontal()
{
        unsigned int width = getWidth ();
        unsigned int height = getHeight ();

        allegro::Pict* flipped = allegro::Pict::newPict( width, height );

        picture->lockReadOnly() ;
        /////flipped->lockWriteOnly() ;

        for ( unsigned int y = 0 ; y < height ; y ++ )
                for ( unsigned int x = 0 ; x < width ; x ++ )
                        flipped->putPixelAt( width - x - 1, y, picture->getPixelAt( x, y ) );

        /////flipped->unlock() ;
        picture->unlock() ;

        picture = multiptr< allegro::Pict >( flipped );
}

void Picture::flipVertical()
{
        unsigned int width = getWidth ();
        unsigned int height = getHeight ();

        allegro::Pict* flipped = allegro::Pict::newPict( width, height );

        picture->lockReadOnly() ;
        /////flipped->lockWriteOnly() ;

        for ( unsigned int y = 0 ; y < height ; y ++ )
                for ( unsigned int x = 0 ; x < width ; x ++ )
                        flipped->putPixelAt( x, height - y - 1, picture->getPixelAt( x, y ) );

        /////flipped->unlock() ;
        picture->unlock() ;

        picture = multiptr< allegro::Pict >( flipped );
}

void Picture::rotate90 ()
{
        unsigned int width = getWidth ();
        unsigned int height = getHeight ();

        allegro::Pict* rotated = allegro::Pict::newPict( height, width );

        picture->lockReadOnly() ;
        /////rotated->lockWriteOnly() ;

        for ( unsigned int y = 0 ; y < height ; y ++ )
                for ( unsigned int x = 0 ; x < width ; x ++ )
                        rotated->putPixelAt( y, x, picture->getPixelAt( x, y ) );

        /////rotated->unlock() ;
        picture->unlock() ;

        picture = multiptr< allegro::Pict >( rotated );
}

void Picture::rotate270 ()
{
        unsigned int width = getWidth ();
        unsigned int height = getHeight ();

        allegro::Pict* rotated = allegro::Pict::newPict( height, width );

        picture->lockReadOnly() ;
        /////rotated->lockWriteOnly() ;

        for ( unsigned int y = 0 ; y < height ; y ++ )
                for ( unsigned int x = 0 ; x < width ; x ++ )
                        rotated->putPixelAt( height - y - 1, width - x - 1, picture->getPixelAt( x, y ) );

        /////rotated->unlock() ;
        picture->unlock() ;

        picture = multiptr< allegro::Pict >( rotated );
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

        IF_DEBUG( std::cout << "can’t load picture \"" << pictureFile << "\"" << std::endl )
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

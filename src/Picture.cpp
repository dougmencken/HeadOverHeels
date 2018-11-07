
#include "Picture.hpp"
#include "Color.hpp"
#include "Ism.hpp"

#ifdef DEBUG
#  define DEBUG_PICTURES        0
#  include <iostream>
#endif


Picture::Picture( unsigned int width, unsigned int height )
        : picture( allegro::Pict::newPict( width, height ) )
        , name( "Picture." + iso::makeRandomString( 12 ) )
{
        fillWithColor( Color() );

#if defined( DEBUG_PICTURES )  &&  DEBUG_PICTURES
        std::cout << "created Picture " << getName() << " with width " << width << " and height " << height << std::endl ;
#endif
}

Picture::Picture( unsigned int width, unsigned int height, const Color& color )
        : picture( allegro::Pict::newPict( width, height ) )
        , name( "Picture." + iso::makeRandomString( 12 ) )
{
        fillWithColor( color );

#if defined( DEBUG_PICTURES )  &&  DEBUG_PICTURES
        std::cout << "created Picture " << getName() << " with width " << width << " and height " << height << " filled with " << color.toString () << std::endl ;
#endif
}

Picture::Picture( const allegro::Pict& pict )
        : picture( allegro::Pict::asCloneOf( pict.ptr() ) )
        , name( "Picture." + iso::makeRandomString( 12 ) )
{
#if defined( DEBUG_PICTURES )  &&  DEBUG_PICTURES
        std::cout << "created Picture " << getName() << " as copy of const allegro::Pict &" << std::endl ;
#endif
}

Picture::Picture( const Picture& pic )
        : picture( allegro::Pict::asCloneOf( pic.getAllegroPict().ptr() ) )
        , name( "copy of " + pic.name )
{
#if defined( DEBUG_PICTURES )  &&  DEBUG_PICTURES
        std::cout << "created Picture " << getName() << " as copy of const Picture &" << std::endl ;
#endif
}

Picture::~Picture( )
{
#if defined( DEBUG_PICTURES )  &&  DEBUG_PICTURES
        std::cout << "bye bye " << getName() << std::endl ;
#endif

        delete picture ;
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

void Picture::colorize( const Color& color )
{
        Color::colorizePicture( this, color );
}

void Picture::toGrayscale()
{
        Color::pictureToGrayscale( this );
}

void Picture::saveAsPCX( const std::string& path )
{
        allegro::savePictAsPCX( path + getName(), getAllegroPict() );
}

void Picture::saveAsPNG( const std::string& path )
{
        allegro::savePictAsPNG( path + getName(), getAllegroPict() );
}

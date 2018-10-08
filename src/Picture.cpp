
#include "Picture.hpp"
#include "Color.hpp"
#include "Ism.hpp"

#ifdef DEBUG
#  define DEBUG_PICTURES        0
#  include <iostream>
#endif


Picture::Picture( unsigned int width, unsigned int height )
        : picture( allegro::Pict::newPict( width, height ) )
        , name( "Picture." + isomot::makeRandomString( 12 ) )
{
        fillWithColor( Color::colorOfTransparency() );

#if defined( DEBUG_PICTURES )  &&  DEBUG_PICTURES
        std::cout << "created Picture " << getName() << " with width " << width << " and height " << height << std::endl ;
#endif
}

Picture::Picture( unsigned int width, unsigned int height, const Color& color )
        : picture( allegro::Pict::newPict( width, height ) )
        , name( "Picture." + isomot::makeRandomString( 12 ) )
{
        fillWithColor( color );

#if defined( DEBUG_PICTURES )  &&  DEBUG_PICTURES
        std::cout << "created Picture " << getName() << " with width " << width << " and height " << height << " filled with " << color->toString () << std::endl ;
#endif
}

Picture::Picture( const allegro::Pict& pict )
        : picture( allegro::Pict::newPict( pict.getW(), pict.getH(), pict.getColorDepth() ) )
        , name( "Picture." + isomot::makeRandomString( 12 ) )
{
        allegro::bitBlit( pict, getAllegroPict() );

#if defined( DEBUG_PICTURES )  &&  DEBUG_PICTURES
        std::cout << "created Picture " << getName() << " as copy of const allegro::Pict &" << std::endl ;
#endif
}

Picture::Picture( const Picture& pic )
        : picture( allegro::Pict::newPict( pic.getWidth(), pic.getHeight(), pic.getColorDepth() ) )
        , name( "copy of " + pic.name )
{
        allegro::bitBlit( pic.getAllegroPict(), getAllegroPict() );

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

void Picture::setPixelAt( int x, int y, const Color& color ) const
{
        picture->setPixelAt( x, y, color.toAllegroColor() ) ;
}

void Picture::fillWithColor( const Color& color )
{
        picture->clearToColor( color.toAllegroColor () ) ;
}

void Picture::colorize( const Color& color )
{
        Color::colorizePicture( this, color );
}

Picture * Picture::makeGrayscaleCopy ()
{
        Picture* copy = new Picture( *this );
        Color::pictureToGrayscale( copy );
        return copy;
}

Picture * Picture::makeColorizedCopy ( const Color& color )
{
        Picture* copy = new Picture( *this );
        copy->colorize( color );
        return copy;
}

void Picture::saveAsPCX ( const std::string& path )
{
        allegro::savePictAsPCX( path + getName(), getAllegroPict() );
}

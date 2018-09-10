
#include "Picture.hpp"
#include "Color.hpp"
#include "Ism.hpp"

#ifdef DEBUG
#  define DEBUG_PICTURES        0
#  include <iostream>
#endif


Picture::Picture( unsigned int width, unsigned int height )
        : name( "Picture." + isomot::makeRandomString( 12 ) )
{
        picture = allegro::createPicture( width, height );
        fillWithColor( Color::colorOfTransparency() );
}

Picture::Picture( unsigned int width, unsigned int height, Color * color )
        : name( "Picture." + isomot::makeRandomString( 12 ) )
{
        picture = allegro::createPicture( width, height );
        fillWithColor( color );
}

Picture::Picture( allegro::Pict * pict )
        : picture( pict )
        , name( "Picture." + isomot::makeRandomString( 12 ) )
{
}

Picture::Picture( const allegro::Pict& pict )
        : name( "Picture." + isomot::makeRandomString( 12 ) )
{
        picture = allegro::createPicture( pict.w, pict.h, allegro::colorDepthOf( &pict ) );
        allegro::bitBlit( &pict, picture );
}

Picture::Picture( const Picture& pic )
        : name( "copy of " + pic.name )
{
        picture = allegro::createPicture( pic.getWidth(), pic.getHeight(), pic.getColorDepth() );
        allegro::bitBlit( pic.getAllegroPict(), picture );
}

Picture::~Picture( )
{
#if defined( DEBUG_PICTURES )  &&  DEBUG_PICTURES
        std::cout << "bye bye " << getName() << std::endl ;
#endif
        allegro::binPicture( picture );
}

void Picture::fillWithColor( Color * color )
{
        allegro::clearToColor( picture, color->toAllegroColor () );
}

void Picture::colorize( Color * color )
{
        Color::colorizePicture( picture, color );
}

Picture * Picture::makeGrayscaleCopy ()
{
        Picture* copy = new Picture( *this );
        Color::pictureToGrayscale( copy->getAllegroPict() );
        return copy;
}

Picture * Picture::makeColorizedCopy ( Color * color )
{
        Picture* copy = new Picture( *this );
        copy->colorize( color );
        return copy;
}


#include "Picture.hpp"
#include "Ism.hpp"


namespace gui
{

/* static */ BITMAP * Picture::cloneImage( BITMAP * original )
{
        BITMAP * copy = create_bitmap( original->w, original->h );
        blit( original, copy, 0, 0, 0, 0, original->w, original->h );
        return copy;
}

Picture::Picture( int x, int y, BITMAP* image, std::string name ) :
        Widget( x, y )
        , picture( image )
        , nameOfPicture( name )
{

}

Picture::~Picture()
{
        /* std::cout << "forget picture with name \"" << nameOfPicture << "\"" << std::endl ; */

        allegro::destroyBitmap( picture );
}

void Picture::draw( BITMAP* where )
{
        if ( picture != nilPointer )
        {
                allegro::drawSprite( where, picture, getX(), getY() );
        }
}

}

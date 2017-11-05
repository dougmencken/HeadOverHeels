
#include "Picture.hpp"


namespace gui
{

/* static */ BITMAP * Picture::cloneImage( BITMAP * original )
{
        BITMAP * copy = create_bitmap( original->w, original->h );
        blit( original, copy, 0, 0, 0, 0, original->w, original->h );
        return copy;
}

Picture::Picture( int x, int y, BITMAP* image, std::string name ) :
        Widget( x, y ) ,
        picture( image ),
        nameOfPicture( name )
{

}

Picture::~Picture()
{
        /* std::cout << "forget picture with name \"" << nameOfPicture << "\"" << std::endl ; */

        destroy_bitmap( picture );
        picture = 0 ;
}

void Picture::draw( BITMAP* where )
{
        if ( picture != 0 )
        {
                draw_sprite( where, picture, getX(), getY() );
        }
}

}

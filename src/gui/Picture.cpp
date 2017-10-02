
#include "Picture.hpp"


namespace gui
{

Picture::Picture( int x, int y, BITMAP* image, std::string name ) :
        Widget( x, y ) ,
        picture( image ),
        nameOfPicture( name )
{

}

Picture::~Picture()
{
        /* std::cout << "forget picture with name \"" << nameOfPicture << "\"" << std::endl ; */

        delete picture ;
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

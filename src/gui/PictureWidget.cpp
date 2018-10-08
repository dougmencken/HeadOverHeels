
#include "PictureWidget.hpp"
#include "Ism.hpp"


namespace gui
{

PictureWidget::PictureWidget( int x, int y, Picture* image, const std::string& name ) :
        Widget( x, y )
        , picture( image )
        , nameOfPicture( name )
{

}

PictureWidget::~PictureWidget()
{
        delete picture ;
}

void PictureWidget::draw( const allegro::Pict& where )
{
        if ( picture != nilPointer )
        {
                allegro::drawSprite( where, picture->getAllegroPict(), getX(), getY() );
        }
}

}


#include "PictureWidget.hpp"
#include "Ism.hpp"


namespace gui
{

PictureWidget::PictureWidget( int x, int y, const PicturePtr& image, const std::string& name )
        : Widget( x, y )
        , picture( image )
        , nameOfPicture( name )
{

}

void PictureWidget::draw ()
{
        if ( picture != nilPointer )
        {
                allegro::drawSprite( picture->getAllegroPict(), getX(), getY() );
        }
}

}

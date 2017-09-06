
#include "TextField.hpp"
#include "Font.hpp"
#include "Label.hpp"

using gui::TextField;
using gui::Alignment;
using gui::Label;


TextField::TextField( int x, int y, unsigned int width, unsigned int height, const Alignment& alignment )
: Widget( x, y ),
        width( width ),
        height( height ),
        alignment( alignment ),
        heightOfField( 0 )
{

}

TextField::~TextField()
{
        std::for_each( this->lines.begin (), this->lines.end (), DeleteObject() );
        this->lines.clear();
}

void TextField::draw( BITMAP* where )
{
        for ( std::list< Label* >::iterator i = this->lines.begin (); i != this->lines.end (); ++i )
        {
                ( *i )->draw( where );
        }
}

void TextField::addLine( const std::string& text, const std::string& font, const std::string& color )
{
        int posX = 0;

        switch ( this->alignment )
        {
                case gui::LeftAlignment:
                        break;

                case gui::CenterAlignment:
                {
                        std::auto_ptr< Label > label( new Label( text ) );
                        posX = ( this->width - this->getX() - label->getWidth() ) >> 1;
                }
                        break;

                case gui::RightAlignment:
                {
                        std::auto_ptr< Label > label( new Label( text ) );
                        posX = this->width - label->getWidth();
                }
                        break;
        }

        Label* label = new Label( text, font, color );
        label->moveTo( posX, this->getY () + heightOfField );
        this->heightOfField += label->getHeight();

        lines.push_back( label );
}

void TextField::moveTo( int x, int y )
{
        for ( std::list< Label * >::iterator i = this->lines.begin (); i != this->lines.end (); ++i )
        {
                Label* label = ( *i );
                label->moveTo( label->getX() + x - this->getX(), label->getY() + y - this->getY() );
        }

        Widget::moveTo( x, y );
}

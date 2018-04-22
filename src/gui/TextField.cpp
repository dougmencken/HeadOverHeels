
#include "TextField.hpp"
#include "Font.hpp"
#include "Label.hpp"
#include "Ism.hpp"

#include <algorithm> // std::for_each

using gui::TextField;
using gui::Label;


TextField::TextField( unsigned int width, const std::string& align )
        : Widget( )
        , width( width )
        , alignment( align )
        , heightOfField( 0 )
{

}

TextField::~TextField()
{
        std::for_each( this->lines.begin (), this->lines.end (), isomot::DeleteObject() );
        this->lines.clear();
}

void TextField::draw( BITMAP* where )
{
        for ( std::list< Label* >::iterator i = this->lines.begin (); i != this->lines.end (); ++i )
        {
                Label* label = *i ;

                if ( ( label->getY() + static_cast< int >( label->getWidth() ) >= 0 ) &&
                                ( label->getY() < where->h ) )
                {
                        label->draw( where );
                }
        }
}

void TextField::addLine( const std::string& text, const std::string& font, const std::string& color )
{
        Label* label = new Label( text, font, color );

        const int deltaW = static_cast< int >( this->width ) - static_cast< int >( label->getWidth() );
        int posX = 0;

        if ( alignment == "center" )
                posX = deltaW >> 1;
        else if ( alignment == "right" )
                posX = deltaW;

        label->moveTo( posX + this->getX (), this->getY () + heightOfField );
        this->heightOfField += label->getHeight();

        lines.push_back( label );
}

void TextField::setAlignment( const std::string& newAlignment )
{
        for ( std::list< Label * >::iterator i = this->lines.begin (); i != this->lines.end (); ++i )
        {
                Label* label = ( *i );

                int offsetX = 0;

                if ( newAlignment == "center" )
                        offsetX = ( this->width - label->getWidth() ) >> 1;
                else if ( newAlignment == "right" )
                        offsetX = this->width - label->getWidth();

                label->moveTo( offsetX + this->getX (), label->getY () );
        }

        this->alignment = newAlignment;
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

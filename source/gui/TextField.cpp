
#include "TextField.hpp"

#include "Font.hpp"
#include "Label.hpp"
#include "ColorCyclingLabel.hpp"

#include <algorithm> // std::for_each

using gui::TextField ;
using gui::Label ;


TextField::~TextField()
{
        std::for_each( this->lines.begin (), this->lines.end (), DeleteIt() );
        this->lines.clear();
}

void TextField::draw ()
{
        for ( std::vector< Label* >::const_iterator i = this->lines.begin (); i != this->lines.end (); ++ i )
        {
                Label* label = *i ;

                if ( label->getY() + static_cast< int >( label->getHeight() ) < 0 ) continue ;
                if ( label->getY() >= static_cast< int >( allegro::Pict::getWhereToDraw().getH() ) ) continue ;

                label->draw ();
        }
}

void TextField::appendText( const std::string & text, bool height2x, const std::string & color )
{
        bool multicolor = ( color == "multicolor" ) ;
        bool colorcycling = ( color == "cycling" ) ;
        Label* label = colorcycling ? new ColorCyclingLabel( text, height2x )
                                    : new Label( text, new Font( multicolor ? "" : color, height2x ), multicolor ) ;

        const int differenceInWidth = static_cast< int >( this->width ) - static_cast< int >( label->getWidth() );
        int offsetX = 0 ;

        if ( alignment == "center" ) offsetX = differenceInWidth >> 1 ;
        else if ( alignment == "right" ) offsetX = differenceInWidth ;

        label->moveTo( offsetX + this->getX (), this->getY () + this->height );
        this->height += label->getHeight() * interlignePercentage / 100 ;

        lines.push_back( label );
}

/* private */
void TextField::updatePositions ()
{
        this->height = 0 ;

        for ( unsigned int l = 0 ; l < this->lines.size (); ++ l ) {
                Label* label = this->lines[ l ];

                const int differenceInWidth = static_cast< int >( this->width ) - static_cast< int >( label->getWidth() );
                int offsetX = 0 ;

                if ( this->alignment == "center" ) offsetX = differenceInWidth >> 1 ;
                else if ( this->alignment == "right" ) offsetX = differenceInWidth ;

                label->moveTo( offsetX + this->getX (), this->getY () + this->height );
                this->height += label->getHeight() * interlignePercentage / 100 ;
        }
}

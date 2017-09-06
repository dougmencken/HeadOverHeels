
#include "MenuWithMultipleColumns.hpp"

#include "Gui.hpp"
#include "GuiManager.hpp"
#include "InputManager.hpp"
#include "FontManager.hpp"
#include "Label.hpp"
#include "Font.hpp"


namespace gui
{

MenuWithMultipleColumns::MenuWithMultipleColumns( int x, int y, int secondColumnX, unsigned short rows )
: Menu( x, y ),
        secondColumnX( secondColumnX ),
        rows( rows )
{

}

MenuWithMultipleColumns::~MenuWithMultipleColumns( )
{

}

void MenuWithMultipleColumns::draw( BITMAP* where )
{
        if ( where == 0 ) return ;

        refreshPictures ();

        int dx( this->optionImage->w );
        int dy( 0 );
        unsigned short countOfRows = 0;

        if ( activeOption == 0 )
        {
                resetActiveOption ();
        }

        // for each label
        // para cada etiqueta
        for ( std::list< Label* >::iterator i = options.begin (); i != options.end (); ++i, countOfRows++ )
        {
                Label* label = *i;

                // pick a font & color of text
                if ( label == this->activeOption )
                {
                        label->changeFontAndColor( "regular", "orange" );
                }
                else
                {
                        label->changeFontAndColor( "regular", "white" );
                }

                BITMAP* mark = ( activeOption == label ) ? chosenOptionImageMini : optionImage ;

                // place option in first column
                if ( countOfRows <= this->rows )
                {
                        draw_sprite( where, mark, getX (), getY () + dy );

                        label->moveTo( getX () + dx, getY () + dy );
                        label->draw( where );
                }
                // place option in second column
                else
                {
                        // for first option in second column
                        if ( countOfRows == this->rows + 1 )
                        {
                                dy = 0;
                        }

                        // dibuja la viñeta
                        // para cada etiqueta
                        // ( poems, no less )
                        draw_sprite( where, mark, getX () + secondColumnX, getY () + dy );

                        label->moveTo( getX () + dx + this->secondColumnX, getY () + dy );
                        label->draw( where );
                }

                // update vertical offset
                dy += label->getFont()->getCharHeight() - 4;

                // adjust spacing between lines
                dy -= label->getFont()->getCharHeight() >> 5;
        }

        if ( where != this->whereToDraw )
        {
                this->whereToDraw = where;
        }
}

}

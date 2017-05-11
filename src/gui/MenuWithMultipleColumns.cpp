
#include "MenuWithMultipleColumns.hpp"

#include "Gui.hpp"
#include "GuiManager.hpp"
#include "InputManager.hpp"
#include "FontManager.hpp"
#include "Label.hpp"
#include "Font.hpp"


namespace gui
{

MenuWithMultipleColumns::MenuWithMultipleColumns( unsigned int x, unsigned int y, unsigned int secondColumnX, unsigned short rows )
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
        int count = 0;

        if ( activeOption == 0 )
        {
                resetActiveOption ();
        }

        // For each label
        // Para cada etiqueta
        for ( std::list< Label* >::iterator i = options.begin (); i != options.end (); ++i )
        {
                Label* label = *i;

                // Establece el tipo de letra dependiendo de si la opción está o no seleccionada
                if ( label == this->activeOption )
                {
                        label->changeFontAndColor( "regular", "orange" );
                }
                else
                {
                        label->changeFontAndColor( "regular", "white" );
                }

                // Dibuja la opción de menú en un menú de una sola columna o si es un menú de
                // doble columna, dibuja la opción en la primera columna
                if ( count <= this->rows )
                {
                        BITMAP* mark = ( activeOption == label ) ? chosenOptionImageMini : optionImage ;
                        draw_sprite( where, mark, getX (), getY () + dy );

                        // Ajusta la posición de la etiqueta
                        label->changePosition( getX () + dx, getY () + dy );
                        // Dibuja la etiqueta
                        label->draw( where );

                        // Actualiza la diferencia de la altura
                        dy += label->getFont()->getCharHeight() - 4;
                }
                // Dibuja la opción de menú en la segunda columna
                else
                {
                        // Altura inicial de la segunda columna
                        if ( count == this->rows + 1 )
                        {
                                dy = 0;
                        }

                        // Dibuja la viñeta
                        draw_sprite( where, ( label == activeOption ? chosenOptionImageMini : optionImage ),
                                                getX () + secondColumnX, getY () + dy );

                        // Ajusta la posición de la etiqueta
                        label->changePosition( getX () + dx + this->secondColumnX, getY () + dy );

                        // Dibuja la etiqueta
                        label->draw( where );

                        // Actualiza la diferencia de la altura
                        dy += label->getFont()->getCharHeight() - 4;
                }

                // Reducción del interlineado
                dy -= label->getFont()->getCharHeight() >> 5;

                // Si el menú es de doble columna, cuenta el número de filas
                if ( this->secondColumnX != 0 )
                {
                        count++;
                }
        }

        if ( where != this->whereToDraw )
                this->whereToDraw = where;
}

}

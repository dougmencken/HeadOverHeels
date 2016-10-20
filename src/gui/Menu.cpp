
#include "Menu.hpp"
#include "Gui.hpp"
#include "GuiManager.hpp"
#include "InputManager.hpp"
#include "FontManager.hpp"
#include "Label.hpp"
#include "Font.hpp"

namespace gui
{

Menu::Menu( unsigned int x, unsigned int y )
: Widget( x, y ),
        secondColumnX( 0 ),
        rows( 0xffff ),
        activeOption( 0 ),
        optionImage( GuiManager::getInstance()->findImage( "option" ) ),
        selectedOptionImage( GuiManager::getInstance()->findImage( "selected-option" ) ),
        selectedOptionImageMini( GuiManager::getInstance()->findImage( "selected-option-mini" ) )
{

}

Menu::Menu( unsigned int x, unsigned int y, unsigned int secondColumnX, unsigned short rows )
: Widget( x, y ),
        secondColumnX( secondColumnX ),
        rows( rows ),
        activeOption( 0 ),
        optionImage( GuiManager::getInstance()->findImage( "option" ) ),
        selectedOptionImage( GuiManager::getInstance()->findImage( "selected-option" ) ),
        selectedOptionImageMini( GuiManager::getInstance()->findImage( "selected-option-mini" ) )
{

}

Menu::~Menu( )
{
        std::for_each( options.begin (), options.end (), DeleteObject() );
        options.clear();
}

void Menu::draw( BITMAP* where )
{
        int dx( this->optionImage->w );
        int dy( 0 );
        int count = 0;

        // Para cada etiqueta
        for ( std::list< Label* >::iterator i = options.begin (); i != options.end (); ++i )
        {
                Label* label = *i;
                std::string fontName( label->getFontName() );
                std::string color( label->getColor() );

                // Establece el tipo de letra dependiendo de si la opción está o no seleccionada
                if ( label == this->activeOption )
                {
                        if ( this->secondColumnX == 0 )
                        {
                                label->changeFont( "big", label->getColor() );
                        }
                        else
                        {
                                label->changeFont( "regular", "orange" );
                        }
                }
                else
                {
                        label->changeFont( "regular", "white" );
                }

                // Dibuja la opción de menú en un menú de una sola columna o si es un menú de
                // doble columna, dibuja la opción en la primera columna
                if ( count <= this->rows )
                {
                        // Dibuja la viñeta
                        BITMAP* bullet = 0;
                        if ( this->activeOption == label )
                        {
                                bullet = this->secondColumnX == 0 ? this->selectedOptionImage : this->selectedOptionImageMini;
                        }
                        else
                        {
                                bullet = this->optionImage;
                        }
                        draw_sprite( where, bullet, getX (), getY () + dy );

                        // Ajusta la posición de la etiqueta
                        label->changePosition( getX () + dx, getY () + dy );
                        // Dibuja la etiqueta
                        label->draw( where );

                        // Actualiza la diferencia de la altura
                        dy += label->getFont()->getCharHeight() - ( label == this->activeOption && this->secondColumnX == 0 ? 8 : 4 );
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
                        draw_sprite( where,
                                        ( label == this->activeOption ? this->selectedOptionImageMini : this->optionImage ),
                                        getX () + this->secondColumnX, getY () + dy );
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
}

void Menu::handleKey( int key )
{
        switch ( key >> 8 )
        {
                case KEY_UP:
                        this->previousOption();
                        break;

                case KEY_DOWN:
                        this->nextOption();
                        break;

                default:
                        if ( this->getNext() != 0 )
                        {
                                getNext()->handleKey( key );
                        }
        }
}

void Menu::addOption( Label* label )
{
        options.push_back( label );
}

void Menu::addActiveOption( Label* label )
{
        this->addOption( label );
        setNext( label );
        this->activeOption = label;
}

void Menu::previousOption ()
{
        std::list< Label* >::iterator i = std::find_if( options.begin (), options.end (), std::bind2nd( EqualXYOfLabel(), this->activeOption->getXY () ) );
        assert ( i != options.end () );
        this->activeOption = ( i == options.begin() ? *options.rbegin() : *( --i ) );
        setNext( this->activeOption );
}

void Menu::nextOption ()
{
        std::list< Label* >::iterator i = std::find_if( options.begin (), options.end (), std::bind2nd( EqualXYOfLabel(), this->activeOption->getXY () ) );
        assert ( i != options.end () );
        this->activeOption = ( ++i == options.end() ? *options.begin() : *i );
        setNext( this->activeOption );
}

}

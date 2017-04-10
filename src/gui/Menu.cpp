
#include "Menu.hpp"
#include "Gui.hpp"
#include "GuiManager.hpp"
#include "InputManager.hpp"
#include "FontManager.hpp"
#include "Label.hpp"
#include "Font.hpp"


namespace gui
{

Menu::Menu( int x, int y )
: Widget( x, y ),
        handlerOfKeys( 0 ),
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
        if ( where == 0 ) return ;

        if ( activeOption == 0 )
        {
                resetActiveOption ();
        }

        int dx( this->optionImage->w );
        int dy( 0 );

        // For each label
        // Para cada etiqueta
        for ( std::list< Label* >::iterator i = options.begin (); i != options.end (); ++i )
        {
                Label* label = *i;

                // Change the font depending on whether or not the option is active now
                if ( label == this->activeOption )
                {
                        label->changeFontAndColor( "big", label->getColor() );
                }
                else
                {
                        label->changeFontAndColor( "regular", label->getColor() );
                }

                BITMAP* mark = ( this->activeOption == label ) ? this->selectedOptionImage : this->optionImage ;
                draw_sprite( where, mark, getX (), getY () + dy );

                // Ajusta la posición de la etiqueta
                label->changePosition( getX () + dx, getY () + dy );
                // Dibuja la etiqueta
                label->draw( where );

                // Actualiza la diferencia de la altura
                dy += label->getFont()->getCharHeight() - ( label == this->activeOption ? 8 : 4 );

                // Reduce for leading
                dy -= label->getFont()->getCharHeight() >> 5;
        }

        if ( where != this->whereToDraw )
                this->whereToDraw = where;
}

void Menu::redraw ()
{
        draw( this->whereToDraw );
        GuiManager::getInstance()->refresh();
}

void Menu::handleKey( int rawKey )
{
        int theKey = rawKey >> 8;

        if ( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && ( theKey == KEY_F ) )
        {
                gui::GuiManager::getInstance()->toggleFullScreenVideo ();
                return;
        }

        switch ( theKey )
        {
                case KEY_UP:
                        this->previousOption();
                        break;

                case KEY_DOWN:
                        this->nextOption();
                        break;

                default:
                        if ( this->handlerOfKeys != 0 )
                        {
                                handlerOfKeys->handleKey( rawKey );
                        }
        }
}

void Menu::addOption( Label* label )
{
        options.push_back( label );
}

void Menu::setActiveOption ( Label* option )
{
        for ( std::list< Label * >::iterator i = options.begin () ; i != options.end () ; ++i )
        {
                if ( ( *i ) == option )
                {
                        handlerOfKeys = option ;
                        activeOption = option ;
                        return;
                }
        }

        if ( option != 0 )
        {
                fprintf( stderr, "option \"%s\" isn't from this menu\n", option->getText().c_str () );
        }
}

void Menu::resetActiveOption ()
{
        setActiveOption( * options.begin () );
}

void Menu::previousOption ()
{
        std::list< Label* >::iterator i = std::find_if( options.begin (), options.end (), std::bind2nd( EqualXYOfLabel(), this->activeOption->getXY () ) );
        assert ( i != options.end () );
        this->activeOption = ( i == options.begin() ? *options.rbegin() : *( --i ) );
        this->handlerOfKeys = this->activeOption ;
}

void Menu::nextOption ()
{
        std::list< Label* >::iterator i = std::find_if( options.begin (), options.end (), std::bind2nd( EqualXYOfLabel(), this->activeOption->getXY () ) );
        assert ( i != options.end () );
        this->activeOption = ( ++i == options.end() ? *options.begin() : *i );
        this->handlerOfKeys = this->activeOption ;
}

}

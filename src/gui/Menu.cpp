
#include "Menu.hpp"
#include "Gui.hpp"
#include "GuiManager.hpp"
#include "InputManager.hpp"
#include "Label.hpp"
#include "Font.hpp"

#include <algorithm> // std::for_each


namespace gui
{

Menu::Menu( )
        : Widget( ),
        activeOption( nilPointer ),
        whereToDraw( nilPointer ),
        optionImage( nilPointer ),
        chosenOptionImage( nilPointer ),
        chosenOptionImageMini( nilPointer )
{
        refreshPictures ();
}

Menu::~Menu( )
{
        std::for_each( options.begin (), options.end (), isomot::DeleteObject() );
        options.clear();
}

void Menu::refreshPictures ()
{
        if ( optionImage != nilPointer ) destroy_bitmap( optionImage );
        optionImage = load_png( isomot::pathToFile( gui::GuiManager::getInstance()->getPathToPicturesOfGui() + "option.png" ), nilPointer );

        if ( chosenOptionImage != nilPointer ) destroy_bitmap( chosenOptionImage );
        chosenOptionImage = load_png( isomot::pathToFile( gui::GuiManager::getInstance()->getPathToPicturesOfGui() + "chosen-option.png" ), nilPointer );

        if ( chosenOptionImageMini != nilPointer ) destroy_bitmap( chosenOptionImageMini );
        chosenOptionImageMini = load_png( isomot::pathToFile( gui::GuiManager::getInstance()->getPathToPicturesOfGui() + "chosen-option-mini.png" ), nilPointer );
}

void Menu::draw( BITMAP* where )
{
        if ( where == nilPointer ) return ;

        if ( where != this->whereToDraw )
        {
                this->whereToDraw = where;
        }

        if ( activeOption == nilPointer )
        {
                resetActiveOption ();
        }

        // adjust font of every option
        // font for chosen option is double-height
        for ( std::list< Label* >::iterator i = options.begin (); i != options.end (); ++i )
        {
                Label* label = *i;

                if ( label == this->activeOption )
                {
                        if ( label->getFontFamily() != "big" )
                                label->changeFontFamily( "big" );
                }
                else
                {
                        if ( label->getFontFamily() != "regular" )
                                label->changeFontFamily( "regular" );
                }
        }

        // update position of the whole menu to draw it centered
        int previousX = getX (); int previousY = getY ();
        setX( previousX + ( ( isomot::ScreenWidth - previousX ) >> 1 ) - ( getWidthOfMenu () >> 1 ) );
        setY( previousY + ( ( isomot::ScreenHeight - previousY ) >> 1 ) - ( getHeightOfMenu() >> 1 ) );

        int dx( this->optionImage != nilPointer ? this->optionImage->w : 0 );
        int dy( 0 );

        // for each label
        // para cada etiqueta
        for ( std::list< Label* >::iterator i = options.begin (); i != options.end (); ++i )
        {
                Label* label = *i;

                BITMAP * mark = ( this->activeOption == label ) ? this->chosenOptionImage : this->optionImage ;
                if ( mark != nilPointer )
                        draw_sprite( where, mark, getX (), getY () + dy );

                label->moveTo( getX () + dx, getY () + dy );
                label->draw( where );

                // update vertical offset
                dy += label->getHeight() - ( label == this->activeOption ? 8 : 4 );

                // adjust spacing between lines
                dy -= label->getHeight() >> 5;
        }

        // back to initial position of menu
        setX ( previousX );
        setY ( previousY );
}

void Menu::redraw ()
{
        draw( this->whereToDraw );
        GuiManager::getInstance()->redraw();
}

void Menu::handleKey( int rawKey )
{
        int theKey = rawKey >> 8;

        if ( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && ( theKey == KEY_F ) )
        {
                gui::GuiManager::getInstance()->toggleFullScreenVideo ();
                return;
        }

        if ( this->activeOption != nilPointer )
        {
                switch ( theKey )
                {
                        case KEY_UP:
                                this->previousOption();
                                break;

                        case KEY_DOWN:
                                this->nextOption();
                                break;

                        default:
                                activeOption->handleKey( rawKey );
                }
        }
}

void Menu::addOption( Label* label )
{
        if ( label != nilPointer )
                options.push_back( label );
}

void Menu::setActiveOption ( Label* option )
{
        for ( std::list< Label * >::iterator i = options.begin () ; i != options.end () ; ++i )
        {
                if ( ( *i ) == option )
                {
                        activeOption = option ;
                        return;
                }
        }

        if ( option != nilPointer )
        {
                std::cerr << "option \"" << option->getText() << "\" isn’t from this menu" << std::endl ;
        }
}

void Menu::resetActiveOption ()
{
        setActiveOption( * options.begin () );
}

void Menu::setVerticalOffset ( int offset )
{
        setY( offset );
}

unsigned int Menu::getWidthOfMenu () const
{
        unsigned int widthOfMenu = 0;

        for ( std::list< Label * >::const_iterator i = options.begin () ; i != options.end () ; ++i )
        {
                unsigned int theWidth = ( *i )->getWidth() + ( this->optionImage != nilPointer ? this->optionImage->w : 0 ) ;
                if ( theWidth > widthOfMenu ) widthOfMenu = theWidth ;
        }

        return widthOfMenu;
}

unsigned int Menu::getHeightOfMenu () const
{
        unsigned int heightOfMenu = 0;

        for ( std::list< Label * >::const_iterator i = options.begin () ; i != options.end () ; ++i )
        {
                heightOfMenu += ( *i )->getHeight() - 4;
                heightOfMenu -= ( *i )->getHeight() >> 5;
        }

        return ( heightOfMenu >= 4 ) ? ( heightOfMenu - 4 ) : 0; // -4 is for that single active option
}

void Menu::previousOption ()
{
        std::list< Label* >::const_iterator i = std::find_if( options.begin (), options.end (), std::bind2nd( EqualXYOfLabel(), this->activeOption->getXY () ) );
        assert ( i != options.end () );

        this->activeOption = ( i == options.begin() ? *( --options.end() ) : *( --i ) );
}

void Menu::nextOption ()
{
        std::list< Label* >::const_iterator i = std::find_if( options.begin (), options.end (), std::bind2nd( EqualXYOfLabel(), this->activeOption->getXY () ) );
        assert ( i != options.end () );

        ++i ;
        this->activeOption = ( i == options.end() ? *options.begin() : *i );
}

}

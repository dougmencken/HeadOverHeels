
#include "Menu.hpp"
#include "Gui.hpp"
#include "GuiManager.hpp"
#include "InputManager.hpp"
#include "Label.hpp"
#include "Font.hpp"

#include <algorithm> // std::for_each
#include <cassert>


namespace gui
{

Menu::Menu( )
        : Widget( )
        , whereToDraw( nilPointer )
        , activeOption( nilPointer )
        , optionImage( nilPointer )
        , chosenOptionImage( nilPointer )
        , chosenOptionImageMini( nilPointer )
{
        refreshPictures ();
}

Menu::~Menu( )
{
        std::for_each( options.begin (), options.end (), isomot::DeleteIt() );
        options.clear();
}

void Menu::refreshPictures ()
{
        delete optionImage ;
        delete chosenOptionImage ;
        delete chosenOptionImageMini ;

        smartptr< allegro::Pict > optionPict( allegro::loadPNG( isomot::pathToFile( gui::GuiManager::getInstance()->getPathToPicturesOfGui() + "option.png" ) ) );
        optionImage = new Picture( * optionPict.get() );

        smartptr< allegro::Pict > chosenOptionPict( allegro::loadPNG( isomot::pathToFile( gui::GuiManager::getInstance()->getPathToPicturesOfGui() + "chosen-option.png" ) ) );
        chosenOptionImage = new Picture( * chosenOptionPict.get() );

        smartptr< allegro::Pict > chosenOptionMiniPict( allegro::loadPNG( isomot::pathToFile( gui::GuiManager::getInstance()->getPathToPicturesOfGui() + "chosen-option-mini.png" ) ) );
        chosenOptionImageMini = new Picture( * chosenOptionMiniPict.get() );
}

void Menu::draw( const allegro::Pict& where )
{
        if ( ! where.isNotNil() ) return ;

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
                        if ( label->getFontFamily() != "plain" )
                                label->changeFontFamily( "plain" );
                }
        }

        // update position of the whole menu to draw it centered
        int previousX = getX (); int previousY = getY ();
        setX( previousX + ( ( isomot::ScreenWidth() - previousX ) >> 1 ) - ( getWidthOfMenu () >> 1 ) );
        setY( previousY + ( ( isomot::ScreenHeight() - previousY ) >> 1 ) - ( getHeightOfMenu() >> 1 ) );

        int dx( this->optionImage != nilPointer ? this->optionImage->getWidth() : 0 );
        int dy( 0 );

        // for each label
        // para cada etiqueta
        for ( std::list< Label* >::iterator i = options.begin (); i != options.end (); ++i )
        {
                Label* label = *i;

                Picture * mark = ( this->activeOption == label ) ? this->chosenOptionImage : this->optionImage ;
                if ( mark != nilPointer )
                        allegro::drawSprite( where, mark->getAllegroPict(), getX (), getY () + dy );

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
        if ( whereToDraw != nilPointer ) draw( whereToDraw->getAllegroPict() );
        GuiManager::getInstance()->redraw();
}

void Menu::handleKey( const std::string& key )
{
        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "f" ) )
        {
                gui::GuiManager::getInstance()->toggleFullScreenVideo ();
                return;
        }

        if ( this->activeOption != nilPointer )
        {
                if ( key == "Up" )
                        this->previousOption();
                else if ( key == "Down" )
                        this->nextOption();
                else
                        activeOption->handleKey( key );
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
                unsigned int theWidth = ( *i )->getWidth() + ( this->optionImage != nilPointer ? this->optionImage->getWidth() : 0 ) ;
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

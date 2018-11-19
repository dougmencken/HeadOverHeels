
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

/* static */ Picture * Menu::beforeOption = nilPointer ;

/* static */ Picture * Menu::beforeChosenOption = nilPointer ;

/* static */ Picture * Menu::beforeChosenOptionMini = nilPointer ;


Menu::Menu( )
        : Widget( )
        , whereToDraw( )
        , activeOption( nilPointer )
{
        makePicturesForOptions ();
}

Menu::~Menu( )
{
        std::for_each( options.begin (), options.end (), iso::DeleteIt() );
        options.clear();
}

/* static */ void Menu::makePicturesForOptions ()
{
        if ( beforeOption == nilPointer )
        {
                autouniqueptr< allegro::Pict > optionPict( allegro::Pict::fromPNGFile (
                        iso::pathToFile( iso::sharePath(), "menu-option.png" )
                ) );
                assert( optionPict->isNotNil() );

                beforeOption = new Picture( optionPict->getW(), optionPict->getH() );

                autouniqueptr< Picture > blackOption( new Picture( *optionPict.get() ) );
                blackOption->colorize( Color::blackColor() );

                const unsigned int offsetOfTintX = 1;
                const unsigned int offsetOfTintY = 1;
                allegro::bitBlit(
                        blackOption->getAllegroPict(),
                        beforeOption->getAllegroPict(),
                        0, 0,
                        offsetOfTintX, offsetOfTintY,
                        beforeOption->getWidth() - offsetOfTintX, beforeOption->getHeight() - offsetOfTintY
                );

                const allegro::Pict& previousWhere = allegro::Pict::getWhereToDraw() ;
                allegro::Pict::setWhereToDraw( beforeOption->getAllegroPict() );
                allegro::drawSprite( *optionPict.get(), 0, 0 );
                allegro::Pict::setWhereToDraw( previousWhere );

                beforeOption->setName( "picture to show before menu option" );
        }

        if ( beforeChosenOptionMini == nilPointer )
        {
                autouniqueptr< allegro::Pict > chosenOptionMiniPict( allegro::Pict::fromPNGFile (
                        iso::pathToFile( iso::sharePath(), "chosen-menu-option.png" )
                ) );
                assert( chosenOptionMiniPict->isNotNil() );

                beforeChosenOptionMini = new Picture( chosenOptionMiniPict->getW(), chosenOptionMiniPict->getH() );

                autouniqueptr< Picture > blackChosenOptionMini( new Picture( *chosenOptionMiniPict.get() ) );
                blackChosenOptionMini->colorize( Color::blackColor() );

                const unsigned int offsetOfTintX = 1;
                const unsigned int offsetOfTintY = 0;
                allegro::bitBlit(
                        blackChosenOptionMini->getAllegroPict(),
                        beforeChosenOptionMini->getAllegroPict(),
                        0, 0,
                        offsetOfTintX, offsetOfTintY,
                        beforeChosenOptionMini->getWidth() - offsetOfTintX, beforeChosenOptionMini->getHeight() - offsetOfTintY
                );

                const allegro::Pict& previousWhere = allegro::Pict::getWhereToDraw() ;
                allegro::Pict::setWhereToDraw( beforeChosenOptionMini->getAllegroPict() );
                allegro::drawSprite( *chosenOptionMiniPict.get(), 0, 0 );
                allegro::Pict::setWhereToDraw( previousWhere );

                beforeChosenOptionMini->setName( "picture to show before chosen but not double height menu option" );
        }

        if ( beforeChosenOption == nilPointer )
        {
                beforeChosenOption = new Picture( beforeChosenOptionMini->getWidth(), beforeChosenOptionMini->getHeight() << 1 );

                allegro::stretchBlit( beforeChosenOptionMini->getAllegroPict(), beforeChosenOption->getAllegroPict(),
                                        0, 0, beforeChosenOptionMini->getWidth(), beforeChosenOptionMini->getHeight(),
                                        0, 0, beforeChosenOption->getWidth(), beforeChosenOption->getHeight() );

                beforeChosenOption->setName( "picture to show before chosen menu option" );
        }

        beforeChosenOptionMini->colorize( Color::orangeColor() );
}

void Menu::draw ()
{
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
        setX( previousX + ( ( iso::ScreenWidth() - previousX ) >> 1 ) - ( getWidthOfMenu () >> 1 ) );
        setY( previousY + ( ( iso::ScreenHeight() - previousY ) >> 1 ) - ( getHeightOfMenu() >> 1 ) );

        int dx( Menu::beforeOption != nilPointer ? Menu::beforeOption->getWidth() : 0 );
        int dy( 0 );

        // for each label
        // para cada etiqueta
        for ( std::list< Label* >::iterator i = options.begin (); i != options.end (); ++i )
        {
                Label* label = *i;

                Picture * mark = ( this->activeOption == label ) ? Menu::beforeChosenOption : Menu::beforeOption ;
                if ( mark != nilPointer )
                        allegro::drawSprite( mark->getAllegroPict(), getX (), getY () + dy );

                label->moveTo( getX () + dx, getY () + dy );
                label->draw ();

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
        if ( whereToDraw != nilPointer ) drawOn( whereToDraw->getAllegroPict() );
        GuiManager::getInstance().redraw();
}

void Menu::handleKey( const std::string& key )
{
        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "f" ) )
        {
                gui::GuiManager::getInstance().toggleFullScreenVideo ();
                return;
        }

        if ( this->activeOption != nilPointer )
        {
                if ( key == "Up" || key == "q" )
                        this->previousOption();
                else if ( key == "Down" || key == "a" )
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
                unsigned int theWidth = ( *i )->getWidth() + ( Menu::beforeOption != nilPointer ? Menu::beforeOption->getWidth() : 0 ) ;
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

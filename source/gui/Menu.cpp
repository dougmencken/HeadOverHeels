
#include "Menu.hpp"

#include "GuiManager.hpp"
#include "InputManager.hpp"
#include "GamePreferences.hpp"
#include "Label.hpp"
#include "Font.hpp"

#include "ospaths.hpp"

#include <algorithm> // std::for_each
#include <cassert>


namespace gui
{

/* static */ Picture * Menu::pictureBeforeOption = nilPointer ;

/* static */ Picture * Menu::pictureBeforeChosenOption = nilPointer ;

/* static */ Picture * Menu::pictureBeforeChosenOptionSingle = nilPointer ;


Menu::Menu( )
        : Widget( )
        , activeOption( nilPointer )
{
        makePicturesBeforeOptions() ;
}

Menu::~Menu( )
{
        std::for_each( options.begin (), options.end (), DeleteIt() );
        options.clear();
}

/* private */ /* static */
void Menu::makePicturesBeforeOptions ( const int offsetForTintX, const int offsetForTintY )
{
        if ( pictureBeforeOption == nilPointer )
        {
                autouniqueptr< allegro::Pict > optionPict( allegro::Pict::fromPNGFile (
                        ospaths::pathToFile( ospaths::sharePath(), "menu-option.png" )
                ) );
                assert( optionPict->isNotNil() );

                autouniqueptr< Picture > beforeOptionBlack( new Picture( * optionPict ) );

                // white to transparency
                Color::replaceColor( * beforeOptionBlack, Color::whiteColor(), Color::keyColor() );
                // black to white
                autouniqueptr< Picture > beforeOptionWhite( new Picture( * beforeOptionBlack ) );
                Color::replaceColor( * beforeOptionWhite, Color::blackColor(), Color::whiteColor() );

                pictureBeforeOption = new Picture( optionPict->getW(), optionPict->getH() );

                allegro::bitBlit(
                        beforeOptionBlack->getAllegroPict(),
                        pictureBeforeOption->getAllegroPict(),
                        0, 0,
                        offsetForTintX, offsetForTintY,
                        pictureBeforeOption->getWidth() - offsetForTintX, pictureBeforeOption->getHeight() - offsetForTintY
                );

                const allegro::Pict& previousWhere = allegro::Pict::getWhereToDraw() ;
                allegro::Pict::setWhereToDraw( pictureBeforeOption->getAllegroPict() );
                allegro::drawSprite( beforeOptionWhite->getAllegroPict(), 0, 0 );
                allegro::Pict::setWhereToDraw( previousWhere );

                pictureBeforeOption->setName( "the picture to show before a menu option" );
        }

        if ( pictureBeforeChosenOptionSingle == nilPointer )
        {
                autouniqueptr< allegro::Pict > chosenOptionSinglePict( allegro::Pict::fromPNGFile (
                        ospaths::pathToFile( ospaths::sharePath(), "chosen-menu-option.png" )
                ) );
                assert( chosenOptionSinglePict->isNotNil() );

                autouniqueptr< Picture > beforeChosenOptionBlack( new Picture( * chosenOptionSinglePict ) );

                // white to transparency
                Color::replaceColor( * beforeChosenOptionBlack, Color::whiteColor(), Color::keyColor() );
                // black to white
                autouniqueptr< Picture > beforeChosenOptionWhite( new Picture( * beforeChosenOptionBlack ) );
                Color::replaceColor( * beforeChosenOptionWhite, Color::blackColor(), Color::whiteColor() );

                pictureBeforeChosenOptionSingle = new Picture( chosenOptionSinglePict->getW(), chosenOptionSinglePict->getH() );

                const int chosenOptionTintOffsetY = 0 ;
                allegro::bitBlit(
                        beforeChosenOptionBlack->getAllegroPict(),
                        pictureBeforeChosenOptionSingle->getAllegroPict(),
                        0, 0,
                        offsetForTintX, chosenOptionTintOffsetY,
                        pictureBeforeChosenOptionSingle->getWidth() - offsetForTintX,
                                pictureBeforeChosenOptionSingle->getHeight() - chosenOptionTintOffsetY
                );

                const allegro::Pict& previousWhere = allegro::Pict::getWhereToDraw() ;
                allegro::Pict::setWhereToDraw( pictureBeforeChosenOptionSingle->getAllegroPict() );
                allegro::drawSprite( beforeChosenOptionWhite->getAllegroPict(), 0, 0 );
                allegro::Pict::setWhereToDraw( previousWhere );

                pictureBeforeChosenOptionSingle->setName( "the picture to show before a chosen but not double height menu option" );
        }

        if ( pictureBeforeChosenOption == nilPointer )
        {
                pictureBeforeChosenOption = new Picture( pictureBeforeChosenOptionSingle->getWidth(),
                                                                        pictureBeforeChosenOptionSingle->getHeight() << 1 );

                allegro::stretchBlit( pictureBeforeChosenOptionSingle->getAllegroPict(), pictureBeforeChosenOption->getAllegroPict(),
                                        0, 0, pictureBeforeChosenOptionSingle->getWidth(), pictureBeforeChosenOptionSingle->getHeight(),
                                        0, 0, pictureBeforeChosenOption->getWidth(), pictureBeforeChosenOption->getHeight() );

                pictureBeforeChosenOption->setName( "the picture to show before a chosen menu option" );
        }

        pictureBeforeChosenOptionSingle->colorizeWhite( Color::byName( "orange" ) );
}

/* static */
const Picture & Menu::getPictureBeforeOption ()
{
        assert( Menu::pictureBeforeOption != nilPointer );
        return *Menu::pictureBeforeOption ;
}

/* static */
const Picture & Menu::getPictureBeforeChosenOption ()
{
        assert( Menu::pictureBeforeChosenOption != nilPointer );
        return *Menu::pictureBeforeChosenOption ;
}

/* static */
const Picture & Menu::getPictureBeforeChosenOptionSingle ()
{
        assert( Menu::pictureBeforeChosenOptionSingle != nilPointer );
        return *Menu::pictureBeforeChosenOptionSingle ;
}

void Menu::draw ()
{
        if ( activeOption == nilPointer ) resetActiveOption ();

        // adjust font of every option
        // font for chosen option is double-height
        for ( std::vector< Label* >::const_iterator it = options.begin (); it != options.end (); ++ it )
        {
                Label* option = *it ;

                if ( option == getActiveOption() )
                {
                        if ( option->getFontFamily() != "big" )
                                option->changeFontFamily( "big" );
                }
                else
                {
                        if ( option->getFontFamily() != "plain" )
                                option->changeFontFamily( "plain" );
                }
        }

        // update position of the whole menu to draw it centered
        int previousX = getX (); int previousY = getY ();
        setX( previousX + ( ( GamePreferences::getScreenWidth() - previousX ) >> 1 ) - ( getWidthOfMenu () >> 1 ) );
        setY( previousY + ( ( GamePreferences::getScreenHeight() - previousY ) >> 1 ) - ( getHeightOfMenu() >> 1 ) );

        int dx( Menu::getPictureBeforeOption().getWidth () );
        int dy( 0 );

        // for each label
        // para cada etiqueta
        for ( std::vector< Label* >::const_iterator il = options.begin (); il != options.end (); ++ il )
        {
                Label* option = *il ;

                const Picture & mark = ( getActiveOption() == option ) ? Menu::getPictureBeforeChosenOption() : Menu::getPictureBeforeOption() ;
                allegro::drawSprite( mark.getAllegroPict(), getX (), getY () + dy );

                option->moveTo( getX () + dx, getY () + dy );
                option->draw ();

                // update vertical offset
                dy += option->getHeight() - ( option == getActiveOption() ? 8 : 4 );

                // adjust spacing between lines
                dy -= option->getHeight() >> 5 ;
        }

        // back to initial position of menu
        setX ( previousX );
        setY ( previousY );
}

void Menu::redraw ()
{
        GuiManager::getInstance().redraw();
}

void Menu::handleKey( const std::string & key )
{
        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "f" ) )
        {
                gui::GuiManager::getInstance().toggleFullScreenVideo ();
                return;
        }

        if ( getActiveOption() != nilPointer )
        {
                if ( key == "Up" || key == "q" )
                        this->previousOption() ;
                else if ( key == "Down" || key == "a" )
                        this->nextOption() ;
                else
                        activeOption->handleKey( key );
        }
}

void Menu::addOption( Label* label )
{
        if ( label != nilPointer )
                options.push_back( label );
}

void Menu::setActiveOption ( const std::string & textOfOption )
{
        for ( std::vector< Label * >::const_iterator it = options.begin () ; it != options.end () ; ++ it )
        {
                if ( ( *it )->getText() == textOfOption )
                {
                        this->activeOption = *it ;
                        return ;
                }
        }

        std::cerr << "option \"" << textOfOption << "\" isn’t from this menu" << std::endl ;
}

void Menu::resetActiveOption ()
{
        if ( options.size () > 0 )
                setActiveOption( ( * options.begin() )->getText() );
}

void Menu::setVerticalOffset ( int offset )
{
        setY( offset );
}

unsigned int Menu::getWidthOfMenu () const
{
        unsigned int widthOfMenu = 0;

        for ( std::vector< Label * >::const_iterator i = options.begin () ; i != options.end () ; ++i )
        {
                unsigned int theWidth = ( *i )->getWidth() + Menu::getPictureBeforeOption().getWidth() ;
                if ( theWidth > widthOfMenu ) widthOfMenu = theWidth ;
        }

        return widthOfMenu;
}

unsigned int Menu::getHeightOfMenu () const
{
        unsigned int heightOfMenu = 0;

        for ( std::vector< Label * >::const_iterator i = options.begin () ; i != options.end () ; ++i )
        {
                heightOfMenu += ( *i )->getHeight() - 4;
                heightOfMenu -= ( *i )->getHeight() >> 5;
        }

        return ( heightOfMenu >= 4 ) ? ( heightOfMenu - 4 ) : 0; // -4 is for that single active option
}

void Menu::previousOption ()
{
        std::vector < Label * >::const_iterator it = options.begin ();
        for ( ; it != options.end (); ++ it )
        {
                if ( ( *it )->getY () == this->activeOption->getY ()
                        && ( *it )->getX () == this->activeOption->getX () ) break ;
        }
        assert ( it != options.end () );

        this->activeOption = ( it == options.begin() ? *( -- options.end() ) : *( -- it ) );
}

void Menu::nextOption ()
{
        std::vector < Label * >::const_iterator it = options.begin ();
        for ( ; it != options.end (); ++ it )
        {
                if ( ( *it )->getY () == this->activeOption->getY ()
                        && ( *it )->getX () == this->activeOption->getX () ) break ;
        }
        assert ( it != options.end () );

        ++ it ;
        this->activeOption = ( it == options.end() ? *options.begin() : *it );
}

}

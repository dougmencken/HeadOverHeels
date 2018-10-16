
#include "Screen.hpp"
#include "Gui.hpp"
#include "Menu.hpp"
#include "GuiManager.hpp"
#include "Action.hpp"
#include "Label.hpp"
#include "TextField.hpp"
#include "PictureWidget.hpp"
#include "AnimatedPictureWidget.hpp"
#include "Color.hpp"
#include "Picture.hpp"

#include <iostream>
//#include <algorithm> // std::for_each


const double delayBetweenFrames = 0.1 ;


namespace gui
{

/* static */ Picture * Screen::backgroundPicture = nilPointer ;

/* static */ void Screen::refreshBackground ()
{
        delete Screen::backgroundPicture ;

        Screen::backgroundPicture = Screen::loadPicture( "background.png" );
        Screen::backgroundPicture->setName( "background for slides of user interface" );
}


Screen::Screen( Action* action ) :
        Widget( 0, 0 ),
        imageOfScreen( nilPointer ),
        actionOfScreen( action ),
        escapeAction( nilPointer ),
        keyHandler( nilPointer ),
        pictureOfHead( nilPointer ),
        pictureOfHeels( nilPointer )
{
        if ( action != nilPointer && action->getWhereToDraw() != nilPointer )
        {
                imageOfScreen = action->getWhereToDraw() ;
                imageOfScreen->setName( "image of screen for action " + action->getNameOfAction() );

                refreshPicturesOfHeadAndHeels ();
        }
}

Screen::~Screen( )
{
        freeWidgets() ;
}

void Screen::setEscapeAction ( Action * action )
{
        delete escapeAction ;
        escapeAction = action ;
}

void Screen::refreshPicturesOfHeadAndHeels ()
{
        bool headOnScreen = false;

        if ( pictureOfHead != nilPointer )
        {
                int xHead = pictureOfHead->getX ();
                int yHead = pictureOfHead->getY ();

                if ( pictureOfHead->isOnScreen() )
                {
                        headOnScreen = true;
                        removeWidget( pictureOfHead );
                }
                else
                        delete pictureOfHead ;

                pictureOfHead = new AnimatedPictureWidget( xHead, yHead, loadAnimation( "head.gif" ), delayBetweenFrames, "animated Head" );
        }

        if ( headOnScreen && pictureOfHead != nilPointer )
                addWidget( pictureOfHead );

        bool heelsOnScreen = false;

        if ( pictureOfHeels != nilPointer )
        {
                int xHeels = pictureOfHeels->getX ();
                int yHeels = pictureOfHeels->getY ();

                if ( pictureOfHeels->isOnScreen() )
                {
                        heelsOnScreen = true;
                        removeWidget( pictureOfHeels );
                }
                else
                        delete pictureOfHeels ;

                pictureOfHeels = new AnimatedPictureWidget( xHeels, yHeels, loadAnimation( "heels.gif" ), delayBetweenFrames, "animated Heels" );
        }

        if ( heelsOnScreen && pictureOfHeels != nilPointer )
        {
                addWidget( pictureOfHeels );
        }
}

void Screen::draw( const allegro::Pict& where )
{
        if ( ! imageOfScreen->getAllegroPict().equals( where ) )
        {
                std::cout << "change image to draw gui.Screen for \"" <<
                        ( actionOfScreen != nilPointer ? actionOfScreen->getNameOfAction() : "nil" ) <<
                        "\" action" << std::endl ;

                delete this->imageOfScreen ;
                this->imageOfScreen = new Picture( where );
        }

        redraw();
        drawOnGlobalScreen( );
}

void Screen::redraw( )
{
        if ( imageOfScreen == nilPointer ) return ;

        imageOfScreen->fillWithColor( Color::redColor() );

        // draw background, if any

        if ( Screen::backgroundPicture != nilPointer )
        {
                unsigned int backgroundWidth = backgroundPicture->getWidth();
                unsigned int backgroundHeight = backgroundPicture->getHeight();

                if ( backgroundWidth == isomot::ScreenWidth() && backgroundHeight == isomot::ScreenHeight() )
                {
                        allegro::bitBlit( backgroundPicture->getAllegroPict(), imageOfScreen->getAllegroPict() );
                }
                else
                {
                        double ratioX = static_cast< double >( isomot::ScreenWidth() ) / static_cast< double >( backgroundWidth ) ;
                        double ratioY = static_cast< double >( isomot::ScreenHeight() ) / static_cast< double >( backgroundHeight ) ;

                        if ( ratioX == ratioY )
                        {
                                allegro::stretchBlit( backgroundPicture->getAllegroPict(), imageOfScreen->getAllegroPict(),
                                                0, 0, backgroundWidth, backgroundHeight,
                                                0, 0, isomot::ScreenWidth(), isomot::ScreenHeight() );
                        }
                        else if ( ratioX > ratioY ) /* horizontal over~stretching */
                        {
                                unsigned int proportionalWidth = static_cast< unsigned int >( backgroundWidth * ratioY );
                                unsigned int offsetX = ( isomot::ScreenWidth() - proportionalWidth ) >> 1;

                                imageOfScreen->fillWithColor( Color::blackColor() );

                                allegro::stretchBlit( backgroundPicture->getAllegroPict(), imageOfScreen->getAllegroPict(),
                                                0, 0, backgroundWidth, backgroundHeight,
                                                offsetX, 0, proportionalWidth, isomot::ScreenHeight() );
                        }
                        else /* if ( ratioY > ratioX ) */ /* vertical over~stretching */
                        {
                                unsigned int proportionalHeight = static_cast< unsigned int >( backgroundHeight * ratioX );
                                unsigned int offsetY = ( isomot::ScreenHeight() - proportionalHeight ) >> 1;

                                imageOfScreen->fillWithColor( Color::blackColor() );

                                allegro::stretchBlit( backgroundPicture->getAllegroPict(), imageOfScreen->getAllegroPict(),
                                                0, 0, backgroundWidth, backgroundHeight,
                                                0, offsetY, isomot::ScreenWidth(), proportionalHeight );
                        }
                }
        }

        // draw each component

        for ( std::list< Widget * >::iterator it = widgets.begin () ; it != widgets.end () ; ++it )
        {
                ( *it )->draw( imageOfScreen->getAllegroPict() );
        }
}

void Screen::drawOnGlobalScreen( )
{
        // copy resulting image to screen
        allegro::bitBlit( imageOfScreen->getAllegroPict(), allegro::Pict::theScreen() );
}

void Screen::handleKey( const std::string& key )
{
        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "f" ) )
        {
                gui::GuiManager::getInstance()->toggleFullScreenVideo ();
                return;
        }

        if ( escapeAction != nilPointer && key == "Escape" )
        {
                fprintf( stdout, "escape action %s ~~\n", ( escapeAction != nilPointer ? escapeAction->getNameOfAction().c_str () : "nope" ) );
                this->escapeAction->doIt ();
                fprintf( stdout, "~~ done with action %s\n", ( escapeAction != nilPointer ? escapeAction->getNameOfAction().c_str () : "nope" ) );
        }
        else
        {
                if ( this->keyHandler != nilPointer )
                {
                        this->keyHandler->handleKey( key );
                }
        }
}

void Screen::addWidget( Widget* widget )
{
        if ( widget == nilPointer ) return ;

        this->widgets.push_back( widget );
        widget->setOnScreen( true );
        if ( widget->isMenu() ) dynamic_cast< gui::Menu* >( widget )->setWhereToDraw( imageOfScreen );
}

bool Screen::removeWidget( Widget* widget )
{
        for ( std::list< Widget* >::const_iterator it = widgets.begin (); it != widgets.end (); ++ it )
        {
                if ( ( *it ) == widget )
                {
                        widgets.remove( widget );
                        delete widget;

                        return true;
                }
        }

        return false;
}

void Screen::freeWidgets ()
{
        if ( pictureOfHead != nilPointer && ! pictureOfHead->isOnScreen () )
                delete pictureOfHead;

        if ( pictureOfHeels != nilPointer && ! pictureOfHeels->isOnScreen () )
                delete pictureOfHeels;

        while ( ! this->widgets.empty () )
        {
                Widget* w = *( widgets.begin () );
                widgets.remove( w );
                delete w;
        }

        pictureOfHead = nilPointer;
        pictureOfHeels = nilPointer;
}

void Screen::addPictureOfHeadAt ( int x, int y )
{
        if ( pictureOfHead == nilPointer )
                pictureOfHead = new AnimatedPictureWidget( x, y, loadAnimation( "head.gif" ), delayBetweenFrames, "animated Head" );
        else
                pictureOfHead->moveTo( x, y );

        addWidget( pictureOfHead );
}

void Screen::addPictureOfHeelsAt ( int x, int y )
{
        if ( pictureOfHeels == nilPointer )
                pictureOfHeels = new AnimatedPictureWidget( x, y, loadAnimation( "heels.gif" ), delayBetweenFrames, "animated Heels" );
        else
                pictureOfHeels->moveTo( x, y );

        addWidget( pictureOfHeels );
}

void Screen::placeHeadAndHeels( bool picturesToo, bool copyrightsToo )
{
        const unsigned int screenWidth = isomot::ScreenWidth();
        const unsigned int space = ( screenWidth / 20 ) - 10;

        Label* Head = new Label( "Head", "big", "yellow" );
        Label* over = new Label( "over", "", "multicolor" );
        Label* Heels = new Label( "Heels", "big", "yellow" );

        over->moveTo( ( screenWidth - over->getWidth() - 20 ) >> 1, space + Head->getHeight() - over->getHeight() - 8 );
        addWidget( over );

        Head->moveTo( over->getX() - Head->getWidth() - over->getFont()->getCharWidth() + 4, space );
        addWidget( Head );

        Heels->moveTo( over->getX() + over->getWidth() + over->getFont()->getCharWidth() - 4, space );
        addWidget( Heels );

        TextField* JonRitman = new TextField( screenWidth >> 2, "center" ) ;
        JonRitman->addLine( "Jon", "", "multicolor" );
        JonRitman->addLine( "Ritman", "", "multicolor" );
        JonRitman->moveTo( Head->getX() - JonRitman->getWidthOfField() - space, space );
        addWidget( JonRitman );

        TextField* BernieDrummond = new TextField( screenWidth >> 2, "center" ) ;
        BernieDrummond->addLine( "Bernie", "", "multicolor" );
        BernieDrummond->addLine( "Drummond", "", "multicolor" );
        BernieDrummond->moveTo( Heels->getX() + Heels->getWidth() + space, space );
        addWidget( BernieDrummond );

        if ( picturesToo )
        {
                const unsigned int widthOfSprite = 48;
                addPictureOfHeadAt( Head->getX() + ( ( Head->getWidth() - widthOfSprite ) >> 1 ), Head->getY() + Head->getHeight() );
                addPictureOfHeelsAt( Heels->getX() + ( ( Heels->getWidth() - widthOfSprite ) >> 1 ), Heels->getY() + Heels->getHeight() );
        }

        if ( copyrightsToo )
        {
                Label* ocean = new Label( "{ 1987 Ocean Software Ltd.", "", "cyan" );
                Label* Jorge = new Label( "{ 2009 Jorge Rodríguez Santos", "", "orange" );
                Label* Douglas = new Label( "{ 2018 Douglas Mencken", "", "yellow" );

                const unsigned int screenHeight = isomot::ScreenHeight();
                const int leading = 28;
                const int whereY = screenHeight - space - leading + 4;
                const int whereX = ( screenWidth - Jorge->getWidth() ) >> 1 ;

                // (c) 1987 Ocean Software Ltd.
                ocean->moveTo( whereX, whereY );
                addWidget( ocean );

                // (c) 2009 Jorge Rodríguez Santos
                Jorge->moveTo( whereX, whereY - leading );
                addWidget( Jorge );

                // (c) 2018 Douglas Mencken
                Douglas->moveTo( whereX, whereY - leading - leading );
                addWidget( Douglas );
        }
}

/* static */
Picture * Screen::loadPicture ( const std::string& nameOfPicture )
{
#if defined( DEBUG ) && DEBUG
        std::cout << "Screen::loadPicture( \"" << nameOfPicture << "\" )" << std::endl ;
#endif
        smartptr< allegro::Pict > pict( allegro::Pict::fromPNGFile (
                isomot::pathToFile( GuiManager::getInstance()->getPathToPicturesOfGui() + nameOfPicture )
        ) );
        return new Picture( *pict.get() ) ;
}

/* static */
std::vector< allegro::Pict * > Screen::loadAnimation ( const char * nameOfGif )
{
        std::vector< allegro::Pict * > animation;
        std::vector< int > durations;

        allegro::loadGIFAnimation( isomot::pathToFile( GuiManager::getInstance()->getPathToPicturesOfGui() + nameOfGif ), animation, durations );

        return animation;
}

/* static */
void Screen::scrollHorizontally( Screen* oldScreen, Screen* newScreen, bool rightToLeft )
{
        if ( oldScreen == newScreen ||
                oldScreen == nilPointer || newScreen == nilPointer ||
                oldScreen->imageOfScreen == nilPointer || newScreen->imageOfScreen == nilPointer ) return ;

        Picture oldPicture( * oldScreen->imageOfScreen );

        newScreen->redraw ();
        Picture * newPicture = newScreen->imageOfScreen;

        unsigned int step = ( ( ( isomot::ScreenWidth() >> 6 ) + 5 ) / 10 ) << 1 ;

        for ( unsigned int x = step ; x < isomot::ScreenWidth() ; x += step )
        {
                /* bitBlit( from, to, fromX, fromY, toX, toY, width, height ) */

                if ( rightToLeft )
                {
                        allegro::bitBlit( oldPicture.getAllegroPict(), allegro::Pict::theScreen(), x, 0, 0, 0, oldPicture.getWidth() - x, isomot::ScreenHeight() );
                        allegro::bitBlit( newPicture->getAllegroPict(), allegro::Pict::theScreen(), 0, 0, oldPicture.getWidth() - x, 0, x, isomot::ScreenHeight() );
                }
                else
                {
                        allegro::bitBlit( newPicture->getAllegroPict(), allegro::Pict::theScreen(), newPicture->getWidth() - x, 0, 0, 0, x, isomot::ScreenHeight() );
                        allegro::bitBlit( oldPicture.getAllegroPict(), allegro::Pict::theScreen(), 0, 0, x, 0, newPicture->getWidth() - x, isomot::ScreenHeight() );
                }

                milliSleep( 1 );
        }
}

/* static */
void Screen::wipeHorizontally( Screen* oldScreen, Screen* newScreen, bool rightToLeft )
{
        if ( oldScreen == newScreen ||
                oldScreen == nilPointer || newScreen == nilPointer ||
                oldScreen->imageOfScreen == nilPointer || newScreen->imageOfScreen == nilPointer ) return ;

        newScreen->redraw ();
        Picture * newPicture = newScreen->imageOfScreen ;

        unsigned int step = ( ( ( isomot::ScreenWidth() >> 6 ) + 5 ) / 10 ) << 1 ;

        for ( unsigned int x = step ; x < isomot::ScreenWidth() ; x += step )
        {
                if ( rightToLeft )
                {
                        allegro::bitBlit( newPicture->getAllegroPict(), allegro::Pict::theScreen(), newPicture->getWidth() - x, 0, newPicture->getWidth() - x, 0, x, isomot::ScreenHeight() );
                }
                else
                {
                        allegro::bitBlit( newPicture->getAllegroPict(), allegro::Pict::theScreen(), 0, 0, 0, 0, x, isomot::ScreenHeight() );
                }

                milliSleep( 1 );
        }
}

/* static */
void Screen::barWipeHorizontally( Screen* oldScreen, Screen* newScreen, bool rightToLeft )
{
        if ( oldScreen == newScreen ||
                oldScreen == nilPointer || newScreen == nilPointer ||
                oldScreen->imageOfScreen == nilPointer || newScreen->imageOfScreen == nilPointer ) return ;

        Picture buffer( * oldScreen->imageOfScreen );

        newScreen->redraw ();
        Picture * newPicture = newScreen->imageOfScreen ;

        unsigned int pieces = isomot::ScreenWidth() >> 6 ;
        unsigned int widthOfPiece = isomot::ScreenWidth() / pieces ;

        unsigned int step = ( pieces + 5 ) / 10 ;

        for ( unsigned int x = step ; x < widthOfPiece ; x += step )
        {
                for ( unsigned int pieceX = 0 ; pieceX < isomot::ScreenWidth() ; pieceX += widthOfPiece )
                {
                        if ( rightToLeft )
                        {
                                allegro::bitBlit( newPicture->getAllegroPict(), buffer.getAllegroPict(), pieceX + widthOfPiece - x, 0, pieceX + widthOfPiece - x, 0, x, isomot::ScreenHeight() );
                        }
                        else
                        {
                                allegro::bitBlit( newPicture->getAllegroPict(), buffer.getAllegroPict(), pieceX, 0, pieceX, 0, x, isomot::ScreenHeight() );
                        }
                }

                allegro::bitBlit( buffer.getAllegroPict(), allegro::Pict::theScreen() );

                milliSleep( 2 );
        }
}

/* static */
void Screen::randomPixelFade( bool fadeIn, Screen * slide, const Color& color )
{
        if ( slide == nilPointer || slide->imageOfScreen == nilPointer ) return;

        // refresh screen before fading
        slide->redraw();

        AllegroColor aColor = color.toAllegroColor() ;

        if ( fadeIn )
        {
                allegro::Pict::theScreen().clearToColor( aColor );
        }

        const unsigned int screenWidth = slide->imageOfScreen->getWidth() ;
        const unsigned int screenHeight = slide->imageOfScreen->getHeight() ;

        const size_t howManyPixels = screenWidth * screenHeight;

        std::vector< bool > bits( howManyPixels, false ); // bit map of howManyPixels bits

        unsigned int limit = ( howManyPixels >= 100000 ) ? ( howManyPixels / 50000 ) - 1 : 0 ;

        for ( size_t yet = 0 ; yet < howManyPixels - limit ; )
        {
                int x = rand() % screenWidth ;  /* random between 0 and screenWidth - 1 */
                int y =  rand() % screenHeight ; /* random between 0 and screenHeight - 1 */

                if ( ! bits[ x + y * screenWidth ] )
                {
                        if ( fadeIn )
                                allegro::Pict::theScreen().drawPixelAt( x, y, slide->imageOfScreen->getPixelAt( x, y ) );
                        else
                                allegro::Pict::theScreen().drawPixelAt( x, y, aColor );

                        bits[ x + y * screenWidth ] = true;
                        yet ++;
                }
        }
}

}

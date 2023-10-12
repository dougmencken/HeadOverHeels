
#include "Screen.hpp"
#include "Gui.hpp"
#include "Menu.hpp"
#include "GuiManager.hpp"
#include "GameManager.hpp"
#include "Action.hpp"
#include "Label.hpp"
#include "TextField.hpp"
#include "PictureWidget.hpp"
#include "AnimatedPictureWidget.hpp"
#include "Color.hpp"

#include "sleep.hpp"
#include "screen.hpp"
#include "ospaths.hpp"

#include "MayNotBePossible.hpp"

#include <iostream>


const float delayBetweenFrames = 0.1 ;


namespace gui
{

/* static */ Picture * Screen::backgroundPicture = nilPointer ;

/* static */ void Screen::refreshBackground ()
{
        std::cout << "refreshing the background for user interface slides" << std::endl ;

        delete Screen::backgroundPicture ;

        Screen::backgroundPicture = Screen::loadPicture( "background.png" );

        if ( Screen::backgroundPicture == nilPointer ) // not found
        {       // don't crash, just present the red one like in the original game
                Screen::backgroundPicture = new Picture( variables::getScreenWidth(), variables::getScreenHeight(), Color::byName( "red" ) ) ;
                std::cout << "there's no background.png but I made the red background for you dear" << std::endl ;
        }

        Screen::backgroundPicture->setName( "the background for user interface slides" );
}

/* static */ void Screen::toBlackBackground ()
{
        delete Screen::backgroundPicture ;
        Screen::backgroundPicture = new Picture( variables::getScreenWidth(), variables::getScreenHeight(), Color::blackColor() ) ;
        Screen::backgroundPicture->setName( "the black background for user interface slides" );
}

Screen::Screen( Action* action ) :
        Widget( 0, 0 ),
        imageOfScreen( new Picture( variables::getScreenWidth(), variables::getScreenHeight() ) ),
        actionOfScreen( action ),
        escapeAction( nilPointer ),
        keyHandler( nilPointer ),
        pictureOfHead( nilPointer ),
        pictureOfHeels( nilPointer )
{
        imageOfScreen->setName( "image of screen for action " + action->getNameOfAction() );

        if ( Screen::backgroundPicture == nilPointer ) Screen::refreshBackground () ;

        refreshPicturesOfHeadAndHeels ();
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

void Screen::draw ()
{
        refresh ();
        drawOnGlobalScreen( );
}

void Screen::refresh () const
{
        if ( imageOfScreen == nilPointer ) return ; // nothing to refresh

        const allegro::Pict& previousWhere = allegro::Pict::getWhereToDraw() ;
        allegro::Pict::setWhereToDraw( imageOfScreen->getAllegroPict() );

        imageOfScreen->fillWithColor( Color::byName( "red" ) ); // the red background is so red

        // draw background, if any

        if ( Screen::backgroundPicture == nilPointer )
        {
                Screen::refreshBackground ();
        }
        if ( Screen::backgroundPicture == nilPointer )
        {       // it's impossible
                throw new MayNotBePossible( "Screen::backgroundPicture is nil after Screen::refreshBackground()" ) ;
        }

        unsigned int backgroundWidth = backgroundPicture->getWidth();
        unsigned int backgroundHeight = backgroundPicture->getHeight();

        if ( backgroundWidth == variables::getScreenWidth() && backgroundHeight == variables::getScreenHeight() )
        {
                allegro::bitBlit( backgroundPicture->getAllegroPict() );
        }
        else
        {
                float ratioX = static_cast< float >( variables::getScreenWidth() ) / static_cast< float >( backgroundWidth ) ;
                float ratioY = static_cast< float >( variables::getScreenHeight() ) / static_cast< float >( backgroundHeight ) ;

                if ( ratioX == ratioY )
                {
                        allegro::stretchBlit( backgroundPicture->getAllegroPict(),
                                        0, 0, backgroundWidth, backgroundHeight,
                                        0, 0, variables::getScreenWidth(), variables::getScreenHeight() );
                }
                else if ( ratioX > ratioY ) /* horizontal over~stretching */
                {
                        unsigned int proportionalWidth = static_cast< unsigned int >( backgroundWidth * ratioY );
                        unsigned int offsetX = ( variables::getScreenWidth() - proportionalWidth ) >> 1;

                        imageOfScreen->fillWithColor( Color::blackColor() );

                        allegro::stretchBlit( backgroundPicture->getAllegroPict(),
                                        0, 0, backgroundWidth, backgroundHeight,
                                        offsetX, 0, proportionalWidth, variables::getScreenHeight() );
                }
                else /* if ( ratioY > ratioX ) */ /* vertical over~stretching */
                {
                        unsigned int proportionalHeight = static_cast< unsigned int >( backgroundHeight * ratioX );
                        unsigned int offsetY = ( variables::getScreenHeight() - proportionalHeight ) >> 1;

                        imageOfScreen->fillWithColor( Color::blackColor() );

                        allegro::stretchBlit( backgroundPicture->getAllegroPict(),
                                        0, 0, backgroundWidth, backgroundHeight,
                                        0, offsetY, variables::getScreenWidth(), proportionalHeight );
                }
        }

        if ( game::GameManager::getInstance().isSimpleGraphicsSet() )
        {
                static const size_t howManyColors = 8 ;
                static const std::string colors[ howManyColors ] =
                        { "black", "blue", "red", "magenta", "green", "cyan", "yellow", "white" } ;

                const unsigned int sizeOfSquare = 8 ;

                const unsigned int colorsX = variables::getScreenWidth() - ( howManyColors * sizeOfSquare );
                const unsigned int colorsY = variables::getScreenHeight() - ( 2 * sizeOfSquare );

                for ( size_t i = 0 ; i < howManyColors ; ++ i )
                {
                        allegro::fillRect( colorsX + ( i * sizeOfSquare ), colorsY + sizeOfSquare,
                                           colorsX + ( i * sizeOfSquare ) + sizeOfSquare - 1, colorsY + sizeOfSquare  + sizeOfSquare - 1,
                                           Color::byName( colors[ i ] ).toAllegroColor () );

                        allegro::fillRect( colorsX + ( i * sizeOfSquare ), colorsY,
                                           colorsX + ( i * sizeOfSquare ) + sizeOfSquare - 1, colorsY + sizeOfSquare - 1,
                                           Color::byName( "reduced " + colors[ i ] ).toAllegroColor () );

                        /* allegro::fillRect( colorsX + ( i * sizeOfSquare ), colorsY + ( sizeOfSquare << 1 ),
                                           colorsX + ( i * sizeOfSquare ) + sizeOfSquare - 1, colorsY + ( sizeOfSquare << 1 ) + sizeOfSquare - 1,
                                           Color::byName( "light " + colors[ i ] ).toAllegroColor () ); */
                }
        }

        // draw each component

        for ( std::list< Widget * >::const_iterator it = widgets.begin () ; it != widgets.end () ; ++it )
        {
                ( *it )->draw ();
        }

        allegro::Pict::setWhereToDraw( previousWhere );
}

void Screen::drawOnGlobalScreen( )
{
        allegro::bitBlit( imageOfScreen->getAllegroPict(), allegro::Pict::theScreen() );
        allegro::update ();
}

void Screen::handleKey( const std::string& key )
{
        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "f" ) )
        {
                gui::GuiManager::getInstance().toggleFullScreenVideo ();
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
        const unsigned int screenWidth = variables::getScreenWidth();
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
                Label* Jorge = new Label( "{ 2009 Jorge Rodríguez Santos", "", "orange" );
                Label* Douglas = new Label( "{ 2023 Douglas Mencken", "", "yellow" );

                const unsigned int screenHeight = variables::getScreenHeight();
                const int leading = 28;
                const int whereY = screenHeight - space - leading + 4;
                const int whereX = ( screenWidth - Jorge->getWidth() ) >> 1 ;

                // Jorge Rodríguez Santos
                Jorge->moveTo( whereX, whereY );
                addWidget( Jorge );

                // Douglas Mencken
                Douglas->moveTo( whereX, whereY - leading );
                addWidget( Douglas );
        }
}

/* static */
Picture * Screen::loadPicture ( const std::string & nameOfPicture )
{
#if defined( DEBUG ) && DEBUG
        std::cout << "Screen::loadPicture( \"" << nameOfPicture << "\" )" << std::endl ;
#endif
        const std::string & pathToPictures = GuiManager::getInstance().getPathToThesePictures() ;
        autouniqueptr< allegro::Pict > pict( allegro::Pict::fromPNGFile (
                ospaths::pathToFile( pathToPictures, nameOfPicture )
        ) );

        if ( pict != nilPointer && pict->isNotNil () ) {
                return new Picture( *pict ) ;
        }

        std::cout << "can't load picture \"" << pathToPictures << ospaths::pathSeparator() << nameOfPicture << "\"" << std::endl ;
        return nilPointer ;
}

/* static */
std::vector< allegro::Pict * > Screen::loadAnimation ( const char * nameOfGif )
{
        std::vector< allegro::Pict * > animation;
        std::vector< int > durations;

        allegro::loadGIFAnimation( ospaths::pathToFile( GuiManager::getInstance().getPathToThesePictures(), nameOfGif ), animation, durations );

        return animation;
}

/* static */
void Screen::scrollHorizontally( const Screen& oldScreen, const Screen& newScreen, bool rightToLeft )
{
        if ( &oldScreen == &newScreen ||
                oldScreen.imageOfScreen == nilPointer || newScreen.imageOfScreen == nilPointer ) return ;

        Picture oldPicture( * oldScreen.imageOfScreen );

        newScreen.refresh ();
        Picture& newPicture = * newScreen.imageOfScreen ;

        unsigned int step = ( ( ( variables::getScreenWidth() >> 6 ) + 5 ) / 10 ) << 1 ;

        for ( unsigned int x = step ; x < variables::getScreenWidth() ; x += step )
        {
                /* bitBlit( from, to, fromX, fromY, toX, toY, width, height ) */

                if ( rightToLeft )
                {
                        allegro::bitBlit( oldPicture.getAllegroPict(), allegro::Pict::theScreen(), x, 0, 0, 0, oldPicture.getWidth() - x, variables::getScreenHeight() );
                        allegro::bitBlit( newPicture.getAllegroPict(), allegro::Pict::theScreen(), 0, 0, oldPicture.getWidth() - x, 0, x, variables::getScreenHeight() );
                }
                else
                {
                        allegro::bitBlit( newPicture.getAllegroPict(), allegro::Pict::theScreen(), newPicture.getWidth() - x, 0, 0, 0, x, variables::getScreenHeight() );
                        allegro::bitBlit( oldPicture.getAllegroPict(), allegro::Pict::theScreen(), 0, 0, x, 0, newPicture.getWidth() - x, variables::getScreenHeight() );
                }

                allegro::update ();
                somn::milliSleep( 1 );
        }
}

/* static */
void Screen::wipeHorizontally( const Screen& oldScreen, const Screen& newScreen, bool rightToLeft )
{
        if ( &oldScreen == &newScreen ||
                oldScreen.imageOfScreen == nilPointer || newScreen.imageOfScreen == nilPointer ) return ;

        newScreen.refresh ();
        Picture& newPicture = * newScreen.imageOfScreen ;

        unsigned int step = ( ( ( variables::getScreenWidth() >> 6 ) + 5 ) / 10 ) << 1 ;

        for ( unsigned int x = step ; x < variables::getScreenWidth() ; x += step )
        {
                if ( rightToLeft )
                {
                        allegro::bitBlit( newPicture.getAllegroPict(), allegro::Pict::theScreen(), newPicture.getWidth() - x, 0, newPicture.getWidth() - x, 0, x, variables::getScreenHeight() );
                }
                else
                {
                        allegro::bitBlit( newPicture.getAllegroPict(), allegro::Pict::theScreen(), 0, 0, 0, 0, x, variables::getScreenHeight() );
                }

                allegro::update ();
                somn::milliSleep( 1 );
        }
}

/* static */
void Screen::barWipeHorizontally( const Screen& oldScreen, const Screen& newScreen, bool rightToLeft )
{
        if ( &oldScreen == &newScreen ||
                oldScreen.imageOfScreen == nilPointer || newScreen.imageOfScreen == nilPointer ) return ;

        Picture buffer( * oldScreen.imageOfScreen );

        newScreen.refresh ();
        Picture& newPicture = * newScreen.imageOfScreen ;

        unsigned int pieces = variables::getScreenWidth() >> 6 ;
        unsigned int widthOfPiece = variables::getScreenWidth() / pieces ;

        unsigned int step = ( pieces + 5 ) / 10 ;

        for ( unsigned int x = step ; x < widthOfPiece ; x += step )
        {
                for ( unsigned int pieceX = 0 ; pieceX < variables::getScreenWidth() ; pieceX += widthOfPiece )
                {
                        if ( rightToLeft )
                        {
                                allegro::bitBlit( newPicture.getAllegroPict(), buffer.getAllegroPict(), pieceX + widthOfPiece - x, 0, pieceX + widthOfPiece - x, 0, x, variables::getScreenHeight() );
                        }
                        else
                        {
                                allegro::bitBlit( newPicture.getAllegroPict(), buffer.getAllegroPict(), pieceX, 0, pieceX, 0, x, variables::getScreenHeight() );
                        }
                }

                allegro::bitBlit( buffer.getAllegroPict(), allegro::Pict::theScreen() );
                allegro::update ();

                somn::milliSleep( 2 );
        }
}

/* static */
void Screen::randomPixelFade( bool fadeIn, const Screen& slide, const Color& color )
{
        if ( slide.imageOfScreen == nilPointer ) return;

        // refresh screen before fading
        slide.refresh();

        AllegroColor aColor = color.toAllegroColor() ;

        if ( fadeIn )
        {
                allegro::Pict::theScreen().clearToColor( aColor );
                allegro::update();
        }

        const unsigned int screenWidth = slide.imageOfScreen->getWidth() ;
        const unsigned int screenHeight = slide.imageOfScreen->getHeight() ;

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
                                allegro::Pict::theScreen().drawPixelAt( x, y, slide.imageOfScreen->getPixelAt( x, y ) );
                        else
                                allegro::Pict::theScreen().drawPixelAt( x, y, aColor );

                        allegro::update();

                        bits[ x + y * screenWidth ] = true;
                        yet ++;
                }
        }
}

}

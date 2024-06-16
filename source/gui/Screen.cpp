
#include "Screen.hpp"

#include "Menu.hpp"
#include "GuiManager.hpp"
#include "GameManager.hpp"
#include "GamePreferences.hpp"
#include "Label.hpp"
#include "TextField.hpp"
#include "PictureWidget.hpp"
#include "AnimatedPictureWidget.hpp"
#include "Color.hpp"

#include "sleep.hpp"
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

        const std::string & pathToPictures = ospaths::sharePath() + GameManager::getInstance().getChosenGraphicsSet() ;
        Screen::backgroundPicture = Picture::loadPicture( ospaths::pathToFile( pathToPictures, "background.png" ) );

        if ( Screen::backgroundPicture == nilPointer ) // not found
        {       // don't crash, just present the red one like in the original game
                Screen::backgroundPicture = new Picture( GamePreferences::getScreenWidth(), GamePreferences::getScreenHeight(), Color::byName( "red" ) ) ;
                std::cout << "there’s no \"background.png\" but I made the red background for you dear" << std::endl ;
        }

        Screen::backgroundPicture->setName( "the background for user interface slides" );
}

/* static */ void Screen::toBlackBackground ()
{
        delete Screen::backgroundPicture ;
        Screen::backgroundPicture = new Picture( GamePreferences::getScreenWidth(), GamePreferences::getScreenHeight(), Color::blackColor() ) ;
        Screen::backgroundPicture->setName( "the black background for user interface slides" );
}

Screen::Screen( Action & action ) :
        Widget( 0, 0 ),
        imageOfScreen( new Picture( GamePreferences::getScreenWidth(), GamePreferences::getScreenHeight() ) ),
        drawSpectrumColors( false ),
        actionOfScreen( action ),
        escapeAction( nilPointer ),
        nextKeyHandler( nilPointer ),
        pictureOfHead( nilPointer ),
        pictureOfHeels( nilPointer )
{
        this->imageOfScreen->setName( "image of screen for action " + getNameOfAction() );

        if ( Screen::backgroundPicture == nilPointer ) Screen::refreshBackground () ;

        refreshPicturesOfHeadAndHeels ();
}

Screen::~Screen( )
{
        freeWidgets() ;
}

void Screen::addPictureOfHeadAt ( int x, int y )
{
        if ( this->pictureOfHead == nilPointer ) {
                const std::string & pathToPictures = ospaths::sharePath() + GameManager::getInstance().getChosenGraphicsSet() ;
                this->pictureOfHead = new AnimatedPictureWidget(
                                                x, y,
                                                Picture::loadAnimation( ospaths::pathToFile( pathToPictures, "head.gif" ) ),
                                                delayBetweenFrames,
                                                "animated Head" );
        } else
                this->pictureOfHead->moveTo( x, y );

        addWidget( this->pictureOfHead );
}

void Screen::addPictureOfHeelsAt ( int x, int y )
{
        if ( this->pictureOfHeels == nilPointer ) {
                const std::string & pathToPictures = ospaths::sharePath() + GameManager::getInstance().getChosenGraphicsSet() ;
                this->pictureOfHeels = new AnimatedPictureWidget(
                                                x, y,
                                                Picture::loadAnimation( ospaths::pathToFile( pathToPictures, "heels.gif" ) ),
                                                delayBetweenFrames,
                                                "animated Heels" );
        } else
                this->pictureOfHeels->moveTo( x, y );

        addWidget( this->pictureOfHeels );
}

void Screen::refreshPicturesOfHeadAndHeels ()
{
        if ( this->pictureOfHead != nilPointer )
        {
                int xHead = this->pictureOfHead->getX ();
                int yHead = this->pictureOfHead->getY ();

                if ( this->pictureOfHead->isOnScreen() )
                        removeWidget( this->pictureOfHead );
                else
                        delete this->pictureOfHead ;

                this->pictureOfHead = nilPointer ;
                addPictureOfHeadAt( xHead, yHead );
        }

        if ( this->pictureOfHeels != nilPointer )
        {
                int xHeels = this->pictureOfHeels->getX ();
                int yHeels = this->pictureOfHeels->getY ();

                if ( this->pictureOfHeels->isOnScreen() )
                        removeWidget( this->pictureOfHeels );
                else
                        delete this->pictureOfHeels ;

                this->pictureOfHeels = nilPointer ;
                addPictureOfHeelsAt( xHeels, yHeels );
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
                throw MayNotBePossible( "Screen::backgroundPicture is nil after Screen::refreshBackground()" ) ;
        }

        unsigned int backgroundWidth = backgroundPicture->getWidth();
        unsigned int backgroundHeight = backgroundPicture->getHeight();

        if ( backgroundWidth == GamePreferences::getScreenWidth() && backgroundHeight == GamePreferences::getScreenHeight() )
        {
                allegro::bitBlit( backgroundPicture->getAllegroPict() );
        }
        else
        {
                float ratioX = static_cast< float >( GamePreferences::getScreenWidth() ) / static_cast< float >( backgroundWidth ) ;
                float ratioY = static_cast< float >( GamePreferences::getScreenHeight() ) / static_cast< float >( backgroundHeight ) ;

                if ( ratioX == ratioY )
                {
                        allegro::stretchBlit( backgroundPicture->getAllegroPict(),
                                        0, 0, backgroundWidth, backgroundHeight,
                                        0, 0, GamePreferences::getScreenWidth(), GamePreferences::getScreenHeight() );
                }
                else if ( ratioX > ratioY ) /* horizontal over~stretching */
                {
                        unsigned int proportionalWidth = static_cast< unsigned int >( backgroundWidth * ratioY );
                        unsigned int offsetX = ( GamePreferences::getScreenWidth() - proportionalWidth ) >> 1;

                        imageOfScreen->fillWithColor( Color::blackColor() );

                        allegro::stretchBlit( backgroundPicture->getAllegroPict(),
                                        0, 0, backgroundWidth, backgroundHeight,
                                        offsetX, 0, proportionalWidth, GamePreferences::getScreenHeight() );
                }
                else /* if ( ratioY > ratioX ) */ /* vertical over~stretching */
                {
                        unsigned int proportionalHeight = static_cast< unsigned int >( backgroundHeight * ratioX );
                        unsigned int offsetY = ( GamePreferences::getScreenHeight() - proportionalHeight ) >> 1;

                        imageOfScreen->fillWithColor( Color::blackColor() );

                        allegro::stretchBlit( backgroundPicture->getAllegroPict(),
                                        0, 0, backgroundWidth, backgroundHeight,
                                        0, offsetY, GamePreferences::getScreenWidth(), proportionalHeight );
                }
        }

        if ( drawSpectrumColors /* || GameManager::getInstance().isSimpleGraphicsSet() */ )
                Screen::draw2x8colors( *this ) ;

        // draw each component
        for ( std::vector< Widget * >::const_iterator wi = widgets.begin () ; wi != widgets.end () ; ++ wi )
                ( *wi )->draw ();

        allegro::Pict::setWhereToDraw( previousWhere );
}

void Screen::drawOnGlobalScreen( )
{
        allegro::bitBlit( imageOfScreen->getAllegroPict(), allegro::Pict::theScreen() );
        allegro::update ();
}

/* static */
void Screen::draw2x8colors ( const Screen & slide )
{
        static const size_t howManyColors = 8 ;
        static const std::string colors[ howManyColors ] =
                { "black", "blue", "red", "magenta", "green", "cyan", "yellow", "white" } ;

        const unsigned int sizeOfSquare = 8 ;

        const unsigned int colorsX = slide.getImageOfScreen().getWidth () - ( howManyColors * sizeOfSquare );
        const unsigned int colorsY = slide.getImageOfScreen().getHeight() - ( 2 * sizeOfSquare );

        for ( size_t i = 0 ; i < howManyColors ; ++ i )
        {
                allegro::fillRect( slide.getImageOfScreen().getAllegroPict (),
                                   colorsX + ( i * sizeOfSquare ), colorsY + sizeOfSquare,
                                   colorsX + ( i * sizeOfSquare ) + sizeOfSquare - 1, colorsY + sizeOfSquare + sizeOfSquare - 1,
                                   Color::byName( colors[ i ] ).toAllegroColor () );

                allegro::fillRect( slide.getImageOfScreen().getAllegroPict (),
                                   colorsX + ( i * sizeOfSquare ), colorsY,
                                   colorsX + ( i * sizeOfSquare ) + sizeOfSquare - 1, colorsY + sizeOfSquare - 1,
                                   Color::byName( "reduced " + colors[ i ] ).toAllegroColor () );

                /* allegro::fillRect( slide.getImageOfScreen().getAllegroPict (),
                                   colorsX + ( i * sizeOfSquare ), colorsY + ( sizeOfSquare << 1 ),
                                   colorsX + ( i * sizeOfSquare ) + sizeOfSquare - 1, colorsY + ( sizeOfSquare << 1 ) + sizeOfSquare - 1,
                                   Color::byName( "light " + colors[ i ] ).toAllegroColor () ); */
        }
}

void Screen::handleKey( const std::string& key )
{
        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "f" ) )
        {
                gui::GuiManager::getInstance().toggleFullScreenVideo ();
                return;
        }

        if ( this->escapeAction != nilPointer && key == "Escape" )
                this->escapeAction->doIt ();
        else
                if ( this->nextKeyHandler != nilPointer )
                        this->nextKeyHandler->handleKey( key );
}

void Screen::addWidget( Widget* widget )
{
        if ( widget == nilPointer ) return ;

        this->widgets.push_back( widget );
        widget->setOnScreen( true );
}

bool Screen::removeWidget( Widget* widget )
{
        for ( std::vector< Widget* >::const_iterator it = widgets.begin (); it != widgets.end (); )
        {
                if ( ( *it ) == widget ) {
                        /* it = */ widgets.erase( it );
                        delete widget ;

                        return true ;
                } else
                        ++ it ;
        }

        return false ;
}

void Screen::freeWidgets ()
{
        if ( pictureOfHead != nilPointer && ! pictureOfHead->isOnScreen () )
                delete pictureOfHead;

        if ( pictureOfHeels != nilPointer && ! pictureOfHeels->isOnScreen () )
                delete pictureOfHeels;

        while ( ! this->widgets.empty () )
        {
                Widget* w = this->widgets.back ();
                widgets.pop_back() ;
                delete w ;
        }

        this->pictureOfHead = nilPointer ;
        this->pictureOfHeels = nilPointer ;
}

void Screen::placeHeadAndHeels( bool picturesToo, bool copyrightsToo )
{
        const unsigned int screenWidth = GamePreferences::getScreenWidth();
        const unsigned int space = ( screenWidth / 20 ) - 10;

        Label* Head = new Label( "Head", Font::fontWith2xHeightAndColor( "yellow" ) );
        Label* over = new Label( "over", Font::fontWithColor( "" ), /* multicolor */ true );
        Label* Heels = new Label( "Heels", Font::fontWith2xHeightAndColor( "yellow" ) );

        over->moveTo( ( screenWidth - over->getWidth() - 20 ) >> 1, space + Head->getHeight() - over->getHeight() - 8 );
        addWidget( over );

        unsigned int widthOfSpace = over->getFont().getWidthOfLetter( " " );
        int spaceWithoutSpacing = widthOfSpace - over->getSpacing ();

        Head->moveTo( over->getX() - Head->getWidth() - spaceWithoutSpacing, space );
        addWidget( Head );

        Heels->moveTo( over->getX() + over->getWidth() + spaceWithoutSpacing, space );
        addWidget( Heels );

        TextField* JonRitman = new TextField( screenWidth >> 2, "center" ) ;
        JonRitman->appendText( "Jon", false, "multicolor" );
        JonRitman->appendText( "Ritman", false, "multicolor" );
        JonRitman->moveTo( Head->getX() - JonRitman->getWidthOfField() - space, space );
        addWidget( JonRitman );

        TextField* BernieDrummond = new TextField( screenWidth >> 2, "center" ) ;
        BernieDrummond->appendText( "Bernie", false, "multicolor" );
        BernieDrummond->appendText( "Drummond", false, "multicolor" );
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
                Label* Jorge = new Label( "{ 2009 Jorge Rodríguez Santos", Font::fontWithColor( "orange" ) );
                Label* Douglas = new Label( "{ 2024 Douglas Mencken", Font::fontWithColor( "yellow" ) );

                const unsigned int screenHeight = GamePreferences::getScreenHeight();
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
void Screen::scrollHorizontally( const Screen& oldScreen, const Screen& newScreen, bool rightToLeft )
{
        if ( &oldScreen == &newScreen ||
                oldScreen.imageOfScreen == nilPointer || newScreen.imageOfScreen == nilPointer ) return ;

        Picture buffer( * oldScreen.imageOfScreen );

        autouniqueptr < Timer > drawTimer( new Timer() );
        drawTimer->go ();

        newScreen.refresh ();
        Picture& newPicture = * newScreen.imageOfScreen ;

        unsigned int step = ( ( ( GamePreferences::getScreenWidth() >> 6 ) + 5 ) / 10 ) << 1 ;

        for ( unsigned int x = step ; x < GamePreferences::getScreenWidth() ; x += step )
        {
                /* bitBlit( from, to, fromX, fromY, toX, toY, width, height ) */

                if ( rightToLeft )
                        allegro::bitBlit( newPicture.getAllegroPict(), buffer.getAllegroPict(),
                                                0, 0, oldScreen.imageOfScreen->getWidth() - x, 0, x, GamePreferences::getScreenHeight() );
                else
                        allegro::bitBlit( newPicture.getAllegroPict(), buffer.getAllegroPict(),
                                                newPicture.getWidth() - x, 0, 0, 0, x, GamePreferences::getScreenHeight() );

                if ( drawTimer->getValue() > 0.02 ) // every 20 milliseconds, that’s 1/50 of a second
                {
                        allegro::bitBlit( buffer.getAllegroPict(), allegro::Pict::theScreen() );
                        allegro::update ();
                        drawTimer->reset ();
                }

                somn::milliSleep( 1 );
        }
}

/* static */
void Screen::wipeHorizontally( const Screen& oldScreen, const Screen& newScreen, bool rightToLeft )
{
        if ( &oldScreen == &newScreen ||
                oldScreen.imageOfScreen == nilPointer || newScreen.imageOfScreen == nilPointer ) return ;

        Picture buffer( * oldScreen.imageOfScreen );

        autouniqueptr < Timer > drawTimer( new Timer() );
        drawTimer->go ();

        newScreen.refresh ();
        Picture& newPicture = * newScreen.imageOfScreen ;

        unsigned int step = ( ( ( GamePreferences::getScreenWidth() >> 6 ) + 5 ) / 10 ) << 1 ;

        for ( unsigned int x = step ; x < GamePreferences::getScreenWidth() ; x += step )
        {
                if ( rightToLeft )
                        allegro::bitBlit( newPicture.getAllegroPict(), buffer.getAllegroPict(),
                                                newPicture.getWidth() - x, 0, newPicture.getWidth() - x, 0, x, GamePreferences::getScreenHeight() );
                else
                        allegro::bitBlit( newPicture.getAllegroPict(), buffer.getAllegroPict(),
                                                0, 0, 0, 0, x, GamePreferences::getScreenHeight() );

                if ( drawTimer->getValue() > 0.02 ) // every 20 milliseconds, that’s 1/50 of a second
                {
                        allegro::bitBlit( buffer.getAllegroPict(), allegro::Pict::theScreen() );
                        allegro::update ();
                        drawTimer->reset ();
                }

                somn::milliSleep( 1 );
        }
}

/* static */
void Screen::barWipeHorizontally( const Screen& oldScreen, const Screen& newScreen, bool rightToLeft )
{
        if ( &oldScreen == &newScreen ||
                oldScreen.imageOfScreen == nilPointer || newScreen.imageOfScreen == nilPointer ) return ;

        Picture buffer( * oldScreen.imageOfScreen );

        autouniqueptr < Timer > drawTimer( new Timer() );
        drawTimer->go ();

        newScreen.refresh ();
        Picture& newPicture = * newScreen.imageOfScreen ;

        unsigned int pieces = GamePreferences::getScreenWidth() >> 6 ;
        unsigned int widthOfPiece = GamePreferences::getScreenWidth() / pieces ;

        unsigned int step = ( pieces + 5 ) / 10 ;

        for ( unsigned int x = step ; x < widthOfPiece ; x += step )
        {
                for ( unsigned int pieceX = 0 ; pieceX < GamePreferences::getScreenWidth() ; pieceX += widthOfPiece )
                {
                        if ( rightToLeft )
                                allegro::bitBlit( newPicture.getAllegroPict(), buffer.getAllegroPict(),
                                                        pieceX + widthOfPiece - x, 0, pieceX + widthOfPiece - x, 0, x, GamePreferences::getScreenHeight() );
                        else
                                allegro::bitBlit( newPicture.getAllegroPict(), buffer.getAllegroPict(),
                                                        pieceX, 0, pieceX, 0, x, GamePreferences::getScreenHeight() );
                }

                if ( drawTimer->getValue() > 0.02 ) // every 20 milliseconds, that’s 1/50 of a second
                {
                        allegro::bitBlit( buffer.getAllegroPict(), allegro::Pict::theScreen() );
                        allegro::update ();
                        drawTimer->reset ();
                }

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

        const unsigned int howManyPixels = screenWidth * screenHeight ;
        const unsigned int chunk = ( howManyPixels >> 9 ) - 1 ;

        /*
         * the bit map of screenWidth x screenHeight bits
         *
         * if someone thinks that bits the bitmap is redundant here since pixels are copied off screen,
         * so copying a pixel one more time is better than looking for was it already copied or not,
         * don’t bother removing it, the counting of pixels with the bit map is here for ending the loop
         */
        std::vector< bool > bits( howManyPixels, false );

        Picture buffer( allegro::Pict::theScreen () );

        autouniqueptr < Timer > drawTimer( new Timer() );
        drawTimer->go ();

        for ( unsigned int yet = 0 ; yet < howManyPixels ; )
        {
                int x = rand() % screenWidth ;  /* random between 0 and screenWidth - 1 */
                int y = rand() % screenHeight ; /* random between 0 and screenHeight - 1 */

                if ( ! bits[ x + y * screenWidth ] )
                {
                        if ( fadeIn )
                                buffer.getAllegroPict().drawPixelAt( x, y, slide.imageOfScreen->getPixelAt( x, y ) );
                        else
                                buffer.getAllegroPict().drawPixelAt( x, y, aColor );

                        if ( yet % chunk == 0 ) // after painting a chunk
                                somn::milliSleep( 1 ); // wait a millisecond

                        if ( drawTimer->getValue() > 0.02 ) // every 20 milliseconds, that’s 1/50 of a second
                        {
                                allegro::bitBlit( buffer.getAllegroPict(), allegro::Pict::theScreen() );
                                allegro::update ();
                                drawTimer->reset ();
                        }

                        bits[ x + y * screenWidth ] = true;
                        yet ++;
                }
        }
}

}

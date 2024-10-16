
#include "Slide.hpp"

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

/* static */ Picture * Slide::backgroundPicture = nilPointer ;
/* static */ Picture * Slide::scaledBackgroundPicture = nilPointer ;

/* static */ void Slide::refreshBackground ()
{
        std::cout << "refreshing the background for user interface slides" << std::endl ;

        if ( Slide::backgroundPicture != nilPointer ) {
                delete Slide::backgroundPicture ;
                Slide::backgroundPicture = nilPointer ;
        }
        if ( Slide::scaledBackgroundPicture != nilPointer ) {
                delete Slide::scaledBackgroundPicture ;
                Slide::scaledBackgroundPicture = nilPointer ;
        }

        const std::string & pathToPictures = ospaths::sharePath() + GameManager::getInstance().getChosenGraphicsSet() ;

        if ( GameManager::getInstance().isPresentGraphicsSet () ) {
                Color backgroundColor( /* red */ 0x4e, /* green */ 0xbe, /* blue */ 0xef, /* alpha */ 0xff );
                Picture * solidColoredBackground = new Picture( 1920, 1440, backgroundColor );

                Picture * leftLensFlare = Picture::loadPicture( ospaths::pathToFile( pathToPictures, "background-left-difference.png" ) );
                Picture * rightLensFlare = Picture::loadPicture( ospaths::pathToFile( pathToPictures, "background-right-difference.png" ) );

                if ( leftLensFlare != nilPointer && rightLensFlare != nilPointer ) {
                        Picture * colorPlusLeft = Picture::summation( *solidColoredBackground, *leftLensFlare );
                        Slide::backgroundPicture = Picture::summation( *colorPlusLeft, *rightLensFlare ) ;
                        delete colorPlusLeft ;
                } else
                        Slide::backgroundPicture = new Picture( GamePreferences::getScreenWidth(), GamePreferences::getScreenHeight(), backgroundColor ) ;

                delete solidColoredBackground ;
                if ( leftLensFlare != nilPointer ) delete leftLensFlare ;
                if ( rightLensFlare != nilPointer ) delete rightLensFlare ;
        } else
                Slide::backgroundPicture = Picture::loadPicture( ospaths::pathToFile( pathToPictures, "background.png" ) );

        if ( Slide::backgroundPicture == nilPointer )
        {       // don't crash, just present the red one like in the original game
                Slide::backgroundPicture = new Picture( GamePreferences::getScreenWidth(), GamePreferences::getScreenHeight(), Color::byName( "red" ) ) ;
                std::cout << "there’s no \"background.png\" but I made the red background for you dear" << std::endl ;
        }

        Slide::backgroundPicture->setName( "the background for user interface slides" );
}

/* static */ void Slide::toBlackBackground ()
{
        if ( Slide::backgroundPicture != nilPointer ) {
                delete Slide::backgroundPicture ;
                Slide::backgroundPicture = nilPointer ;
        }
        if ( Slide::scaledBackgroundPicture != nilPointer ) {
                delete Slide::scaledBackgroundPicture ;
                Slide::scaledBackgroundPicture = nilPointer ;
        }

        Slide::backgroundPicture = new Picture( GamePreferences::getScreenWidth(), GamePreferences::getScreenHeight(), Color::blackColor() ) ;
        Slide::backgroundPicture->setName( "the black background for user interface slides" );
}

Slide::Slide() :
        Widget( 0, 0 ),
        imageOfSlide( new Picture( GamePreferences::getScreenWidth(), GamePreferences::getScreenHeight() ) ),
        noTransitionFrom( false ),
        noTransitionTo( false ),
        drawSpectrumColors( false ),
        escapeAction( nilPointer ),
        keyHandler( nilPointer ),
        pictureOfHead( nilPointer ),
        pictureOfHeels( nilPointer )
{
        if ( Slide::backgroundPicture == nilPointer ) Slide::refreshBackground () ;

        refreshPicturesOfHeadAndHeels ();
}

Slide::~Slide( )
{
        removeAllWidgets() ;
}

void Slide::addPictureOfHeadAt ( int x, int y )
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

void Slide::addPictureOfHeelsAt ( int x, int y )
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

void Slide::refreshPicturesOfHeadAndHeels ()
{
        if ( this->pictureOfHead != nilPointer )
        {
                int xHead = this->pictureOfHead->getX ();
                int yHead = this->pictureOfHead->getY ();

                if ( this->pictureOfHead->isOnSomeSlide () )
                        removeWidget( this->pictureOfHead );

                delete this->pictureOfHead ;
                this->pictureOfHead = nilPointer ;

                addPictureOfHeadAt( xHead, yHead );
        }

        if ( this->pictureOfHeels != nilPointer )
        {
                int xHeels = this->pictureOfHeels->getX ();
                int yHeels = this->pictureOfHeels->getY ();

                if ( this->pictureOfHeels->isOnSomeSlide () )
                        removeWidget( this->pictureOfHeels );

                delete this->pictureOfHeels ;
                this->pictureOfHeels = nilPointer ;

                addPictureOfHeelsAt( xHeels, yHeels );
        }
}

void Slide::draw ()
{
        refresh ();
        drawOnGlobalScreen( );
}

void Slide::refresh () const
{
        if ( imageOfSlide == nilPointer ) return ; // nothing to refresh

        const allegro::Pict& previousWhere = allegro::Pict::getWhereToDraw() ;
        allegro::Pict::setWhereToDraw( imageOfSlide->getAllegroPict() );

        imageOfSlide->fillWithColor( Color::byName( "red" ) ); // the red background is so red

        // draw background

        if ( Slide::backgroundPicture == nilPointer )
                Slide::refreshBackground ();

        if ( Slide::backgroundPicture == nilPointer )
                // it's impossible
                throw MayNotBePossible( "Slide::backgroundPicture is nil after Slide::refreshBackground()" ) ;

        if ( Slide::scaledBackgroundPicture == nilPointer )
        {
                const unsigned int screenWidth = GamePreferences::getScreenWidth() ;
                const unsigned int screenHeight = GamePreferences::getScreenHeight() ;

                const unsigned int backgroundWidth = backgroundPicture->getWidth() ;
                const unsigned int backgroundHeight = backgroundPicture->getHeight() ;

                float ratioX = static_cast< float >( screenWidth ) / static_cast< float >( backgroundWidth ) ;
                float ratioY = static_cast< float >( screenHeight ) / static_cast< float >( backgroundHeight ) ;

                std::cout << "scaling the background image to fit the screen" << std::endl ;
                std::cout << "   screen width (" << screenWidth << ")"
                                << " / background width (" << backgroundWidth << ") = " << ratioX << std::endl ;
                std::cout << "   screen height (" << screenHeight << ")"
                                << " / background height (" << backgroundHeight << ") = " << ratioY << std::endl ;

                if ( backgroundWidth == screenWidth && backgroundHeight == screenHeight )
                {
                        Slide::scaledBackgroundPicture = /* just copy */ new Picture( Slide::backgroundPicture->getAllegroPict() );
                }
                else
                {
                        Slide::scaledBackgroundPicture = new Picture( screenWidth, screenHeight, Color::byName( "green" ) );

                        if ( ratioX == ratioY )
                        {
                                allegro::stretchBlit( Slide::backgroundPicture->getAllegroPict(),
                                                      Slide::scaledBackgroundPicture->getAllegroPict(),
                                                                0, 0, backgroundWidth, backgroundHeight,
                                                                0, 0, screenWidth, screenHeight );
                        }
                        else if ( ratioX > ratioY ) /* horizontal over~stretching */
                        {
                                if ( backgroundWidth > 640 ) {
                                        unsigned int scaledWidth = static_cast< unsigned int >( backgroundWidth * ratioX );
                                        unsigned int scaledHeight = static_cast< unsigned int >( backgroundHeight * ratioX );

                                        Picture * uniformlyScaledBackground = new Picture( scaledWidth, scaledHeight, Color::whiteColor() );
                                        allegro::stretchBlit( Slide::backgroundPicture->getAllegroPict(),
                                                              uniformlyScaledBackground->getAllegroPict(),
                                                                        0, 0, backgroundWidth, backgroundHeight,
                                                                        0, 0, scaledWidth, scaledHeight ) ;

                                        // the background will be cropped at the bottom
                                        allegro::bitBlit( uniformlyScaledBackground->getAllegroPict(),
                                                          Slide::scaledBackgroundPicture->getAllegroPict(),
                                                                        0, 0, 0, 0,
                                                                        scaledWidth, screenHeight );

                                        delete uniformlyScaledBackground ;
                                } else {
                                        unsigned int proportionalWidth = static_cast< unsigned int >( backgroundWidth * ratioY );
                                        unsigned int offsetX = ( screenWidth - proportionalWidth ) >> 1 ;

                                        allegro::stretchBlit( Slide::backgroundPicture->getAllegroPict(),
                                                              Slide::scaledBackgroundPicture->getAllegroPict(),
                                                                        0, 0, backgroundWidth, backgroundHeight,
                                                                        offsetX, 0, proportionalWidth, screenHeight );
                                }
                        }
                        else /* if ( ratioY > ratioX ) */ /* vertical over~stretching */
                        {
                                unsigned int proportionalHeight = static_cast< unsigned int >( backgroundHeight * ratioX );
                                unsigned int offsetY = ( screenHeight - proportionalHeight ) >> 1 ;

                                allegro::stretchBlit( Slide::backgroundPicture->getAllegroPict(),
                                                      Slide::scaledBackgroundPicture->getAllegroPict(),
                                                                0, 0, backgroundWidth, backgroundHeight,
                                                                0, offsetY, screenWidth, proportionalHeight );
                        }
                }
        }

        // draw scaled background picture
        allegro::bitBlit( Slide::scaledBackgroundPicture->getAllegroPict() );

        if ( drawSpectrumColors /* || GameManager::getInstance().isSimpleGraphicsSet() */ )
                Slide::draw2x8colors( *this ) ;

        // draw each component
        for ( std::vector< Widget * >::const_iterator wi = widgets.begin () ; wi != widgets.end () ; ++ wi )
                ( *wi )->draw ();

        allegro::Pict::setWhereToDraw( previousWhere );
}

void Slide::drawOnGlobalScreen( )
{
        allegro::bitBlit( imageOfSlide->getAllegroPict(), allegro::Pict::theScreen() );
        allegro::update ();
}

/* static */
void Slide::draw2x8colors ( const Slide & where )
{
        static const size_t howManyColors = 8 ;
        static const std::string colors[ howManyColors ] =
                { "black", "blue", "red", "magenta", "green", "cyan", "yellow", "white" } ;

        const unsigned int sizeOfSquare = 8 ;

        const unsigned int colorsX = where.getImageOfSlide().getWidth () - ( howManyColors * sizeOfSquare );
        const unsigned int colorsY = where.getImageOfSlide().getHeight() - ( 2 * sizeOfSquare );

        for ( size_t i = 0 ; i < howManyColors ; ++ i )
        {
                allegro::fillRect( where.getImageOfSlide().getAllegroPict (),
                                   colorsX + ( i * sizeOfSquare ), colorsY + sizeOfSquare,
                                   colorsX + ( i * sizeOfSquare ) + sizeOfSquare - 1, colorsY + sizeOfSquare + sizeOfSquare - 1,
                                   Color::byName( colors[ i ] ).toAllegroColor () );

                allegro::fillRect( where.getImageOfSlide().getAllegroPict (),
                                   colorsX + ( i * sizeOfSquare ), colorsY,
                                   colorsX + ( i * sizeOfSquare ) + sizeOfSquare - 1, colorsY + sizeOfSquare - 1,
                                   Color::byName( "reduced " + colors[ i ] ).toAllegroColor () );

                /* allegro::fillRect( where.getImageOfSlide().getAllegroPict (),
                                   colorsX + ( i * sizeOfSquare ), colorsY + ( sizeOfSquare << 1 ),
                                   colorsX + ( i * sizeOfSquare ) + sizeOfSquare - 1, colorsY + ( sizeOfSquare << 1 ) + sizeOfSquare - 1,
                                   Color::byName( "light " + colors[ i ] ).toAllegroColor () ); */
        }
}

void Slide::handleKey( const std::string & key )
{
        if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "f" ) ) {
                gui::GuiManager::getInstance().toggleFullScreenVideo ();
                return;
        }

        if ( this->escapeAction != nilPointer && key == "Escape" ) {
                allegro::releaseKey( "Escape" );
                this->escapeAction->doIt ();
        } else
          if ( this->keyHandler != nilPointer )
                this->keyHandler->handleKey( key );
}

void Slide::addWidget( Widget* widget )
{
        if ( widget == nilPointer ) return ;

        this->widgets.push_back( widget );
        widget->setContainingSlide( this );
}

bool Slide::removeWidget( Widget* widget )
{
        for ( std::vector< Widget* >::const_iterator it = widgets.begin() ; it != widgets.end() ; )
        {
                if ( ( *it ) == widget ) {
                        /* it = */ widgets.erase( it );
                        return true ;
                } else
                        ++ it ;
        }

        return false ;
}

void Slide::removeAllWidgets ()
{
        if ( pictureOfHead != nilPointer && ! pictureOfHead->isOnSomeSlide() ) {
                delete pictureOfHead ;
                this->pictureOfHead = nilPointer ;
        }

        if ( pictureOfHeels != nilPointer && ! pictureOfHeels->isOnSomeSlide() ) {
                delete pictureOfHeels ;
                this->pictureOfHeels = nilPointer ;
        }

        this->widgets.clear() ;
}

void Slide::placeHeadAndHeels( bool picturesToo, bool copyrightsToo )
{
        const unsigned int screenWidth = GamePreferences::getScreenWidth();
        const unsigned int space = ( screenWidth / 20 ) - 10;

        Label* Head = new Label( "Head", new Font( "yellow", true ) );
        Label* over = new Label( "over", new Font( "white" ), /* multicolor */ true );
        Label* Heels = new Label( "Heels", new Font( "yellow", true ) );

        over->moveTo( ( screenWidth - over->getWidth() - 20 ) >> 1, space + Head->getHeight() - over->getHeight() - 8 );
        addWidget( over );

        unsigned int widthOfSpace = over->getFont().getWidthOfLetter( " " );
        int spaceWithoutSpacing = widthOfSpace - over->getSpacing ();

        Head->moveTo( over->getX() - Head->getWidth() - spaceWithoutSpacing, space );
        addWidget( Head );

        Heels->moveTo( over->getX() + over->getWidth() + spaceWithoutSpacing, space );
        addWidget( Heels );

        TextField* JonRitman = new TextField( screenWidth >> 2, "center" ) ;
        JonRitman->appendLine( "Jon", false, "multicolor" );
        JonRitman->appendLine( "Ritman", false, "multicolor" );
        JonRitman->moveTo( Head->getX() - JonRitman->getWidthOfField() - space, space );
        addWidget( JonRitman );

        TextField* BernieDrummond = new TextField( screenWidth >> 2, "center" ) ;
        BernieDrummond->appendLine( "Bernie", false, "multicolor" );
        BernieDrummond->appendLine( "Drummond", false, "multicolor" );
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
                Label* Jorge = new Label( "{ 2009 Jorge Rodríguez Santos", new Font( "orange" ) );
                Label* Douglas = new Label( "{ 2024 Douglas Mencken", new Font( "yellow" ) );

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
void Slide::scrollHorizontally( const Slide & oldSlide, const Slide & newSlide, bool rightToLeft )
{
        if ( &oldSlide == &newSlide ||
                oldSlide.imageOfSlide == nilPointer || newSlide.imageOfSlide == nilPointer ) return ;

        Picture buffer( * oldSlide.imageOfSlide );

        autouniqueptr < Timer > drawTimer( new Timer() );
        drawTimer->go ();

        newSlide.refresh ();
        Picture& newPicture = * newSlide.imageOfSlide ;

        unsigned int step = ( ( ( GamePreferences::getScreenWidth() >> 6 ) + 5 ) / 10 ) << 1 ;

        for ( unsigned int x = step ; x < GamePreferences::getScreenWidth() ; x += step )
        {
                /* bitBlit( from, to, fromX, fromY, toX, toY, width, height ) */

                if ( rightToLeft )
                        allegro::bitBlit( newPicture.getAllegroPict(), buffer.getAllegroPict(),
                                                0, 0, oldSlide.imageOfSlide->getWidth() - x, 0, x, GamePreferences::getScreenHeight() );
                else
                        allegro::bitBlit( newPicture.getAllegroPict(), buffer.getAllegroPict(),
                                                newPicture.getWidth() - x, 0, 0, 0, x, GamePreferences::getScreenHeight() );

                if ( drawTimer->getValue() > 0.02 ) // every 20 milliseconds, that’s 1/50 of a second
                {
                        allegro::bitBlit( buffer.getAllegroPict(), allegro::Pict::theScreen() );
                        allegro::update ();
                        drawTimer->go() ;
                }

                somn::milliSleep( 1 );
        }
}

/* static */
void Slide::wipeHorizontally( const Slide & oldSlide, const Slide & newSlide, bool rightToLeft )
{
        if ( &oldSlide == &newSlide ||
                oldSlide.imageOfSlide == nilPointer || newSlide.imageOfSlide == nilPointer ) return ;

        Picture buffer( * oldSlide.imageOfSlide );

        autouniqueptr < Timer > drawTimer( new Timer() );
        drawTimer->go ();

        newSlide.refresh ();
        Picture& newPicture = * newSlide.imageOfSlide ;

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
                        drawTimer->go() ;
                }

                somn::milliSleep( 1 );
        }
}

/* static */
void Slide::barWipeHorizontally( const Slide & oldSlide, const Slide & newSlide, bool rightToLeft )
{
        if ( &oldSlide == &newSlide ||
                oldSlide.imageOfSlide == nilPointer || newSlide.imageOfSlide == nilPointer ) return ;

        Picture buffer( * oldSlide.imageOfSlide );

        autouniqueptr < Timer > drawTimer( new Timer() );
        drawTimer->go ();

        newSlide.refresh ();
        Picture& newPicture = * newSlide.imageOfSlide ;

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
                        drawTimer->go() ;
                }

                somn::milliSleep( 2 );
        }
}

/* static */
void Slide::randomPixelFade( bool fadeIn, const Slide & slide, const Color & color )
{
        if ( slide.imageOfSlide == nilPointer ) return;

        // refresh the slide before fading
        slide.refresh();

        AllegroColor aColor = color.toAllegroColor() ;

        if ( fadeIn )
        {
                allegro::Pict::theScreen().clearToColor( aColor );
                allegro::update();
        }

        const unsigned int screenWidth = slide.imageOfSlide->getWidth() ;
        const unsigned int screenHeight = slide.imageOfSlide->getHeight() ;

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
                                buffer.getAllegroPict().drawPixelAt( x, y, slide.imageOfSlide->getPixelAt( x, y ).toAllegroColor() );
                        else
                                buffer.getAllegroPict().drawPixelAt( x, y, aColor );

                        if ( yet % chunk == 0 ) // after painting a chunk
                                somn::milliSleep( 1 ); // wait a millisecond

                        if ( drawTimer->getValue() > 0.02 ) // every 20 milliseconds, that’s 1/50 of a second
                        {
                                allegro::bitBlit( buffer.getAllegroPict(), allegro::Pict::theScreen() );
                                allegro::update ();
                                drawTimer->go() ;
                        }

                        bits[ x + y * screenWidth ] = true;
                        yet ++;
                }
        }
}

}

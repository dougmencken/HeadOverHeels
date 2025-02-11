
#include "Slide.hpp"

#include "GuiManager.hpp"
#include "GameManager.hpp"
#include "GamePreferences.hpp"
#include "Color.hpp"
#include "Label.hpp"
#include "Menu.hpp"

#include "sleep.hpp"
#include "ospaths.hpp"

#include "MayNotBePossible.hpp"

#include <iostream>


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
}

Slide::Slide() :
        Widget( 0, 0 ),
        imageOfSlide( new Picture( GamePreferences::getScreenWidth(), GamePreferences::getScreenHeight() ) ),
        noTransitionFrom( false ),
        noTransitionTo( false ),
        drawSpectrumColors( false ),
        escapeAction( nilPointer ),
        keyHandler( nilPointer )
{
        refresh () ;
}

Slide::~Slide( )
{
        removeAllWidgets() ;
}

void Slide::draw ()
{
        if ( this->imageOfSlide == nilPointer ) return ; // nothing to draw

        const allegro::Pict& previousWhere = allegro::Pict::getWhereToDraw() ;
        allegro::Pict::setWhereToDraw( imageOfSlide->getAllegroPict() );

        imageOfSlide->fillWithColor( Color::byName( "red" ) ); // the red background is so red

        // draw scaled background picture
        allegro::bitBlit( Slide::scaledBackgroundPicture->getAllegroPict() );

        if ( drawSpectrumColors )
                Slide::draw2x8colors( *this ) ;

        // draw each widget
        for ( std::vector< Widget * >::const_iterator wi = widgets.begin () ; wi != widgets.end () ; ++ wi )
                ( *wi )->draw ();

        allegro::Pict::setWhereToDraw( previousWhere );

        drawOnGlobalScreen( );
}

void Slide::drawOnGlobalScreen ()
{
        if ( GameManager::isThreadRunning () ) return ;
        if ( this->imageOfSlide == nilPointer ) return ; // nothing to draw

        allegro::bitBlit( this->imageOfSlide->getAllegroPict(), allegro::Pict::theScreen() );
        allegro::update ();
}

void Slide::refresh ()
{
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
                                }
                                else {
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

        if ( Slide::scaledBackgroundPicture == nilPointer )
                throw MayNotBePossible( "Slide::scaledBackgroundPicture is nil at the end of Slide::refresh()" ) ;
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
        this->widgets.clear() ;
}

/* static */
void Slide::scrollHorizontally( const Slide & oldSlide, const Slide & newSlide, bool rightToLeft )
{
        if ( &oldSlide == &newSlide ||
                oldSlide.imageOfSlide == nilPointer || newSlide.imageOfSlide == nilPointer ) return ;

        Picture buffer( * oldSlide.imageOfSlide );

        autouniqueptr < Timer > drawTimer( new Timer() );
        drawTimer->go ();

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
        if ( slide.imageOfSlide == nilPointer ) return ;

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

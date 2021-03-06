
#include "GameManager.hpp"
#include "Mediator.hpp"
#include "PlayerItem.hpp"
#include "PictureWidget.hpp"
#include "Label.hpp"
#include "ColorCyclingLabel.hpp"
#include "Camera.hpp"
#include "GuiManager.hpp"
#include "MapManager.hpp"
#include "InputManager.hpp"
#include "BonusManager.hpp"
#include "SoundManager.hpp"
#include "InputManager.hpp"
#include "LanguageText.hpp"
#include "LanguageManager.hpp"
#include "Color.hpp"

#include <tinyxml2.h>


namespace iso
{

GameManager GameManager::instance ;


GameManager::GameManager( )
        : headRoom( "blacktooth1head.xml" )
        , heelsRoom( "blacktooth23heels.xml" )
        , freedomLabel( nilPointer )
        , numberOfCapture( 0 )
        , prefixOfCaptures( makeRandomString( 10 ) )
        , capturePath( iso::homePath() + "capture" )
        , chosenGraphicSet( "gfx" )
        , vidasInfinitas( false )
        , immunityToCollisions( false )
        , noFallingDown( false )
        , playTuneOfScenery( true )
        , castShadows( true )
        , drawBackgroundPicture( true )
        , recordCaptures( false )
        , recordingTimer( new Timer () )
        , isomot( )
        , saverAndLoader( )
        , headLives( 0 )
        , heelsLives( 0 )
        , highSpeed( 0 )
        , highJumps( 0 )
        , headShield( 0 )
        , heelsShield( 0 )
        , horn( false )
        , handbag( false )
        , donuts( 0 )
        , takenCrown( false )
        , eatenFish( false )
        , gameOver( false )
        , freedom( false )
        , emperator( false )
{
        if ( ! util::folderExists( capturePath ) )
                mkdir( capturePath.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH );

        recordingTimer->go ();
}

GameManager::~GameManager( )
{
        cleanUp () ;
}

void GameManager::cleanUp ()
{
        sceneryBackgrounds.clear ();

        ambiancePictures.clear ();

        isomot.binView ();
        isomot.getMapManager().clear ();

        Item::getPoolOfPictures().clear ();

        delete freedomLabel ;
}

/* static */
GameManager& GameManager::getInstance ()
{
        return instance ;
}

/* static */
PicturePtr GameManager::refreshPicture ( const std::string& nameOfPicture )
{
        IF_DEBUG( std::cout << "refreshing picture \"" << nameOfPicture << "\"" << std::endl )

        autouniqueptr< allegro::Pict > pict( allegro::Pict::fromPNGFile (
                iso::pathToFile( gui::GuiManager::getInstance().getPathToPicturesOfGui(), nameOfPicture )
        ) ) ;

        PicturePtr newPicture( new Picture( *pict ) );
        newPicture->setName( nameOfPicture );

        return newPicture ;
}

WhyPaused GameManager::begin ()
{
        IF_DEBUG( fprintf ( stdout, "GameManager::begin ()\n" ) )

        this->vidasInfinitas = false;
        this->headLives = 8;
        this->heelsLives = 8;
        this->highSpeed = 0;
        this->highJumps = 0;
        this->headShield = 0;
        this->heelsShield = 0;
        this->horn = false;
        this->handbag = false;
        this->donuts = 0;
        this->imageOfItemInBag.clear ();
        this->planets.clear ();

        refreshAmbianceImages ();
        refreshSceneryBackgrounds ();

        isomot.beginNewGame ();

        return this->update ();
}

WhyPaused GameManager::update ()
{
        IF_DEBUG( fprintf ( stdout, "GameManager::update ()\n" ) )

        WhyPaused why = WhyPaused::Nevermind ;

        while ( why == WhyPaused::Nevermind )
        {
                if ( ! InputManager::getInstance().pauseTyped () && ! this->takenCrown && ! this->eatenFish && ! this->gameOver )
                {
                        if ( headLives > 0 || heelsLives > 0 )
                        {
                                if ( isomot.getMapManager().getActiveRoom() != nilPointer )
                                {
                                        // actualiza la vista isométrica
                                        Picture* view = isomot.updateMe ();

                                        if ( view != nilPointer )
                                        {
                                                drawAmbianceOfGame( view->getAllegroPict() );
                                                drawOnScreen( view->getAllegroPict() );
                                        }
                                        else
                                        {
                                                autouniqueptr< allegro::Pict > nothing ( allegro::Pict::newPict( ScreenWidth(), ScreenHeight() ) );
                                                nothing->clearToColor( Color::byName( "gray75" ).toAllegroColor() );
                                                drawAmbianceOfGame( *nothing );
                                                drawOnScreen( *nothing );
                                        }

                                        milliSleep( 1000 / GameManager::updatesPerSecond );
                                }
                                else
                                {
                                        std::cout << "there’s no active room, game over" << std::endl ;
                                        this->gameOver = true ;
                                }
                        }
                        else
                        {
                                std::cout << "lives are exhausted, game over" << std::endl ;
                                this->gameOver = true ;
                        }
                }
                else
                {
                        why = this->pause ();
                }
        }

        return why;
}

WhyPaused GameManager::pause ()
{
        bool exit = false;
        bool confirm = false;
        WhyPaused why = WhyPaused::Nevermind ;

        // detiene el motor isométrico
        isomot.pause();
        Picture* view = isomot.updateMe ();
        allegro::bitBlit( view->getAllegroPict(), allegro::Pict::theScreen() );
        allegro::update ();

        allegro::emptyKeyboardBuffer();

        const allegro::Pict& previousWhere = allegro::Pict::getWhereToDraw() ;
        allegro::Pict::setWhereToDraw( view->getAllegroPict() );

        // the user just released some planet
        if ( this->takenCrown )
        {
                why = WhyPaused::FreePlanet ;
                this->takenCrown = false;
        }
        // the user has just eaten reincarnation fish
        else if ( this->eatenFish )
        {
                this->eatenFish = false;

                gui::LanguageManager* language = gui::GuiManager::getInstance().getLanguageManager();
                gui::LanguageText* text = language->findLanguageStringForAlias( "save-game" );
                int deltaY = ( iso::ScreenHeight() >> 2 ) - 60 ;

                for ( size_t i = 0; i < text->getLinesCount(); i++ )
                {
                        gui::LanguageLine* line = text->getLine( i );
                        gui::Label label( line->text, line->font, line->color );
                        label.moveTo( ( ScreenWidth() - label.getWidth() ) >> 1, deltaY );
                        deltaY += label.getHeight() * 3 / 4;
                        label.draw ();
                }

                text = language->findLanguageStringForAlias( "confirm-resume" );
                deltaY += 20;

                for ( size_t i = 0; i < text->getLinesCount(); i++ )
                {
                        gui::LanguageLine* line = text->getLine( i );
                        gui::Label label( line->text, line->font, line->color );
                        label.moveTo( ( ScreenWidth() - label.getWidth() ) >> 1, deltaY );
                        deltaY += label.getHeight() * 3 / 4;
                        label.draw ();
                }

                allegro::emptyKeyboardBuffer();

                // mientras el usuario no elija una de las dos opciones no se hará nada
                while ( ! confirm && ! exit )
                {
                        allegro::bitBlit( view->getAllegroPict(), allegro::Pict::theScreen() );
                        allegro::update ();

                        if ( allegro::areKeypushesWaiting() )
                        {
                                iso::InputManager& inputManager = InputManager::getInstance();

                                std::string key = allegro::nextKey();

                                // si se pulsa Space se mostrará la interfaz para la grabación de la partida
                                if ( key == "Space" )
                                {
                                        exit = true;
                                        why = WhyPaused::SaveGame ;
                                }
                                else if ( key != inputManager.getUserKeyFor( "movenorth" ) &&
                                          key != inputManager.getUserKeyFor( "movesouth" ) &&
                                          key != inputManager.getUserKeyFor( "moveeast" ) &&
                                          key != inputManager.getUserKeyFor( "movewest" ) &&
                                          key != inputManager.getUserKeyFor( "jump" ) &&
                                          key != inputManager.getUserKeyFor( "take" ) &&
                                          key != inputManager.getUserKeyFor( "take&jump" ) &&
                                          key != inputManager.getUserKeyFor( "swap" ) &&
                                          key != inputManager.getUserKeyFor( "doughnut" ) &&
                                          key != inputManager.getUserKeyFor( "pause" ) &&
                                          key != "Escape" )
                                {
                                        confirm = true;
                                        isomot.resume();
                                }
                        }

                        milliSleep( 100 );
                }
        }
        // the user exhausted lives of both characters
        else if ( this->gameOver )
        {
                this->gameOver = false;
                why = WhyPaused::GameOver ;
        }
        // the user reached Freedom but with not every crown
        else if ( this->freedom )
        {
                this->freedom = false;
                why = WhyPaused::Freedom ;
        }
        // the user completed this game
        else if ( this->emperator )
        {
                this->emperator = false;
                why = WhyPaused::Success ;
        }
        // the user typed key for pause
        else
        {
                SoundManager::getInstance().stopEverySound ();

                gui::LanguageManager* language = gui::GuiManager::getInstance().getLanguageManager();
                gui::LanguageText* text = language->findLanguageStringForAlias( "confirm-quit" );
                int deltaY = ( iso::ScreenHeight() >> 2 );

                for ( size_t i = 0; i < text->getLinesCount(); i++ )
                {
                        gui::LanguageLine* line = text->getLine( i );
                        gui::Label label( line->text, line->font, line->color );
                        label.moveTo( ( ScreenWidth() - label.getWidth() ) >> 1, deltaY );
                        deltaY += label.getHeight() * 3 / 4;
                        label.draw ();
                }

                text = language->findLanguageStringForAlias( "confirm-resume" );
                deltaY += 20;

                for ( size_t i = 0; i < text->getLinesCount(); i++ )
                {
                        gui::LanguageLine* line = text->getLine( i );
                        gui::Label label( line->text, line->font, line->color );
                        label.moveTo( ( ScreenWidth() - label.getWidth() ) >> 1, deltaY );
                        deltaY += label.getHeight() * 3 / 4;
                        label.draw ();
                }

                while ( ! confirm && ! exit )
                {
                        allegro::bitBlit( view->getAllegroPict(), allegro::Pict::theScreen() );
                        allegro::update ();

                        if ( allegro::areKeypushesWaiting() )
                        {
                                if ( allegro::nextKey() == "Escape" )
                                {
                                        exit = true;
                                        why = WhyPaused::GameOver ;
                                }
                                else
                                {
                                        confirm = true;
                                        isomot.resume();
                                }
                        }

                        milliSleep( 100 );
                }
        }

        allegro::Pict::setWhereToDraw( previousWhere );

        return why;
}

WhyPaused GameManager::resume ()
{
        IF_DEBUG( fprintf ( stdout, "GameManager::resume ()\n" ) )

        refreshAmbianceImages ();
        refreshSceneryBackgrounds ();

        isomot.resume ();

        return this->update ();
}

void GameManager::refreshSceneryBackgrounds ()
{
        sceneryBackgrounds[ "jail" ] = refreshPicture( "jail-frame.png" );
        sceneryBackgrounds[ "blacktooth" ] = refreshPicture( "blacktooth-frame.png" );
        sceneryBackgrounds[ "market" ] = refreshPicture( "market-frame.png" );
        sceneryBackgrounds[ "moon" ] = refreshPicture( "moon-frame.png" );
        sceneryBackgrounds[ "safari" ] = refreshPicture( "safari-frame.png" );
        sceneryBackgrounds[ "byblos" ] = refreshPicture( "byblos-frame.png" );
        sceneryBackgrounds[ "penitentiary" ] = refreshPicture( "penitentiary-frame.png" );
        sceneryBackgrounds[ "egyptus" ] = refreshPicture( "egyptus-frame.png" );
}

void GameManager::refreshAmbianceImages ()
{
        ambiancePictures[ "head" ] = refreshPicture( "gui-head.png" );
        ambiancePictures[ "heels" ] = refreshPicture( "gui-heels.png" );

        ambiancePictures[ "gray head" ] = PicturePtr( new Picture( * ambiancePictures[ "head" ] ) );
        ambiancePictures[ "gray heels" ] = PicturePtr( new Picture( * ambiancePictures[ "heels" ] ) );

        if ( ! isSimpleGraphicSet () )
        {
                ambiancePictures[ "gray head" ]->toGrayscale();
                ambiancePictures[ "gray heels" ]->toGrayscale();
        }
        else
        {
                ambiancePictures[ "gray head" ]->colorize( Color::byName( "reduced magenta" ) );
                ambiancePictures[ "gray heels" ]->colorize( Color::byName( "reduced magenta" ) );
        }

        ambiancePictures[ "handbag" ] = refreshPicture( "gui-handbag.png" );
        ambiancePictures[ "horn" ] = refreshPicture( "gui-horn.png" );
        ambiancePictures[ "donuts" ] = refreshPicture( "gui-donuts.png" );

        ambiancePictures[ "gray handbag" ] = PicturePtr( new Picture( * ambiancePictures[ "handbag" ] ) );
        ambiancePictures[ "gray horn" ] = PicturePtr( new Picture( * ambiancePictures[ "horn" ] ) );
        ambiancePictures[ "gray donuts" ] = PicturePtr( new Picture( * ambiancePictures[ "donuts" ] ) );

        if ( ! isSimpleGraphicSet () )
        {
                ambiancePictures[ "gray handbag" ]->toGrayscale();
                ambiancePictures[ "gray horn" ]->toGrayscale();
                ambiancePictures[ "gray donuts" ]->toGrayscale();
        }
        else
        {
                ambiancePictures[ "gray handbag" ]->colorize( Color::byName( "reduced magenta" ) );
                ambiancePictures[ "gray horn" ]->colorize( Color::byName( "reduced magenta" ) );
                ambiancePictures[ "gray donuts" ]->colorize( Color::byName( "reduced magenta" ) );

                ambiancePictures[ "handbag" ]->colorize( Color::byName( "yellow" ) );
                ambiancePictures[ "horn" ]->colorize( Color::byName( "yellow" ) );
                ambiancePictures[ "donuts" ]->colorize( Color::byName( "yellow" ) );
        }

        ambiancePictures[ "grandes saltos" ] = refreshPicture( "high-jumps.png" );
        ambiancePictures[ "gran velocidad" ] = refreshPicture( "high-speed.png" );
        ambiancePictures[ "escudo" ] = refreshPicture( "shield.png" );

        ambiancePictures[ "gray grandes saltos" ] = PicturePtr( new Picture( * ambiancePictures[ "grandes saltos" ] ) );
        ambiancePictures[ "gray gran velocidad" ] = PicturePtr( new Picture( * ambiancePictures[ "gran velocidad" ] ) );
        ambiancePictures[ "gray escudo" ] = PicturePtr( new Picture( * ambiancePictures[ "escudo" ] ) );

        if ( ! isSimpleGraphicSet () )
        {
                ambiancePictures[ "gray grandes saltos" ]->toGrayscale();
                ambiancePictures[ "gray gran velocidad" ]->toGrayscale();
                ambiancePictures[ "gray escudo" ]->toGrayscale();
        }
        else
        {
                ambiancePictures[ "gray grandes saltos" ]->colorize( Color::byName( "reduced magenta" ) );
                ambiancePictures[ "grandes saltos" ]->colorize( Color::byName( "yellow" ) );

                ambiancePictures[ "gray gran velocidad" ]->colorize( Color::byName( "reduced magenta" ) );
                ambiancePictures[ "gran velocidad" ]->colorize( Color::byName( "yellow" ) );

                ambiancePictures[ "gray escudo" ]->colorize( Color::byName( "reduced magenta" ) );
                ambiancePictures[ "escudo" ]->colorize( Color::byName( "yellow" ) );
        }
}

void GameManager::drawAmbianceOfGame ( const allegro::Pict& where )
{
        const allegro::Pict& previousWhere = allegro::Pict::getWhereToDraw() ;
        allegro::Pict::setWhereToDraw( where );

        // scenery of this room
        std::string scenery = isomot.getMapManager().getActiveRoom()->getScenery();

        // empty scenery means that it is the final room
        if ( scenery != "" )
        {
                const unsigned int diffX = iso::ScreenWidth() - 640 ;
                const unsigned int diffY = iso::ScreenHeight() - 480 ;

                if ( ( diffX > 0 || diffY > 0 ) && ! isPresentGraphicSet() && ! isSimpleGraphicSet() )
                        drawBackgroundPicture = false ;

                if ( drawBackgroundPicture ) // marco, varía en función del escenario
                {
                        if ( sceneryBackgrounds.find( scenery ) != sceneryBackgrounds.end () )
                                if ( sceneryBackgrounds[ scenery ] != nilPointer )
                                        allegro::drawSprite( sceneryBackgrounds[ scenery ]->getAllegroPict(), diffX >> 1, diffY );
                }

                const unsigned int headHeelsAmbianceY = 425 + diffY ;
                const unsigned int charStuffAmbianceY = 361 + diffY ;
                const unsigned int bonusAmbianceY = 392 + diffY ;
                const unsigned int immunityAmbianceY = 436 + diffY ;

                const unsigned int dx = diffX >> 1 ;
                const unsigned int leftAmbianceX = 107 + dx ;
                const unsigned int rightAmbianceX = 505 + dx ;
                const unsigned int leftTooAmbianceX = 33 + dx ;
                const unsigned int rightTooAmbianceX = 559 + dx ;

                std::string player = isomot.getMapManager().getActiveRoom()->getMediator()->getLabelOfActiveCharacter();
                allegro::drawSprite (
                        ( (  player == "head" || player == "headoverheels" ) ? ambiancePictures[ "head" ] : ambiancePictures[ "gray head" ] )->getAllegroPict(),
                        161 + dx, headHeelsAmbianceY );
                allegro::drawSprite (
                        ( ( player == "heels" || player == "headoverheels" ) ? ambiancePictures[ "heels" ] : ambiancePictures[ "gray heels" ] )->getAllegroPict(),
                        431 + dx, headHeelsAmbianceY );

                allegro::drawSprite(
                        ( this->horn ? ambiancePictures[ "horn" ] : ambiancePictures[ "gray horn" ] )->getAllegroPict(),
                        leftTooAmbianceX, headHeelsAmbianceY );

                allegro::drawSprite(
                        ( this->handbag ? ambiancePictures[ "handbag" ] : ambiancePictures[ "gray handbag" ] )->getAllegroPict(),
                        rightTooAmbianceX, headHeelsAmbianceY );

                std::string colorOfLabels = "white";
                /* if ( isSimpleGraphicSet () ) colorOfLabels = "magenta"; */

                // vidas de Head
                gui::Label headLivesLabel( util::number2string( this->headLives ), "big", "white", -2 );
                headLivesLabel.moveTo( ( this->headLives > 9 ? 214 : 221 ) + dx, headHeelsAmbianceY - 1 );
                headLivesLabel.draw ();

                // vidas de Heels
                gui::Label heelsLivesLabel( util::number2string( this->heelsLives ), "big", "white", -2 );
                heelsLivesLabel.moveTo( ( this->heelsLives > 9 ? 398 : 405 ) + dx, headHeelsAmbianceY - 1 );
                heelsLivesLabel.draw ();

                // número de rosquillas
                allegro::drawSprite(
                        ( this->donuts != 0 ? ambiancePictures[ "donuts" ] : ambiancePictures[ "gray donuts" ] )->getAllegroPict(),
                        leftTooAmbianceX, charStuffAmbianceY );
                if ( this->donuts > 0 )
                {
                        gui::Label donutsLabel( util::number2string( this->donuts ), "", colorOfLabels, -2 );
                        donutsLabel.moveTo( ( this->donuts > 9 ? 42 : 49 ) + dx, charStuffAmbianceY + 11 );
                        donutsLabel.draw ();
                }

                // grandes saltos
                allegro::drawSprite(
                        ( this->highJumps > 0 ? ambiancePictures[ "grandes saltos" ] : ambiancePictures[ "gray grandes saltos" ] )->getAllegroPict(),
                        rightAmbianceX, bonusAmbianceY );
                if ( this->highJumps > 0 )
                {
                        gui::Label highJumpsLabel( util::number2string( this->highJumps ), "", colorOfLabels, -2 );
                        highJumpsLabel.moveTo( ( this->highJumps > 9 ? 505 : 512 ) + dx, bonusAmbianceY + 1 );
                        highJumpsLabel.draw ();
                }

                // gran velocidad
                allegro::drawSprite(
                        ( this->highSpeed > 0 ? ambiancePictures[ "gran velocidad" ] : ambiancePictures[ "gray gran velocidad" ] )->getAllegroPict(),
                        leftAmbianceX, bonusAmbianceY );
                if ( this->highSpeed > 0 )
                {
                        gui::Label highSpeedLabel( util::number2string( this->highSpeed ), "", colorOfLabels, -2 );
                        highSpeedLabel.moveTo( ( this->highSpeed > 9 ? 107 : 114 ) + dx, bonusAmbianceY + 1 );
                        highSpeedLabel.draw ();
                }

                // escudo de Head
                allegro::drawSprite(
                        ( this->headShield > 0 ? ambiancePictures[ "escudo" ] : ambiancePictures[ "gray escudo" ] )->getAllegroPict(),
                        leftAmbianceX, immunityAmbianceY );
                if ( this->headShield > 0 )
                {
                        int headShieldValue = static_cast< int >( this->headShield * 99.0 / 25.0 );
                        gui::Label headShieldLabel( util::number2string( headShieldValue ), "", colorOfLabels, -2 );
                        headShieldLabel.moveTo( ( headShieldValue > 9 ? 107 : 114 ) + dx, immunityAmbianceY + 1 );
                        headShieldLabel.draw ();
                }

                // escudo de Heels
                allegro::drawSprite(
                        ( this->heelsShield > 0 ? ambiancePictures[ "escudo" ] : ambiancePictures[ "gray escudo" ] )->getAllegroPict(),
                        rightAmbianceX, immunityAmbianceY );
                if ( this->heelsShield > 0 )
                {
                        int heelsShieldValue = static_cast< int >( this->heelsShield * 99.0 / 25.0 );
                        gui::Label heelsShieldLabel( util::number2string( heelsShieldValue ), "", colorOfLabels, -2 );
                        heelsShieldLabel.moveTo( ( heelsShieldValue > 9 ? 505 : 512 ) + dx, immunityAmbianceY + 1 );
                        heelsShieldLabel.draw ();
                }

                // item in handbag
                if ( this->imageOfItemInBag != nilPointer )
                {
                        allegro::drawSprite( imageOfItemInBag->getAllegroPict(), rightTooAmbianceX, charStuffAmbianceY );
                }
        }
        else
        {
                // draw color~cycling FREEDOM on the final scene
                if ( freedomLabel == nilPointer )
                {
                        freedomLabel = new gui::ColorCyclingLabel( "FREEDOM", "big" );
                        freedomLabel->moveTo( ScreenWidth() / 10, ScreenHeight() - 100 );
                }

                freedomLabel->draw ();
        }

        if ( ! hasBackgroundPicture () )
        {
                // show position of camera

                const unsigned short widthOfChar = 8 ; // letters of allegro’s font are 8 x 7
                const unsigned short deltaYtext = 36 ;

                if ( isomot.getMapManager().getActiveRoom() != nilPointer )
                {
                        Color backgroundColor = Color::blackColor();
                        if ( charactersFly() ) backgroundColor = Color::byName( "dark blue" );

                        allegro::fillRect( ( where.getW() - 8 * widthOfChar ) >> 1, where.getH() - deltaYtext - 1,
                                           ( where.getW() + 8 * widthOfChar ) >> 1, where.getH() - deltaYtext + 8,
                                           backgroundColor.toAllegroColor() );

                        allegro::textOut( "camera",
                                          ( where.getW() - 6 * widthOfChar ) >> 1, where.getH() - deltaYtext,
                                          Color::byName( "75% gray" ).toAllegroColor() );

                        const int cameraDeltaX = isomot.getMapManager().getActiveRoom()->getCamera()->getDeltaX();
                        const int cameraDeltaY = isomot.getMapManager().getActiveRoom()->getCamera()->getDeltaY();

                        std::string xCamera = util::number2string( cameraDeltaX );
                        if ( cameraDeltaX > 0 ) xCamera = "+" + xCamera ;
                        std::string yCamera = util::number2string( cameraDeltaY );
                        if ( cameraDeltaY > 0 ) yCamera = "+" + yCamera ;
                        std::string xyCamera = xCamera + "," + yCamera ;

                        allegro::fillRect( ( where.getW() - ( xyCamera.length() + 2 ) * widthOfChar ) >> 1, where.getH() - deltaYtext + 10 - 1,
                                           ( where.getW() + ( xyCamera.length() + 2 ) * widthOfChar ) >> 1, where.getH() - deltaYtext + 10 + 8 + 1,
                                           backgroundColor.toAllegroColor() );

                        const int textX = ( where.getW() - xyCamera.length() * widthOfChar ) >> 1 ;
                        const int textY = where.getH() - deltaYtext + 10 ;

                        allegro::textOut( xCamera, textX, textY,
                                          isomot.doesCameraFollowCharacter() ? Color::byName( "gray" ).toAllegroColor() : Color::whiteColor().toAllegroColor() );
                        allegro::textOut( "," , textX + widthOfChar * xCamera.length(), textY,
                                          isomot.doesCameraFollowCharacter() ? Color::byName( "gray 25%" ).toAllegroColor() : Color::byName( "gray" ).toAllegroColor() );
                        allegro::textOut( yCamera, textX + widthOfChar * ( xCamera.length() + 1 ), textY,
                                          isomot.doesCameraFollowCharacter() ? Color::byName( "gray" ).toAllegroColor() : Color::whiteColor().toAllegroColor() );
                }
        }

        allegro::Pict::setWhereToDraw( previousWhere );
}

void GameManager::drawOnScreen ( const allegro::Pict& view )
{
        if ( recordCaptures )
        {
                // record game’s screen
                if ( recordingTimer->getValue() > 0.1 /* 10 times per one second */ )
                {
                        numberOfCapture++ ;

                        allegro::savePictAsPCX( capturePath + util::pathSeparator() + prefixOfCaptures + util::number2string( numberOfCapture ), view );

                        recordingTimer->reset ();
                }
        }

        allegro::Pict::resetWhereToDraw();

        allegro::bitBlit( view, allegro::Pict::theScreen() );

        if ( recordCaptures )
        {
                allegro::fillRect( 19, 18, 80, 31, Color::byName( "75% gray" ).toAllegroColor() );
                allegro::fillCircle( 34, 24, 5, Color::byName( "red" ).toAllegroColor() );
                allegro::textOut( "REC", 48, 21, Color::byName( "red" ).toAllegroColor() );
        }

        allegro::update ();
}

void GameManager::loadGame ( const std::string& fileName )
{
        isomot.fillItemDescriptions ();
        saverAndLoader.loadGame( fileName );
}

void GameManager::saveGame ( const std::string& fileName )
{
        saverAndLoader.saveGame( fileName );
}

void GameManager::addLives ( const std::string& player, unsigned char lives )
{
        if ( player == "head" )
        {
                if ( this->headLives + lives < 100 )
                {
                        this->headLives += lives;
                }
        }
        else if ( player == "heels" )
        {
                if ( this->heelsLives + lives < 100 )
                {
                        this->heelsLives += lives;
                }
        }
        else if ( player == "headoverheels" )
        {
                if ( this->headLives + lives < 100 )
                {
                        this->headLives += lives;
                }

                if ( this->heelsLives + lives < 100 )
                {
                        this->heelsLives += lives;
                }
        }
}

void GameManager::loseLife ( const std::string& player )
{
        if ( ! vidasInfinitas )
        {
                if ( player == "head" )
                {
                        if ( this->headLives > 0 )
                        {
                                this->headLives--;
                        }
                }
                else if ( player == "heels" )
                {
                        if ( this->heelsLives > 0 )
                        {
                                this->heelsLives--;
                        }
                }
                else if ( player == "headoverheels" )
                {
                        if ( this->headLives > 0 )
                        {
                                this->headLives--;
                        }
                        if ( this->heelsLives > 0 )
                        {
                                this->heelsLives--;
                        }
                }
        }
}

void GameManager::takeMagicItem ( const std::string& label )
{
        if ( label == "horn" )
        {
                this->horn = true;
        }
        else if ( label == "handbag" )
        {
                this->handbag = true;
        }
}

void GameManager::addHighSpeed ( const std::string& player, unsigned int highSpeed )
{
        if ( player == "head" || player == "headoverheels" )
        {
                this->highSpeed += highSpeed;
                if ( this->highSpeed > 99 )
                {
                        this->highSpeed = 99;
                }
        }
}

void GameManager::decreaseHighSpeed ( const std::string& player )
{
        if ( player == "head" || player == "headoverheels" )
        {
                if ( this->highSpeed > 0 )
                {
                        this->highSpeed--;
                }
        }
}

void GameManager::addHighJumps ( const std::string& player, unsigned int highJumps )
{
        if ( player == "heels" || player == "headoverheels" )
        {
                this->highJumps += highJumps;
                if ( this->highJumps > 99 )
                {
                        this->highJumps = 99;
                }
        }
}

void GameManager::decreaseHighJumps ( const std::string& player )
{
        if ( player == "heels" || player == "headoverheels" )
        {
                if ( this->highJumps > 0 )
                {
                        this->highJumps--;
                }
        }
}

void GameManager::addShield ( const std::string& player, float shield )
{
        if ( player == "head" )
        {
                this->headShield += shield;
                if ( this->headShield > 25.0 )
                {
                        this->headShield = 25.0;
                }
        }
        else if ( player == "heels" )
        {
                this->heelsShield += shield;
                if ( this->heelsShield > 25.0 )
                {
                        this->heelsShield = 25.0;
                }
        }
        else if ( player == "headoverheels" )
        {
                this->headShield += shield;
                if ( this->headShield > 25.0 )
                {
                        this->headShield = 25.0;
                }

                this->heelsShield += shield;
                if ( this->heelsShield > 25.0 )
                {
                        this->heelsShield = 25.0;
                }
        }
}

void GameManager::modifyShield ( const std::string& player, float shield )
{
        if ( player == "head" || player == "headoverheels" )
        {
                this->headShield = shield;
                if ( this->headShield < 0 )
                {
                        this->headShield = 0;
                }
        }

        if ( player == "heels" || player == "headoverheels" )
        {
                this->heelsShield = shield;
                if ( this->heelsShield < 0 )
                {
                        this->heelsShield = 0;
                }
        }
}

void GameManager::resetPlanets ()
{
        for ( std::map < std::string, bool >::iterator g = this->planets.begin (); g != this->planets.end (); ++g )
        {
                this->planets[ g->first ] = false ;
        }
}

void GameManager::liberatePlanet ( const std::string& planet, bool now )
{
        if ( planet == "blacktooth" || planet == "safari" || planet == "egyptus" || planet == "penitentiary" || planet == "byblos" )
        {
                this->planets[ planet ] = true;
                this->takenCrown = now;
        }
}

bool GameManager::isFreePlanet ( const std::string& planet ) const
{
        for ( std::map < std::string, bool >::const_iterator i = this->planets.begin (); i != this->planets.end (); ++i )
        {
                if ( planet == i->first )
                {
                        return i->second;
                }
        }

        return false;
}

unsigned short GameManager::countFreePlanets () const
{
        unsigned short count = 0;

        for ( std::map < std::string, bool >::const_iterator p = this->planets.begin (); p != this->planets.end (); ++p )
        {
                if ( p->second )
                {
                        count++ ;
                }
        }

        return count;
}

void GameManager::eatFish ( const PlayerItem& character, Room* room )
{
        this->eatFish( character, room, character.getX (), character.getY (), character.getZ () ) ;
}

void GameManager::eatFish ( const PlayerItem& character, Room* room, int x, int y, int z )
{
        this->eatenFish = true;

        saverAndLoader.ateFish (
                room->getNameOfFileWithDataAboutRoom (),
                character.getLabel (),
                x, y, z,
                character.getOrientation ()
        );
}

unsigned char GameManager::getLives ( const std::string& player ) const
{
        if ( player == "headoverheels" )
        {
                return std::min( this->headLives, this->heelsLives );
        }
        else if ( player == "head" )
        {
                return this->headLives;
        }
        else if ( player == "heels" )
        {
                return this->heelsLives;
        }

        return 0;
}

float GameManager::getShield ( const std::string& player ) const
{
        if ( player == "headoverheels" )
        {
                return std::max( this->headShield, this->heelsShield );
        }
        else if ( player == "head" )
        {
                return this->headShield;
        }
        else if ( player == "heels" )
        {
                return this->heelsShield;
        }

        return 0.0 ;
}

void GameManager::fillToolsOwnedByCharacter ( const std::string& player, std::vector< std::string >& tools ) const
{
        tools.clear ();

        if ( player == "head" || player == "headoverheels" )
        {
                if ( this->horn ) tools.push_back( "horn" );
        }

        if ( player == "heels" || player == "headoverheels" )
        {
                if ( this->handbag ) tools.push_back( "handbag" );
        }
}

unsigned short GameManager::getDonuts ( const std::string& player ) const
{
        return ( player == "head" || player == "headoverheels" ? this->donuts : 0 );
}

unsigned int GameManager::getVisitedRooms ()
{
        return isomot.getMapManager().countVisitedRooms() ;
}

/* static */
bool GameManager::readPreferences( const std::string& fileName )
{
        // read list of sounds from XML file
        tinyxml2::XMLDocument preferences;
        tinyxml2::XMLError result = preferences.LoadFile( fileName.c_str () );
        if ( result != tinyxml2::XML_SUCCESS )
        {
                std::cerr << "can’t read game’s preferences from \"" << fileName << "\"" << std::endl ;
                return false;
        }

        std::cout << "read game’s preferences" << std::endl ;

        tinyxml2::XMLElement* root = preferences.FirstChildElement( "preferences" );

        // chosen language

        tinyxml2::XMLElement* language = root->FirstChildElement( "language" ) ;
        std::string languageString = "en_US";
        if ( language != nilPointer )
        {
                languageString = language->FirstChild()->ToText()->Value();
        }
        gui::GuiManager::getInstance().setLanguage( languageString );

        // chosen keys

        tinyxml2::XMLElement* keyboard = root->FirstChildElement( "keyboard" ) ;
        if ( keyboard != nilPointer )
        {
                const std::vector < std::pair < /* action */ std::string, /* name */ std::string > > & chosenKeys = InputManager::getInstance().getUserKeys ();
                for ( std::vector< std::pair< std::string, std::string > >::const_iterator it = chosenKeys.begin () ; it != chosenKeys.end () ; ++ it )
                {
                        std::string xmlAction = ( it->first == "take&jump" ) ? "takeandjump" : it->first ;
                        tinyxml2::XMLElement* elementForKey = keyboard->FirstChildElement( xmlAction.c_str () ) ;

                        if ( elementForKey != nilPointer && elementForKey->FirstChild() != nilPointer )
                                InputManager::getInstance().changeUserKey( it->first, elementForKey->FirstChild()->ToText()->Value() );
                }
        }

        // preferences for audio

        tinyxml2::XMLElement* audio = root->FirstChildElement( "audio" ) ;
        if ( audio != nilPointer )
        {
                tinyxml2::XMLElement* fx = audio->FirstChildElement( "fx" ) ;
                if ( fx != nilPointer )
                {
                        SoundManager::getInstance().setVolumeOfEffects( std::atoi( fx->FirstChild()->ToText()->Value() ) ) ;
                }

                tinyxml2::XMLElement* music = audio->FirstChildElement( "music" ) ;
                if ( music != nilPointer )
                {
                        SoundManager::getInstance().setVolumeOfMusic( std::atoi( music->FirstChild()->ToText()->Value() ) ) ;
                }

                tinyxml2::XMLElement* roomtunes = audio->FirstChildElement( "roomtunes" ) ;
                if ( roomtunes != nilPointer )
                {
                        bool playRoomTunes = ( std::string( roomtunes->FirstChild()->ToText()->Value() ) == "true" ) ? true : false ;

                        if ( GameManager::getInstance().playMelodyOfScenery () != playRoomTunes )
                                GameManager::getInstance().togglePlayMelodyOfScenery ();
                }
        }

        // preferences for video

        tinyxml2::XMLElement* video = root->FirstChildElement( "video" ) ;
        if ( video != nilPointer )
        {
                tinyxml2::XMLElement* fullscreen = video->FirstChildElement( "fullscreen" ) ;
                if ( fullscreen != nilPointer )
                {
                        bool atFullScreen = ( std::string( fullscreen->FirstChild()->ToText()->Value() ) == "true" ) ? true : false ;

                        if ( gui::GuiManager::getInstance().isAtFullScreen () != atFullScreen )
                                gui::GuiManager::getInstance().toggleFullScreenVideo ();
                }

                tinyxml2::XMLElement* shadows = video->FirstChildElement( "shadows" ) ;
                if ( shadows != nilPointer )
                {
                        bool castShadows = ( std::string( shadows->FirstChild()->ToText()->Value() ) == "true" ) ? true : false ;

                        if ( GameManager::getInstance().getCastShadows () != castShadows )
                                GameManager::getInstance().toggleCastShadows ();
                }

                tinyxml2::XMLElement* background = video->FirstChildElement( "background" ) ;
                if ( background != nilPointer )
                {
                        bool drawBackground = ( std::string( background->FirstChild()->ToText()->Value() ) == "true" ) ? true : false ;

                        if ( GameManager::getInstance().hasBackgroundPicture () != drawBackground )
                                GameManager::getInstance().toggleBackgroundPicture ();
                }

                tinyxml2::XMLElement* graphics = video->FirstChildElement( "graphics" ) ;
                if ( graphics != nilPointer )
                {
                        GameManager::getInstance().setChosenGraphicSet( graphics->FirstChild()->ToText()->Value() ) ;
                }
        }

        return true;
}

/* static */
bool GameManager::writePreferences( const std::string& fileName )
{

        tinyxml2::XMLDocument preferences ;

        tinyxml2::XMLNode * root = preferences.NewElement( "preferences" );
        preferences.InsertFirstChild( root );

        // language

        tinyxml2::XMLElement * language = preferences.NewElement( "language" );
        language->SetText( gui::GuiManager::getInstance().getLanguage().c_str () );
        root->InsertEndChild( language );

        // keys
        {
                tinyxml2::XMLNode * keyboard = preferences.NewElement( "keyboard" );

                const std::vector < std::pair < /* action */ std::string, /* name */ std::string > > & chosenKeys = InputManager::getInstance().getUserKeys ();
                for ( std::vector< std::pair< std::string, std::string > >::const_iterator it = chosenKeys.begin () ; it != chosenKeys.end () ; ++ it )
                {
                        std::string xmlAction = ( it->first == "take&jump" ) ? "takeandjump" : it->first ;
                        tinyxml2::XMLElement* elementForKey = preferences.NewElement( xmlAction.c_str () ) ;
                        elementForKey->SetText( InputManager::getInstance().getUserKeyFor( it->first ).c_str () );
                        keyboard->InsertEndChild( elementForKey );
                }

                root->InsertEndChild( keyboard );
        }

        // audio
        {
                tinyxml2::XMLNode * audio = preferences.NewElement( "audio" );

                tinyxml2::XMLElement * fx = preferences.NewElement( "fx" );
                fx->SetText( SoundManager::getInstance().getVolumeOfEffects () );
                audio->InsertEndChild( fx );

                tinyxml2::XMLElement * music = preferences.NewElement( "music" );
                music->SetText( SoundManager::getInstance().getVolumeOfMusic () );
                audio->InsertEndChild( music );

                tinyxml2::XMLElement * roomtunes = preferences.NewElement( "roomtunes" );
                roomtunes->SetText( GameManager::getInstance().playMelodyOfScenery () ? "true" : "false" );
                audio->InsertEndChild( roomtunes );

                root->InsertEndChild( audio );
        }

        // video
        {
                tinyxml2::XMLNode * video = preferences.NewElement( "video" );

                tinyxml2::XMLElement * fullscreen = preferences.NewElement( "fullscreen" );
                fullscreen->SetText( gui::GuiManager::getInstance().isAtFullScreen () ? "true" : "false" );
                video->InsertEndChild( fullscreen );

                tinyxml2::XMLElement * shadows = preferences.NewElement( "shadows" );
                shadows->SetText( GameManager::getInstance().getCastShadows () ? "true" : "false" );
                video->InsertEndChild( shadows );

                tinyxml2::XMLElement * background = preferences.NewElement( "background" );
                background->SetText( GameManager::getInstance().hasBackgroundPicture () ? "true" : "false" );
                video->InsertEndChild( background );

                tinyxml2::XMLElement * graphics = preferences.NewElement( "graphics" );
                graphics->SetText( GameManager::getInstance().getChosenGraphicSet().c_str () );
                video->InsertEndChild( graphics );

                root->InsertEndChild( video );
        }

        tinyxml2::XMLError result = preferences.SaveFile( fileName.c_str () );
        if ( result != tinyxml2::XML_SUCCESS )
        {
                std::cerr << "can’t write game’s preferences to \"" << fileName << "\"" << std::endl ;
                return false;
        }

        std::cout << "wrote game’s preferences" << std::endl ;
        return true;
}

}

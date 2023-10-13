
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
#include "LanguageText.hpp"
#include "LanguageManager.hpp"
#include "Color.hpp"

#include "util.hpp"
#include "ospaths.hpp"
#include "sleep.hpp"
#include "screen.hpp"


namespace game
{

GameManager GameManager::instance ;


GameManager::GameManager( )
        : theInfo( )
        , headRoom( "blacktooth1head.xml" )
        , heelsRoom( "blacktooth23heels.xml" )
        , freedomLabel( nilPointer )
        , numberOfCapture( 0 )
        , prefixOfCaptures( util::makeRandomString( 10 ) )
        , capturePath( ospaths::homePath() + "capture" )
        , chosenGraphicsSet( "gfx" )
        , vidasInfinitas( false )
        , immunityToCollisions( false )
        , noFallingDown( false )
        , playTuneOfScenery( true )
        , castShadows( true )
        , drawSceneryBackgrounds( true )
        , drawMiniatures( false )
        , recordCaptures( false )
        , recordingTimer( new Timer () )
        , isomot( )
        , saverAndLoader( )
        , keyMoments( )
{
        if ( ! ospaths::folderExists( capturePath ) )
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
                ospaths::pathToFile( gui::GuiManager::getInstance().getPathToThesePictures(), nameOfPicture )
        ) ) ;

        PicturePtr newPicture( new Picture( *pict ) );
        newPicture->setName( nameOfPicture );

        return newPicture ;
}

void GameManager::begin ()
{
        IF_DEBUG( fprintf ( stdout, "GameManager::begin ()\n" ) )

        this->vidasInfinitas = false ;
        this->immunityToCollisions = false ;

        theInfo.resetForANewGame () ;

        this->imageOfItemInBag.clear ();
        this->planets.clear ();

        refreshAmbianceImages ();
        refreshSceneryBackgrounds ();

        isomot.beginNewGame ();

        this->update ();
}

void GameManager::update ()
{
        IF_DEBUG( fprintf ( stdout, "GameManager::update ()\n" ) )

        gui::GuiManager::getInstance().resetWhyTheGameIsPaused() ;

        while ( ! gui::GuiManager::getInstance().getWhyTheGameIsPaused().isPaused () )
        {
                if ( InputManager::getInstance().pauseTyped ()
                                || keyMoments.isGameOver( false )
                                        || keyMoments.wasFishEaten( false )
                                                || keyMoments.wasCrownTaken( false ) )
                        this->pause ();
                else
                {
                        if ( theInfo.getHeadLives () > 0 || theInfo.getHeelsLives () > 0 )
                        {
                                if ( isomot.getMapManager().getActiveRoom() != nilPointer )
                                {
                                        // update the isometric view
                                        Picture* view = isomot.updateMe ();

                                        if ( view != nilPointer )
                                        {
                                                drawAmbianceOfGame( view->getAllegroPict() );
                                                drawOnScreen( view->getAllegroPict() );
                                        }
                                        else
                                        {
                                                autouniqueptr< allegro::Pict > nothing ( allegro::Pict::newPict(
                                                                variables::getScreenWidth(), variables::getScreenHeight()
                                                ) );
                                                nothing->clearToColor( Color::byName( "gray75" ).toAllegroColor() );
                                                drawAmbianceOfGame( *nothing );
                                                drawOnScreen( *nothing );
                                        }

                                        somn::milliSleep( 1000 / Isomot::updatesPerSecond );
                                }
                                else
                                {
                                        std::cout << "there’s no active room, game over" << std::endl ;
                                        keyMoments.gameOver() ;
                                }
                        }
                        else
                        {
                                std::cout << "lives are exhausted, game over" << std::endl ;
                                keyMoments.gameOver() ;
                        }
                }
        }
}

void GameManager::pause ()
{
        // pause the isometric engine
        isomot.pause();

        Picture* view = isomot.updateMe ();
        allegro::bitBlit( view->getAllegroPict(), allegro::Pict::theScreen() );
        allegro::update ();

        allegro::emptyKeyboardBuffer();

        const allegro::Pict& previousWhere = allegro::Pict::getWhereToDraw() ;
        allegro::Pict::setWhereToDraw( view->getAllegroPict() );

        /*
         * convert from GameManager's "key moments" to GuiManager's "why paused"
         */

        gui::GuiManager::getInstance().resetWhyTheGameIsPaused () ;

        // the user just got the planet's crown
        if ( keyMoments.wasCrownTaken( true ) )
        {
                gui::GuiManager::getInstance().getWhyTheGameIsPausedToAlter().forNewlyLiberatedPlanet() ;
        }
        // the user just ate a reincarnation fish
        else if ( keyMoments.wasFishEaten( true ) )
        {
                gui::LanguageManager* language = gui::GuiManager::getInstance().getLanguageManager() ;
                gui::LanguageText* text = language->findLanguageStringForAlias( "save-game" );
                int deltaY = ( variables::getScreenHeight() >> 2 ) - 60 ;

                for ( size_t i = 0; i < text->getLinesCount(); i++ )
                {
                        gui::LanguageLine* line = text->getLine( i );
                        gui::Label label( line->text, line->font, line->color );
                        label.moveTo( ( variables::getScreenWidth() - label.getWidth() ) >> 1, deltaY );
                        deltaY += label.getHeight() * 3 / 4;
                        label.draw ();
                }

                text = language->findLanguageStringForAlias( "confirm-resume" );
                deltaY += 20;

                for ( size_t i = 0; i < text->getLinesCount(); i++ )
                {
                        gui::LanguageLine* line = text->getLine( i );
                        gui::Label label( line->text, line->font, line->color );
                        label.moveTo( ( variables::getScreenWidth() - label.getWidth() ) >> 1, deltaY );
                        deltaY += label.getHeight() * 3 / 4;
                        label.draw ();
                }

                allegro::emptyKeyboardBuffer();

                bool saveit = false;
                bool resume = false;

                // as long as the user doesn't pick one of the two options, to save or not to save
                while ( ! saveit && ! resume )
                {
                        allegro::bitBlit( view->getAllegroPict(), allegro::Pict::theScreen() );
                        allegro::update ();

                        if ( allegro::areKeypushesWaiting() )
                        {
                                iso::InputManager& inputManager = InputManager::getInstance();

                                std::string key = allegro::nextKey();

                                // save game by touching Space, or another key to don't save
                                if ( key == "Space" )
                                {
                                        saveit = true ;
                                        gui::GuiManager::getInstance().getWhyTheGameIsPausedToAlter().forSaving ();
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
                                        resume = true ;
                                        isomot.resume() ;
                                }
                        }

                        somn::milliSleep( 100 );
                }
        }
        else if ( keyMoments.arrivedInFreedomNotWithAllCrowns( true ) )
        {
                gui::GuiManager::getInstance().getWhyTheGameIsPausedToAlter().forArrivingInFreedom() ;
        }
        else if ( keyMoments.areHeadAndHeelsInFreedomWithAllTheCrowns( true ) )
        {
                gui::GuiManager::getInstance().getWhyTheGameIsPausedToAlter().forThatFinalScreen() ;
        }
        else if ( keyMoments.isGameOver( true ) )
        {
                gui::GuiManager::getInstance().getWhyTheGameIsPausedToAlter().forGameOver ();
        }
        // none of the above, thus the user just typed the pause key
        else
        {
                SoundManager::getInstance().stopEverySound ();

                gui::LanguageManager* language = gui::GuiManager::getInstance().getLanguageManager();
                gui::LanguageText* text = language->findLanguageStringForAlias( "confirm-quit" );
                int deltaY = ( variables::getScreenHeight() >> 2 );

                for ( size_t i = 0; i < text->getLinesCount(); i++ )
                {
                        gui::LanguageLine* line = text->getLine( i );
                        gui::Label label( line->text, line->font, line->color );
                        label.moveTo( ( variables::getScreenWidth() - label.getWidth() ) >> 1, deltaY );
                        deltaY += label.getHeight() * 3 / 4;
                        label.draw ();
                }

                text = language->findLanguageStringForAlias( "confirm-resume" );
                deltaY += 20;

                for ( size_t i = 0; i < text->getLinesCount(); i++ )
                {
                        gui::LanguageLine* line = text->getLine( i );
                        gui::Label label( line->text, line->font, line->color );
                        label.moveTo( ( variables::getScreenWidth() - label.getWidth() ) >> 1, deltaY );
                        deltaY += label.getHeight() * 3 / 4;
                        label.draw ();
                }

                bool quit = false ;
                bool resume = false ;

                while ( ! quit && ! resume ) // wait for the user's choice
                {
                        allegro::bitBlit( view->getAllegroPict(), allegro::Pict::theScreen() );
                        allegro::update ();

                        if ( allegro::areKeypushesWaiting() )
                        {
                                if ( allegro::nextKey() == "Escape" )
                                {
                                        quit = true;
                                        gui::GuiManager::getInstance().getWhyTheGameIsPausedToAlter().forGameOver ();
                                }
                                else
                                {
                                        resume = true;
                                        isomot.resume();
                                }
                        }

                        somn::milliSleep( 100 );
                }
        }

        allegro::Pict::setWhereToDraw( previousWhere );
}

void GameManager::resume ()
{
        IF_DEBUG( fprintf ( stdout, "GameManager::resume ()\n" ) )

        refreshAmbianceImages ();
        refreshSceneryBackgrounds ();

        // resume the isometric engine
        isomot.resume ();

        this->update ();
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

        if ( ! isSimpleGraphicsSet () )
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

        if ( ! isSimpleGraphicsSet () )
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
        ambiancePictures[ "gran velocidad" ] = refreshPicture( "quick-steps.png" );
        ambiancePictures[ "escudo" ] = refreshPicture( "shield.png" );

        ambiancePictures[ "gray grandes saltos" ] = PicturePtr( new Picture( * ambiancePictures[ "grandes saltos" ] ) );
        ambiancePictures[ "gray gran velocidad" ] = PicturePtr( new Picture( * ambiancePictures[ "gran velocidad" ] ) );
        ambiancePictures[ "gray escudo" ] = PicturePtr( new Picture( * ambiancePictures[ "escudo" ] ) );

        if ( ! isSimpleGraphicsSet () )
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
                const unsigned int diffX =  variables::getScreenWidth() - /* 640 */ constants::Default_Screen_Width ;
                const unsigned int diffY = variables::getScreenHeight() - /* 480 */ constants::Default_Screen_Height ;

                if ( diffX > 0 || diffY > 0 ) // when the screen is larger than 640 x 480
                        if ( ! isPresentGraphicsSet() && ! isSimpleGraphicsSet() )
                                drawSceneryBackgrounds = false ; // off for other sets of graphics but present & simple

                if ( drawSceneryBackgrounds )
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

                std::string character = isomot.getMapManager().getActiveRoom()->getMediator()->getLabelOfActiveCharacter();
                allegro::drawSprite (
                        ( (  character == "head" || character == "headoverheels" ) ? ambiancePictures[ "head" ] : ambiancePictures[ "gray head" ] )->getAllegroPict(),
                        161 + dx, headHeelsAmbianceY );
                allegro::drawSprite (
                        ( ( character == "heels" || character == "headoverheels" ) ? ambiancePictures[ "heels" ] : ambiancePictures[ "gray heels" ] )->getAllegroPict(),
                        431 + dx, headHeelsAmbianceY );

                allegro::drawSprite(
                        ( this->theInfo.hasHorn () ? ambiancePictures[ "horn" ] : ambiancePictures[ "gray horn" ] )->getAllegroPict(),
                        leftTooAmbianceX, headHeelsAmbianceY );

                allegro::drawSprite(
                        ( this->theInfo.hasHandbag () ? ambiancePictures[ "handbag" ] : ambiancePictures[ "gray handbag" ] )->getAllegroPict(),
                        rightTooAmbianceX, headHeelsAmbianceY );

                std::string colorOfLabels = "white";
                /* if ( isSimpleGraphicsSet () ) colorOfLabels = "magenta"; */

                // vidas de Head
                unsigned char livesForHead = this->theInfo.getHeadLives () ;
                gui::Label headLivesLabel( util::number2string( livesForHead ), "big", "white", -2 );
                headLivesLabel.moveTo( ( livesForHead > 9 ? 214 : 221 ) + dx, headHeelsAmbianceY - 1 );
                headLivesLabel.draw ();

                // vidas de Heels
                unsigned char livesForHeels = this->theInfo.getHeelsLives () ;
                gui::Label heelsLivesLabel( util::number2string( livesForHeels ), "big", "white", -2 );
                heelsLivesLabel.moveTo( ( livesForHeels > 9 ? 398 : 405 ) + dx, headHeelsAmbianceY - 1 );
                heelsLivesLabel.draw ();

                // número de rosquillas
                unsigned short doughnuts = this->theInfo.getDoughnuts () ;
                allegro::drawSprite(
                        ( doughnuts != 0 ? ambiancePictures[ "donuts" ] : ambiancePictures[ "gray donuts" ] )->getAllegroPict(),
                        leftTooAmbianceX, charStuffAmbianceY );
                if ( doughnuts > 0 )
                {
                        gui::Label donutsLabel( util::number2string( doughnuts ), "", colorOfLabels, -2 );
                        donutsLabel.moveTo( ( doughnuts > 9 ? 42 : 49 ) + dx, charStuffAmbianceY + 11 );
                        donutsLabel.draw ();
                }

                // grandes saltos
                unsigned int highJumps = this->theInfo.getBonusHighJumps () ;
                allegro::drawSprite(
                        ( highJumps > 0 ? ambiancePictures[ "grandes saltos" ] : ambiancePictures[ "gray grandes saltos" ] )->getAllegroPict(),
                        rightAmbianceX, bonusAmbianceY );
                if ( highJumps > 0 )
                {
                        gui::Label highJumpsLabel( util::number2string( highJumps ), "", colorOfLabels, -2 );
                        highJumpsLabel.moveTo( ( highJumps > 9 ? 505 : 512 ) + dx, bonusAmbianceY + 1 );
                        highJumpsLabel.draw ();
                }

                // gran velocidad
                unsigned int quickSteps = this->theInfo.getBonusQuickSteps () ;
                allegro::drawSprite(
                        ( quickSteps > 0 ? ambiancePictures[ "gran velocidad" ] : ambiancePictures[ "gray gran velocidad" ] )->getAllegroPict(),
                        leftAmbianceX, bonusAmbianceY );
                if ( quickSteps > 0 )
                {
                        gui::Label bigSpeedLabel( util::number2string( quickSteps ), "", colorOfLabels, -2 );
                        bigSpeedLabel.moveTo( ( quickSteps > 9 ? 107 : 114 ) + dx, bonusAmbianceY + 1 );
                        bigSpeedLabel.draw ();
                }

                // escudo de Head
                int headShieldInteger = this->theInfo.getHeadShieldPoints () ;
                allegro::drawSprite(
                        ( headShieldInteger > 0 ? ambiancePictures[ "escudo" ] : ambiancePictures[ "gray escudo" ] )->getAllegroPict(),
                        leftAmbianceX, immunityAmbianceY );
                if ( headShieldInteger > 0 )
                {
                        gui::Label headShieldLabel( util::number2string( headShieldInteger ), "", colorOfLabels, -2 );
                        headShieldLabel.moveTo( ( headShieldInteger > 9 ? 107 : 114 ) + dx, immunityAmbianceY + 1 );
                        headShieldLabel.draw ();
                }

                // escudo de Heels
                int heelsShieldInteger = this->theInfo.getHeelsShieldPoints () ;
                allegro::drawSprite(
                        ( heelsShieldInteger > 0 ? ambiancePictures[ "escudo" ] : ambiancePictures[ "gray escudo" ] )->getAllegroPict(),
                        rightAmbianceX, immunityAmbianceY );
                if ( heelsShieldInteger > 0 )
                {
                        gui::Label heelsShieldLabel( util::number2string( heelsShieldInteger ), "", colorOfLabels, -2 );
                        heelsShieldLabel.moveTo( ( heelsShieldInteger > 9 ? 505 : 512 ) + dx, immunityAmbianceY + 1 );
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
                        freedomLabel->moveTo( variables::getScreenWidth() / 10, variables::getScreenHeight() - 100 );
                }

                freedomLabel->draw ();
        }

        if ( ! drawSceneryDecor () )
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

                        allegro::savePictAsPCX(
                                        capturePath + ospaths::pathSeparator()
                                                + prefixOfCaptures + util::number2string( numberOfCapture )
                                        , view );

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
                if ( now ) keyMoments.crownTaken() ;
        }
}

bool GameManager::isFreePlanet ( const std::string& planet ) const
{
        for ( std::map < std::string, bool >::const_iterator i = this->planets.begin (); i != this->planets.end (); ++i )
        {
                if ( planet == i->first )
                        return i->second;
        }

        return false;
}

unsigned short GameManager::countFreePlanets () const
{
        unsigned short count = 0;

        for ( std::map < std::string, bool >::const_iterator p = this->planets.begin (); p != this->planets.end (); ++p )
        {
                if ( p->second )
                        count++ ;
        }

        return count;
}

void GameManager::eatFish ( const PlayerItem& character, Room* room )
{
        this->eatFish( character, room, character.getX (), character.getY (), character.getZ () ) ;
}

void GameManager::eatFish ( const PlayerItem& character, Room* room, int x, int y, int z )
{
        keyMoments.fishEaten ();

        saverAndLoader.ateFish (
                room->getNameOfRoomDescriptionFile (),
                character.getLabel (),
                x, y, z,
                character.getOrientation ()
        );
}

unsigned int GameManager::getVisitedRooms ()
{
        return isomot.getMapManager().countVisitedRooms() ;
}

void GameManager::inFreedomWithSoManyCrowns( unsigned int crowns )
{
        if ( crowns == 5 ) {
                // all the five crowns are taken
                keyMoments.HeadAndHeelsAreInFreedomWithAllTheCrowns ();
        } else {
                keyMoments.arriveInFreedomNotWithAllCrowns ();
        }
}

}

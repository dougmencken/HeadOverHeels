
#include "GameManager.hpp"
#include "Isomot.hpp"
#include "Room.hpp"
#include "Mediator.hpp"
#include "PlayerItem.hpp"
#include "Picture.hpp"
#include "Label.hpp"
#include "ColorCyclingLabel.hpp"
#include "GameSaverAndLoader.hpp"
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

#include <sstream>


namespace isomot
{

GameManager * GameManager::instance = nilPointer ;


GameManager::GameManager( )
        : itemDataManager( nilPointer )
        , freedomLabel( nilPointer )
        , recordingTimer( nilPointer )
        , numberOfCapture( 0 )
        , prefixOfCaptures( makeRandomString( 10 ) )
        , capturePath( isomot::homePath() + "capture" + pathSeparator )
        , chosenGraphicSet( "gfx" )
        , vidasInfinitas( false )
        , immunityToCollisions( false )
        , playTuneOfScenery( true )
        , drawShadows( true )
        , drawBackgroundPicture( true )
        , recordCaptures( false )
        , isomot( new Isomot() )
        , saverAndLoader( new GameSaverAndLoader( this, isomot ) )
        , headLives( 0 )
        , heelsLives( 0 )
        , highSpeed( 0 )
        , highJumps( 0 )
        , headShield( 0 )
        , heelsShield( 0 )
        , horn( false )
        , handbag( false )
        , donuts( 0 )
        , itemTaken( nilPointer )
        , takenCrown( false )
        , eatenFish( false )
        , gameOver( false )
        , freedom( false )
        , emperator( false )
        , frameForJail( nilPointer )
        , frameForBlacktooth( nilPointer )
        , frameForMarket( nilPointer )
        , frameForMoon( nilPointer )
        , frameForByblos( nilPointer )
        , frameForSafari( nilPointer )
        , frameForEgyptus( nilPointer )
        , frameForPenitentiary( nilPointer )
        , pictureOfHead( nilPointer )
        , grayPictureOfHead( nilPointer )
        , pictureOfHeels( nilPointer )
        , grayPictureOfHeels( nilPointer )
        , pictureOfBag( nilPointer )
        , grayPictureOfBag( nilPointer )
        , pictureOfHorn( nilPointer )
        , grayPictureOfHorn( nilPointer )
        , pictureOfDonuts( nilPointer )
        , grayPictureOfDonuts( nilPointer )
        , pictureOfGrandesSaltos( nilPointer )
        , grayPictureOfGrandesSaltos( nilPointer )
        , pictureOfGranVelocidad( nilPointer )
        , grayPictureOfGranVelocidad( nilPointer )
        , pictureOfEscudo( nilPointer )
        , grayPictureOfEscudo( nilPointer )
{
        if ( ! file_exists( capturePath.c_str (), FA_DIREC, nilPointer ) )
                mkdir( capturePath.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH );

        recordingTimer = new Timer();
        recordingTimer->go ();
}

GameManager::~GameManager( )
{
        allegro::destroyBitmap( frameForMoon );
        allegro::destroyBitmap( frameForMarket );
        allegro::destroyBitmap( frameForBlacktooth );
        allegro::destroyBitmap( frameForJail );
        allegro::destroyBitmap( frameForPenitentiary );
        allegro::destroyBitmap( frameForEgyptus );
        allegro::destroyBitmap( frameForSafari );
        allegro::destroyBitmap( frameForByblos );

        allegro::destroyBitmap( pictureOfHead );
        allegro::destroyBitmap( pictureOfHeels );
        allegro::destroyBitmap( grayPictureOfHead );
        allegro::destroyBitmap( grayPictureOfHeels );

        allegro::destroyBitmap( pictureOfBag );
        allegro::destroyBitmap( pictureOfHorn );
        allegro::destroyBitmap( grayPictureOfHorn );
        allegro::destroyBitmap( grayPictureOfBag );
        allegro::destroyBitmap( pictureOfDonuts );
        allegro::destroyBitmap( grayPictureOfDonuts );

        allegro::destroyBitmap( pictureOfGrandesSaltos );
        allegro::destroyBitmap( grayPictureOfGrandesSaltos );
        allegro::destroyBitmap( pictureOfGranVelocidad );
        allegro::destroyBitmap( grayPictureOfGranVelocidad );
        allegro::destroyBitmap( pictureOfEscudo );
        allegro::destroyBitmap( grayPictureOfEscudo );

        delete freedomLabel ;

        delete recordingTimer ;

        delete isomot ;

        delete itemDataManager;
}

/* static */
GameManager* GameManager::getInstance ()
{
        if ( instance == nilPointer )
        {
                instance = new GameManager();
        }

        return instance;
}

/* static */
BITMAP * GameManager::refreshPicture ( const char * nameOfPicture )
{
        return load_png( isomot::pathToFile( gui::GuiManager::getInstance()->getPathToPicturesOfGui() + nameOfPicture ), nilPointer );
}

WhyPause GameManager::begin ()
{
        fprintf ( stdout, "GameManager::begin ()\n" ) ;

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
        this->itemTaken = nilPointer;
        this->planets.clear();

        refreshAmbianceImages ();
        refreshBackgroundFrames ();

        assert( isomot != nilPointer );
        isomot->beginNewGame ();

        return this->update ();
}

WhyPause GameManager::resume ()
{
        fprintf ( stdout, "GameManager::resume ()\n" ) ;

        refreshAmbianceImages ();
        refreshBackgroundFrames ();

        assert( isomot != nilPointer );
        isomot->resume ();

        return this->update ();
}

void GameManager::refreshBackgroundFrames ()
{
        allegro::destroyBitmap( frameForEgyptus );
        allegro::destroyBitmap( frameForPenitentiary );
        allegro::destroyBitmap( frameForByblos );
        allegro::destroyBitmap( frameForSafari );
        allegro::destroyBitmap( frameForMoon );
        allegro::destroyBitmap( frameForMarket );
        allegro::destroyBitmap( frameForJail );
        allegro::destroyBitmap( frameForBlacktooth );

        frameForJail = refreshPicture( "jail-frame.png" );
        frameForBlacktooth = refreshPicture( "blacktooth-frame.png" );
        frameForMarket = refreshPicture( "market-frame.png" );
        frameForMoon = refreshPicture( "themoon-frame.png" );
        frameForSafari = refreshPicture( "safari-frame.png" );
        frameForByblos = refreshPicture( "byblos-frame.png" );
        frameForPenitentiary = refreshPicture( "penitentiary-frame.png" );
        frameForEgyptus = refreshPicture( "egyptus-frame.png" );
}

void GameManager::refreshAmbianceImages ()
{
        allegro::destroyBitmap( pictureOfHead );
        allegro::destroyBitmap( grayPictureOfHead );
        allegro::destroyBitmap( pictureOfHeels );
        allegro::destroyBitmap( grayPictureOfHeels );

        pictureOfHead = refreshPicture( "gui-head.png" );
        pictureOfHeels = refreshPicture( "gui-heels.png" );

        if ( ! isSimpleGraphicSet () )
        {
                grayPictureOfHead = Color::pictureToGrayscale( gui::Picture::cloneImage( pictureOfHead ) );
                grayPictureOfHeels = Color::pictureToGrayscale( gui::Picture::cloneImage( pictureOfHeels ) );
        }
        else
        {
                grayPictureOfHead = Color::colorizePicture( gui::Picture::cloneImage( pictureOfHead ), Color::greenColor() );
                grayPictureOfHeels = Color::colorizePicture( gui::Picture::cloneImage( pictureOfHeels ), Color::greenColor() );
        }

        allegro::destroyBitmap( pictureOfBag );
        allegro::destroyBitmap( grayPictureOfBag );
        allegro::destroyBitmap( pictureOfDonuts );
        allegro::destroyBitmap( grayPictureOfDonuts );
        allegro::destroyBitmap( pictureOfHorn );
        allegro::destroyBitmap( grayPictureOfHorn );

        pictureOfBag = refreshPicture( "gui-handbag.png" );
        pictureOfHorn = refreshPicture( "gui-horn.png" );
        pictureOfDonuts = refreshPicture( "gui-donuts.png" );

        if ( ! isSimpleGraphicSet () )
        {
                grayPictureOfHorn = Color::pictureToGrayscale( gui::Picture::cloneImage( pictureOfHorn ) );
                grayPictureOfBag = Color::pictureToGrayscale( gui::Picture::cloneImage( pictureOfBag ) );
                grayPictureOfDonuts = Color::pictureToGrayscale( gui::Picture::cloneImage( pictureOfDonuts ) );
        }
        else
        {
                grayPictureOfHorn = Color::colorizePicture( gui::Picture::cloneImage( pictureOfHorn ), Color::greenColor() );
                grayPictureOfBag = Color::colorizePicture( gui::Picture::cloneImage( pictureOfBag ), Color::greenColor() );
                grayPictureOfDonuts = Color::colorizePicture( gui::Picture::cloneImage( pictureOfDonuts ), Color::greenColor() );

                Color::colorizePicture( pictureOfBag, Color::yellowColor() );
                Color::colorizePicture( pictureOfHorn, Color::yellowColor() );
                Color::colorizePicture( pictureOfDonuts, Color::yellowColor() );
        }

        allegro::destroyBitmap( grayPictureOfGrandesSaltos );
        allegro::destroyBitmap( pictureOfGrandesSaltos );
        allegro::destroyBitmap( grayPictureOfGranVelocidad );
        allegro::destroyBitmap( pictureOfGranVelocidad );
        allegro::destroyBitmap( grayPictureOfEscudo );
        allegro::destroyBitmap( pictureOfEscudo );

        pictureOfGrandesSaltos = refreshPicture( "high-jumps.png" );
        pictureOfGranVelocidad = refreshPicture( "high-speed.png" );
        pictureOfEscudo = refreshPicture( "shield.png" );

        if ( ! isSimpleGraphicSet () )
        {
                grayPictureOfGrandesSaltos = Color::pictureToGrayscale( gui::Picture::cloneImage( pictureOfGrandesSaltos ) );
                grayPictureOfGranVelocidad = Color::pictureToGrayscale( gui::Picture::cloneImage( pictureOfGranVelocidad ) );
                grayPictureOfEscudo = Color::pictureToGrayscale( gui::Picture::cloneImage( pictureOfEscudo ) );
        }
        else
        {
                grayPictureOfGrandesSaltos = Color::colorizePicture( gui::Picture::cloneImage( pictureOfGrandesSaltos ), Color::greenColor() );
                Color::colorizePicture( pictureOfGrandesSaltos, Color::yellowColor() );

                grayPictureOfGranVelocidad = Color::colorizePicture( gui::Picture::cloneImage( pictureOfGranVelocidad ), Color::greenColor() );
                Color::colorizePicture( pictureOfGranVelocidad, Color::yellowColor() );

                grayPictureOfEscudo = Color::colorizePicture( gui::Picture::cloneImage( pictureOfEscudo ), Color::greenColor() );
                Color::colorizePicture( pictureOfEscudo, Color::yellowColor() );
        }
}

void GameManager::drawAmbianceOfGame ( BITMAP * where )
{
        // scenery of this room
        std::string scenery = isomot->getMapManager()->getActiveRoom()->getScenery();

        // empty scenery means that it is the final room
        if ( scenery != "" )
        {
                if ( drawBackgroundPicture )
                {
                        // marco, varía en función del escenario
                        BITMAP * background = nilPointer;

                        if ( scenery == "blacktooth" )
                                background = frameForBlacktooth;
                        else if ( scenery == "jail" )
                                background = frameForJail;
                        else if ( scenery == "market" )
                                background = frameForMarket;
                        else if ( scenery == "themoon" )
                                background = frameForMoon;
                        else if ( scenery == "safari" )
                                background = frameForSafari;
                        else if ( scenery == "byblos" )
                                background = frameForByblos;
                        else if ( scenery == "penitentiary" )
                                background = frameForPenitentiary;
                        else if ( scenery == "egyptus" )
                                background = frameForEgyptus;

                        if ( background != nilPointer )
                                allegro::drawSprite( where, background, 0, 0 );
                }

                std::string player = isomot->getMapManager()->getActiveRoom()->getMediator()->getLabelOfActiveCharacter();
                allegro::drawSprite( where, ( (  player == "head" || player == "headoverheels" ) ? pictureOfHead : grayPictureOfHead ), 161, 425 );
                allegro::drawSprite( where, ( ( player == "heels" || player == "headoverheels" ) ? pictureOfHeels : grayPictureOfHeels ), 431, 425 );

                allegro::drawSprite( where, ( this->horn ? pictureOfHorn : grayPictureOfHorn ), 33, 425 );

                allegro::drawSprite( where, ( this->handbag ? pictureOfBag : grayPictureOfBag ), 559, 425 );

                std::string colorOfLabels = "white";
                /* if ( isSimpleGraphicSet () ) colorOfLabels = "magenta"; */

                // vidas de Head
                gui::Label headLivesLabel( numberToString( this->headLives ), "big", "white", -2 );
                headLivesLabel.moveTo( this->headLives > 9 ? 214 : 221, 424 );
                headLivesLabel.draw( where );

                // vidas de Heels
                gui::Label heelsLivesLabel( numberToString( this->heelsLives ), "big", "white", -2 );
                heelsLivesLabel.moveTo( this->heelsLives > 9 ? 398 : 405, 424 );
                heelsLivesLabel.draw( where );

                // número de rosquillas
                allegro::drawSprite( where, ( this->donuts != 0 ? pictureOfDonuts : grayPictureOfDonuts ), 33, 361 );
                if ( this->donuts > 0 )
                {
                        gui::Label donutsLabel( numberToString( this->donuts ), "regular", colorOfLabels, -2 );
                        donutsLabel.moveTo( this->donuts > 9 ? 42 : 49, 372 );
                        donutsLabel.draw( where );
                }

                // grandes saltos
                allegro::drawSprite( where, ( this->highJumps > 0 ? pictureOfGrandesSaltos : grayPictureOfGrandesSaltos ), 505, 392 );
                if ( this->highJumps > 0 )
                {
                        gui::Label highJumpsLabel( numberToString( this->highJumps ), "regular", colorOfLabels, -2 );
                        highJumpsLabel.moveTo( this->highJumps > 9 ? 505 : 512, 393 );
                        highJumpsLabel.draw( where );
                }

                // gran velocidad
                allegro::drawSprite( where, ( this->highSpeed > 0 ? pictureOfGranVelocidad : grayPictureOfGranVelocidad ), 107, 392 );
                if ( this->highSpeed > 0 )
                {
                        gui::Label highSpeedLabel( numberToString( this->highSpeed ), "regular", colorOfLabels, -2 );
                        highSpeedLabel.moveTo( this->highSpeed > 9 ? 107 : 114, 393 );
                        highSpeedLabel.draw( where );
                }

                // escudo de Head
                allegro::drawSprite( where, ( this->headShield > 0 ? pictureOfEscudo : grayPictureOfEscudo ), 107, 436 );
                if ( this->headShield > 0 )
                {
                        int headShieldValue = static_cast< int >( this->headShield * 99.0 / 25.0 );
                        gui::Label headShieldLabel( numberToString( headShieldValue ), "regular", colorOfLabels, -2 );
                        headShieldLabel.moveTo( headShieldValue > 9 ? 107 : 114, 437 );
                        headShieldLabel.draw( where );
                }

                // escudo de Heels
                allegro::drawSprite( where, ( this->headShield > 0 ? pictureOfEscudo : grayPictureOfEscudo ), 505, 436 );
                if ( this->heelsShield > 0 )
                {
                        int heelsShieldValue = static_cast< int >( this->heelsShield * 99.0 / 25.0 );
                        gui::Label heelsShieldLabel( numberToString( heelsShieldValue ), "regular", colorOfLabels, -2 );
                        heelsShieldLabel.moveTo( heelsShieldValue > 9 ? 505 : 512, 437 );
                        heelsShieldLabel.draw( where );
                }

                // item in handbag
                if ( this->itemTaken != nilPointer )
                {
                        allegro::drawSprite( where, this->itemTaken, 559, 361 );
                }
        }
        else
        {
                // draw color~cycling FREEDOM on the final scene
                if ( freedomLabel == nilPointer )
                {
                        freedomLabel = new gui::ColorCyclingLabel( "FREEDOM", "big" );
                        freedomLabel->moveTo( ScreenWidth / 10, ScreenHeight - 100 );
                }

                freedomLabel->draw( where );
        }

        if ( recordCaptures )
        {
                // record game’s screen
                if ( recordingTimer->getValue() > 0.1 /* 10 times per one second */ )
                {
                        numberOfCapture++ ;
                        std::ostringstream file;
                        file << prefixOfCaptures << numberOfCapture << ".pcx";

                        save_bitmap( ( capturePath + file.str() ).c_str (), where, nilPointer );

                        recordingTimer->reset ();
                }
        }

        acquire_screen();

        blit( where, screen, 0, 0, 0, 0, where->w, where->h );

        if ( recordCaptures )
        {
                if ( ! drawBackgroundPicture ) rectfill( screen, 11, 18, 19, 30, Color::blackColor()->toAllegroColor() );
                rectfill( screen, 19, 18, 80, 30, Color::gray50Color()->toAllegroColor() );
                circlefill( screen, 34, 24, 5, Color::redColor()->toAllegroColor() );
                textout_ex( screen, font, "REC", 48, 21, Color::redColor()->toAllegroColor(), -1 );
        }

        release_screen();
}

void GameManager::loadGame ( const std::string& fileName )
{
        isomot->prepare() ;
        saverAndLoader->loadGame( fileName );
}

void GameManager::saveGame ( const std::string& fileName )
{
        saverAndLoader->saveGame( fileName );
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

void GameManager::addShield ( const std::string& player, double shield )
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

void GameManager::modifyShield ( const std::string& player, double shield )
{
        if ( player == "head" )
        {
                this->headShield = shield;
                if ( this->headShield < 0 )
                {
                        this->headShield = 0;
                }
        }
        else if ( player == "heels" )
        {
                this->heelsShield = shield;
                if ( this->heelsShield < 0 )
                {
                        this->heelsShield = 0;
                }
        }
        else if ( player == "headoverheels" )
        {
                this->headShield = shield;
                if ( this->headShield < 0 )
                {
                        this->headShield = 0;
                }

                this->heelsShield = shield;
                if ( this->heelsShield < 0 )
                {
                        this->heelsShield = 0;
                }
        }
}

void GameManager::emptyHandbag ( const std::string& player )
{
        if ( player == "heels" || player == "headoverheels" )
        {
                this->itemTaken = nilPointer;
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

void GameManager::eatFish ( PlayerItem* character, Room* room )
{
        this->eatFish( character, room, character->getX (), character->getY (), character->getZ () ) ;
}

void GameManager::eatFish ( PlayerItem* character, Room* room, int x, int y, int z )
{
        this->eatenFish = true;

        saverAndLoader->ateFish (
                room->getNameOfFileWithDataAboutRoom (),
                character->getLabel (),
                x, y, z,
                character->getOrientation ()
        );
}

WhyPause GameManager::update ()
{
        WhyPause why = Nevermind;

        // periodically update the game while user does not type pause key confirming it with ESCAPE
        while ( why == Nevermind )
        {
                if ( ! key[ InputManager::getInstance()->getUserKey( "pause" ) ] && ! this->takenCrown && ! this->eatenFish && ! this->gameOver )
                {
                        // actualiza la vista isométrica
                        BITMAP* view = isomot->update();

                        // se ha podido actualizar
                        if ( view != nilPointer )
                        {
                                this->drawAmbianceOfGame( view );
                                milliSleep( 10 );
                        }
                        else
                        {
                                // la partida ha terminado
                                this->gameOver = true;
                        }
                }
                else
                {
                        // detiene el juego
                        why = this->pause ();
                }
        }

        return why;
}

WhyPause GameManager::pause ()
{
        bool exit = false;
        bool confirm = false;
        WhyPause why = Nevermind;

        // detiene el motor isométrico
        isomot->pause();

        // da tiempo a que el usuario vea la parada
        clear_keybuf();

        // el usuario acaba de liberar un planeta, se muestra la pantalla de los planetas
        if ( this->takenCrown )
        {
                why = FreePlanet;
                this->takenCrown = false;
        }
        // el usuario acaba de comer un pez, se le da la opción de grabar la partida
        else if ( this->eatenFish )
        {
                this->eatenFish = false;
                isomot::InputManager* inputManager = InputManager::getInstance();

                gui::LanguageManager* language = gui::GuiManager::getInstance()->getLanguageManager();
                gui::LanguageText* text = language->findLanguageStringForAlias( "save-game" );
                int deltaY = 100;

                for ( size_t i = 0; i < text->getLinesCount(); i++ )
                {
                        gui::LanguageLine* line = text->getLine( i );
                        gui::Label label( line->text, line->font, line->color );
                        label.moveTo( ( ScreenWidth - label.getWidth() ) >> 1, deltaY );
                        deltaY += label.getHeight() * 3 / 4;
                        acquire_screen();
                        label.draw( screen );
                        release_screen();
                }

                text = language->findLanguageStringForAlias( "confirm-resume" );
                deltaY += 20;

                for ( size_t i = 0; i < text->getLinesCount(); i++ )
                {
                        gui::LanguageLine* line = text->getLine( i );
                        gui::Label label( line->text, line->font, line->color );
                        label.moveTo( ( ScreenWidth - label.getWidth() ) >> 1, deltaY );
                        deltaY += label.getHeight() * 3 / 4;
                        acquire_screen();
                        label.draw( screen );
                        release_screen();
                }

                clear_keybuf();

                // mientras el usuario no elija una de las dos opciones no se hará nada
                while ( ! confirm && ! exit )
                {
                        if ( keypressed() )
                        {
                                int key = readkey() >> 8;

                                // si se pulsa SPACE se mostrará la interfaz para la grabación de la partida
                                if ( key == KEY_SPACE )
                                {
                                        exit = true;
                                        why = SaveGame;
                                }
                                else if ( key != inputManager->getUserKey( "movenorth" ) &&
                                          key != inputManager->getUserKey( "movesouth" ) &&
                                          key != inputManager->getUserKey( "moveeast" ) &&
                                          key != inputManager->getUserKey( "movewest" ) &&
                                          key != inputManager->getUserKey( "jump" ) &&
                                          key != inputManager->getUserKey( "take" ) &&
                                          key != inputManager->getUserKey( "take&jump" ) &&
                                          key != inputManager->getUserKey( "swap" ) &&
                                          key != inputManager->getUserKey( "doughnut" ) &&
                                          key != inputManager->getUserKey( "pause" ) &&
                                          key != KEY_ESC )
                                {
                                        confirm = true;
                                        isomot->resume();
                                }
                        }

                        milliSleep( 10 );
                }
        }
        // el usuario ha acabado con las vidas de todos los jugadores
        else if ( this->gameOver )
        {
                this->gameOver = false;
                why = GameOver;
                isomot->reset();
        }
        // el usuario ha llegado a Freedom pero le faltaron coronas
        else if ( this->freedom )
        {
                this->freedom = false;
                why = Freedom;
                isomot->reset();
        }
        // el usuario ha acabado el juego
        else if ( this->emperator )
        {
                this->emperator = false;
                why = Sucess;
                isomot->reset();
        }
        // el usuario ha pulsado la tecla de pausa
        else
        {
                SoundManager::getInstance()->stopEverySound ();

                gui::LanguageManager* language = gui::GuiManager::getInstance()->getLanguageManager();
                gui::LanguageText* text = language->findLanguageStringForAlias( "confirm-quit" );
                int deltaY = ScreenHeight / 4;

                for ( size_t i = 0; i < text->getLinesCount(); i++ )
                {
                        gui::LanguageLine* line = text->getLine( i );
                        gui::Label label( line->text, line->font, line->color );
                        label.moveTo( ( ScreenWidth - label.getWidth() ) >> 1, deltaY );
                        deltaY += label.getHeight() * 3 / 4;
                        acquire_screen();
                        label.draw( screen );
                        release_screen();
                }

                text = language->findLanguageStringForAlias( "confirm-resume" );
                deltaY += 20;

                for ( size_t i = 0; i < text->getLinesCount(); i++ )
                {
                        gui::LanguageLine* line = text->getLine( i );
                        gui::Label label( line->text, line->font, line->color );
                        label.moveTo( ( ScreenWidth - label.getWidth() ) >> 1, deltaY );
                        deltaY += label.getHeight() * 3 / 4;
                        acquire_screen();
                        label.draw( screen );
                        release_screen();
                }

                // mientras el usuario no elija una de las dos opciones no se hará nada
                while ( ! confirm && ! exit )
                {
                        if ( keypressed() )
                        {
                                // Si ha pulsado Escape se suspende la partida en curso
                                if ( readkey() >> 8 == KEY_ESC )
                                {
                                        exit = true;
                                        why = GameOver;
                                        isomot->reset();
                                }
                                // Si se pulsa cualquier otra tecla, prosigue la partida en curso
                                else
                                {
                                        confirm = true;
                                        isomot->resume();
                                }
                        }

                        milliSleep( 10 );
                }
        }

        return why;
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

double GameManager::getShield ( const std::string& player ) const
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

        return 0.0;
}

void GameManager::toolsOwnedByCharacter ( const std::string& player, std::vector< std::string >& tools ) const
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

unsigned int GameManager::getVisitedRooms () const
{
        return isomot->getMapManager()->countVisitedRooms() ;
}

void GameManager::binItemDataManager ()
{
        if ( this->itemDataManager != nilPointer )
        {
                delete this->itemDataManager;
                this->itemDataManager = nilPointer ;
        }
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
        gui::GuiManager::getInstance()->setLanguage( languageString );

        // chosen keys

        tinyxml2::XMLElement* keyboard = root->FirstChildElement( "keyboard" ) ;
        if ( keyboard != nilPointer )
        {
                tinyxml2::XMLElement* movenorth = keyboard->FirstChildElement( "movenorth" ) ;
                if ( movenorth != nilPointer )
                        InputManager::getInstance()->changeUserKey( "movenorth", std::atoi( movenorth->FirstChild()->ToText()->Value() ) );

                tinyxml2::XMLElement* movesouth = keyboard->FirstChildElement( "movesouth" ) ;
                if ( movesouth != nilPointer )
                        InputManager::getInstance()->changeUserKey( "movesouth", std::atoi( movesouth->FirstChild()->ToText()->Value() ) );

                tinyxml2::XMLElement* moveeast = keyboard->FirstChildElement( "moveeast" ) ;
                if ( moveeast != nilPointer )
                        InputManager::getInstance()->changeUserKey( "moveeast", std::atoi( moveeast->FirstChild()->ToText()->Value() ) );

                tinyxml2::XMLElement* movewest = keyboard->FirstChildElement( "movewest" ) ;
                if ( movewest != nilPointer )
                        InputManager::getInstance()->changeUserKey( "movewest", std::atoi( movewest->FirstChild()->ToText()->Value() ) );

                tinyxml2::XMLElement* take = keyboard->FirstChildElement( "take" ) ;
                if ( take != nilPointer )
                        InputManager::getInstance()->changeUserKey( "take", std::atoi( take->FirstChild()->ToText()->Value() ) );

                tinyxml2::XMLElement* jump = keyboard->FirstChildElement( "jump" ) ;
                if ( jump != nilPointer )
                        InputManager::getInstance()->changeUserKey( "jump", std::atoi( jump->FirstChild()->ToText()->Value() ) );

                tinyxml2::XMLElement* doughnut = keyboard->FirstChildElement( "doughnut" ) ;
                if ( doughnut != nilPointer )
                        InputManager::getInstance()->changeUserKey( "doughnut", std::atoi( doughnut->FirstChild()->ToText()->Value() ) );

                tinyxml2::XMLElement* takeandjump = keyboard->FirstChildElement( "takeandjump" ) ;
                if ( takeandjump != nilPointer )
                        InputManager::getInstance()->changeUserKey( "take&jump", std::atoi( takeandjump->FirstChild()->ToText()->Value() ) );

                tinyxml2::XMLElement* swap = keyboard->FirstChildElement( "swap" ) ;
                if ( swap != nilPointer )
                        InputManager::getInstance()->changeUserKey( "swap", std::atoi( swap->FirstChild()->ToText()->Value() ) );

                tinyxml2::XMLElement* pause = keyboard->FirstChildElement( "pause" ) ;
                if ( pause != nilPointer )
                        InputManager::getInstance()->changeUserKey( "pause", std::atoi( pause->FirstChild()->ToText()->Value() ) );
        }

        // preferences for audio

        tinyxml2::XMLElement* audio = root->FirstChildElement( "audio" ) ;
        if ( audio != nilPointer )
        {
                tinyxml2::XMLElement* fx = audio->FirstChildElement( "fx" ) ;
                if ( fx != nilPointer )
                {
                        SoundManager::getInstance()->setVolumeOfEffects( std::atoi( fx->FirstChild()->ToText()->Value() ) ) ;
                }

                tinyxml2::XMLElement* music = audio->FirstChildElement( "music" ) ;
                if ( music != nilPointer )
                {
                        SoundManager::getInstance()->setVolumeOfMusic( std::atoi( music->FirstChild()->ToText()->Value() ) ) ;
                }

                tinyxml2::XMLElement* roomtunes = audio->FirstChildElement( "roomtunes" ) ;
                if ( roomtunes != nilPointer )
                {
                        bool playRoomTunes = ( std::string( roomtunes->FirstChild()->ToText()->Value() ) == "true" ) ? true : false ;

                        if ( GameManager::getInstance()->playMelodyOfScenery () != playRoomTunes )
                                GameManager::getInstance()->togglePlayMelodyOfScenery ();
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

                        if ( gui::GuiManager::getInstance()->isAtFullScreen () != atFullScreen )
                                gui::GuiManager::getInstance()->toggleFullScreenVideo ();
                }

                tinyxml2::XMLElement* shadows = video->FirstChildElement( "shadows" ) ;
                if ( shadows != nilPointer )
                {
                        bool drawShadows = ( std::string( shadows->FirstChild()->ToText()->Value() ) == "true" ) ? true : false ;

                        if ( GameManager::getInstance()->getDrawShadows () != drawShadows )
                                GameManager::getInstance()->toggleDrawShadows ();
                }

                tinyxml2::XMLElement* background = video->FirstChildElement( "background" ) ;
                if ( background != nilPointer )
                {
                        bool drawBackground = ( std::string( background->FirstChild()->ToText()->Value() ) == "true" ) ? true : false ;

                        if ( GameManager::getInstance()->hasBackgroundPicture () != drawBackground )
                                GameManager::getInstance()->toggleBackgroundPicture ();
                }

                tinyxml2::XMLElement* graphics = video->FirstChildElement( "graphics" ) ;
                if ( graphics != nilPointer )
                {
                        GameManager::getInstance()->setChosenGraphicSet( graphics->FirstChild()->ToText()->Value() ) ;
                }
        }

        return true;
}

/* static */
bool GameManager::writePreferences( const std::string& fileName )
{

        tinyxml2::XMLDocument preferences;

        tinyxml2::XMLNode * root = preferences.NewElement( "preferences" );
        preferences.InsertFirstChild( root );

        // language

        tinyxml2::XMLElement * language = preferences.NewElement( "language" );
        language->SetText( gui::GuiManager::getInstance()->getLanguage().c_str () );
        root->InsertEndChild( language );

        // keys
        {
                tinyxml2::XMLNode * keyboard = preferences.NewElement( "keyboard" );

                tinyxml2::XMLElement * movenorth = preferences.NewElement( "movenorth" );
                movenorth->SetText( InputManager::getInstance()->getUserKey( "movenorth" ) );
                keyboard->InsertEndChild( movenorth );

                tinyxml2::XMLElement * movesouth = preferences.NewElement( "movesouth" );
                movesouth->SetText( InputManager::getInstance()->getUserKey( "movesouth" ) );
                keyboard->InsertEndChild( movesouth );

                tinyxml2::XMLElement * moveeast = preferences.NewElement( "moveeast" );
                moveeast->SetText( InputManager::getInstance()->getUserKey( "moveeast" ) );
                keyboard->InsertEndChild( moveeast );

                tinyxml2::XMLElement * movewest = preferences.NewElement( "movewest" );
                movewest->SetText( InputManager::getInstance()->getUserKey( "movewest" ) );
                keyboard->InsertEndChild( movewest );

                tinyxml2::XMLElement * take = preferences.NewElement( "take" );
                take->SetText( InputManager::getInstance()->getUserKey( "take" ) );
                keyboard->InsertEndChild( take );

                tinyxml2::XMLElement * jump = preferences.NewElement( "jump" );
                jump->SetText( InputManager::getInstance()->getUserKey( "jump" ) );
                keyboard->InsertEndChild( jump );

                tinyxml2::XMLElement * doughnut = preferences.NewElement( "doughnut" );
                doughnut->SetText( InputManager::getInstance()->getUserKey( "doughnut" ) );
                keyboard->InsertEndChild( doughnut );

                tinyxml2::XMLElement * takeandjump = preferences.NewElement( "takeandjump" );
                takeandjump->SetText( InputManager::getInstance()->getUserKey( "take&jump" ) );
                keyboard->InsertEndChild( takeandjump );

                tinyxml2::XMLElement * swap = preferences.NewElement( "swap" );
                swap->SetText( InputManager::getInstance()->getUserKey( "swap" ) );
                keyboard->InsertEndChild( swap );

                tinyxml2::XMLElement * pause = preferences.NewElement( "pause" );
                pause->SetText( InputManager::getInstance()->getUserKey( "pause" ) );
                keyboard->InsertEndChild( pause );

                root->InsertEndChild( keyboard );
        }

        // audio
        {
                tinyxml2::XMLNode * audio = preferences.NewElement( "audio" );

                tinyxml2::XMLElement * fx = preferences.NewElement( "fx" );
                fx->SetText( SoundManager::getInstance()->getVolumeOfEffects () );
                audio->InsertEndChild( fx );

                tinyxml2::XMLElement * music = preferences.NewElement( "music" );
                music->SetText( SoundManager::getInstance()->getVolumeOfMusic () );
                audio->InsertEndChild( music );

                tinyxml2::XMLElement * roomtunes = preferences.NewElement( "roomtunes" );
                roomtunes->SetText( GameManager::getInstance()->playMelodyOfScenery () ? "true" : "false" );
                audio->InsertEndChild( roomtunes );

                root->InsertEndChild( audio );
        }

        // video
        {
                tinyxml2::XMLNode * video = preferences.NewElement( "video" );

                tinyxml2::XMLElement * fullscreen = preferences.NewElement( "fullscreen" );
                fullscreen->SetText( gui::GuiManager::getInstance()->isAtFullScreen () ? "true" : "false" );
                video->InsertEndChild( fullscreen );

                tinyxml2::XMLElement * shadows = preferences.NewElement( "shadows" );
                shadows->SetText( GameManager::getInstance()->getDrawShadows () ? "true" : "false" );
                video->InsertEndChild( shadows );

                tinyxml2::XMLElement * background = preferences.NewElement( "background" );
                background->SetText( GameManager::getInstance()->hasBackgroundPicture () ? "true" : "false" );
                video->InsertEndChild( background );

                tinyxml2::XMLElement * graphics = preferences.NewElement( "graphics" );
                graphics->SetText( GameManager::getInstance()->getChosenGraphicSet() );
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

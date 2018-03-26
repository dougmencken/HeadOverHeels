
#include "GameManager.hpp"
#include "Isomot.hpp"
#include "Room.hpp"
#include "Mediator.hpp"
#include "PlayerItem.hpp"
#include "Picture.hpp"
#include "Label.hpp"
#include "GameFileManager.hpp"
#include "GuiManager.hpp"
#include "MapManager.hpp"
#include "InputManager.hpp"
#include "BonusManager.hpp"
#include "SoundManager.hpp"
#include "InputManager.hpp"
#include "LanguageText.hpp"
#include "LanguageManager.hpp"
#include "Color.hpp"


namespace isomot
{

GameManager * GameManager::instance = nilPointer ;


GameManager::GameManager( )
        : chosenGraphicSet( "gfx" )
        , vidasInfinitas( false )
        , immunityToCollisions( false )
        , playTuneOfScenery( true )
        , drawShadows( true )
        , drawBackgroundPicture( true )
        , isomot( new Isomot() )
        , gameFileManager( new GameFileManager( this, isomot ) )
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

        delete isomot ;
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

        assert( this->isomot != nilPointer );
        this->isomot->beginNewGame ();

        return this->update ();
}

WhyPause GameManager::resume ()
{
        fprintf ( stdout, "GameManager::resume ()\n" ) ;

        refreshAmbianceImages ();
        refreshBackgroundFrames ();

        assert( this->isomot != nilPointer );
        this->isomot->resume ();

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
        std::string scenery = this->isomot->getMapManager()->getActiveRoom()->getScenery();

        // empty scenery means it is the final screen
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

                std::string player = this->isomot->getMapManager()->getActiveRoom()->getMediator()->getLabelOfActiveCharacter();
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
                // draw FREEDOM in the final screen
                gui::Label freedomLabel( "FREEDOM", "big", "multicolor" );
                freedomLabel.moveTo( ScreenWidth / 10, ScreenHeight - 100 );
                freedomLabel.draw( where );
        }

        acquire_screen();
        blit( where, screen, 0, 0, 0, 0, where->w, where->h );
        release_screen();
}

void GameManager::loadGame ( const std::string& fileName )
{
        isomot->prepare() ;
        gameFileManager->loadGame( fileName );
}

void GameManager::saveGame ( const std::string& fileName )
{
        gameFileManager->saveGame( fileName );
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

        gameFileManager->assignFishData(
                room->getNameOfFileWithDataAboutRoom (),
                        character->getLabel (),
                        x, y, z,
                        character->getOrientation () ) ;
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
                        BITMAP* view = this->isomot->update();

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
        this->isomot->pause();

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
                gui::LanguageText* text = language->findLanguageString( "save-game" );
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

                text = language->findLanguageString( "confirm-resume" );
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
                                        this->isomot->resume();
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
                this->isomot->reset();
        }
        // el usuario ha llegado a Freedom pero le faltaron coronas
        else if ( this->freedom )
        {
                this->freedom = false;
                why = Freedom;
                this->isomot->reset();
        }
        // el usuario ha acabado el juego
        else if ( this->emperator )
        {
                this->emperator = false;
                why = Sucess;
                this->isomot->reset();
        }
        // el usuario ha pulsado la tecla de pausa
        else
        {
                SoundManager::getInstance()->stopEverySound ();

                gui::LanguageManager* language = gui::GuiManager::getInstance()->getLanguageManager();
                gui::LanguageText* text = language->findLanguageString( "confirm-quit" );
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

                text = language->findLanguageString( "confirm-resume" );
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
                                        this->isomot->reset();
                                }
                                // Si se pulsa cualquier otra tecla, prosigue la partida en curso
                                else
                                {
                                        confirm = true;
                                        this->isomot->resume();
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

std::vector< std::string > GameManager::getToolsOwnedByPlayer ( const std::string& player ) const
{
        std::vector< std::string > tools;

        if ( player == "head" )
        {
                if ( this->horn )
                {
                        tools.push_back( "horn" );
                }
        }
        else if ( player == "heels" )
        {
                if ( this->handbag )
                {
                        tools.push_back( "handbag" );
                }
        }
        else if ( player == "headoverheels" )
        {
                if ( this->horn )
                {
                        tools.push_back( "horn" );
                }
                if ( this->handbag )
                {
                        tools.push_back( "handbag" );
                }
        }

        return tools;
}

unsigned short GameManager::getDonuts ( const std::string& player ) const
{
        return ( player == "head" || player == "headoverheels" ? this->donuts : 0 );
}

unsigned int GameManager::getVisitedRooms () const
{
        return this->isomot->getMapManager()->countVisitedRooms() ;
}

}

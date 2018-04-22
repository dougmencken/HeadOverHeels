
#include "CreatePlanetsScreen.hpp"
#include "GameManager.hpp"
#include "LanguageManager.hpp"
#include "LanguageText.hpp"
#include "SoundManager.hpp"
#include "GuiManager.hpp"
#include "GameManager.hpp"
#include "Color.hpp"
#include "Screen.hpp"
#include "Picture.hpp"
#include "Label.hpp"
#include "CreateMainMenu.hpp"
#include "ContinueGame.hpp"

using gui::CreatePlanetsScreen ;
using gui::ContinueGame ;
using gui::GuiManager ;
using isomot::GameManager ;
using isomot::SoundManager ;


CreatePlanetsScreen::CreatePlanetsScreen( BITMAP* where, bool notNewGame )
        : Action( where )
        , gameInProgress( notNewGame )
        , blacktoothFree( false )
        , egyptusFree( false )
        , penitentiaryFree( false )
        , byblosFree( false )
        , safariFree( false )
{

}

CreatePlanetsScreen::~CreatePlanetsScreen( )
{

}

void CreatePlanetsScreen::doAction ()
{
        SoundManager::getInstance()->playOgg( "music/HeadOverHeels.ogg", /* loop */ false );

        LanguageManager* languageManager = GuiManager::getInstance()->getLanguageManager();

        Screen* planets = GuiManager::getInstance()->findOrCreateScreenForAction( this );

        if ( planets->countWidgets() > 0 )
        {
                planets->freeWidgets() ;
        }

        const unsigned int screenWidth = isomot::ScreenWidth();
        const unsigned int screenHeight = isomot::ScreenHeight();

        // etiqueta fija “ El Imperio Blacktooth ”
        std::string colorOfLabel = "yellow";
        if ( GameManager::getInstance()->isSimpleGraphicSet () ) colorOfLabel = "green";
        Label* empire = new Label( languageManager->findLanguageStringForAlias( "blacktooth-empire" )->getText(), "big", colorOfLabel );
        empire->moveTo( ( screenWidth - empire->getWidth() ) >> 1, 2 );
        empire->setAction( new ContinueGame( getWhereToDraw(), gameInProgress ) );

        planets->addWidget( empire );
        planets->setKeyHandler( empire );

        Picture* imageOfChapeau = 0;

        BITMAP* chapeau = Screen::loadPicture( "crown.png" );
        BITMAP* chapeauTriste = 0;
        if ( ! GameManager::getInstance()->isSimpleGraphicSet () )
        {
                chapeauTriste = Color::pictureToGrayscale( Picture::cloneImage( chapeau ) );
        }
        else
        {
                chapeauTriste = Color::colorizePicture( Picture::cloneImage( chapeau ), Color::gray50Color() );
                Color::colorizePicture( chapeau, Color::yellowColor() );
        }

        const int halfOfChapeauWidth = chapeau->w >> 1;
        const int chapeauOffsetY = - ( chapeau->h + ( screenHeight >> 7 ) ) ;
        const int labelOffsetY = 80;
        const int topOffsetY = 60;

        // Blacktooth

        BITMAP* planetBlacktooth = Screen::loadPicture( "blacktooth.png" );
        if ( planetBlacktooth != nilPointer )
        {
                const int blacktoothX = ( screenWidth - planetBlacktooth->w ) >> 1 ;
                const int blacktoothY = ( ( screenHeight - planetBlacktooth->h ) >> 1 ) + ( topOffsetY >> 1 ) ;

                Picture* imageOfBlacktooth = new Picture( blacktoothX, blacktoothY, planetBlacktooth, "planet Blacktooth" );
                planets->addWidget( imageOfBlacktooth );

                imageOfChapeau = new Picture(
                        blacktoothX + ( imageOfBlacktooth->getWidth() >> 1 ) - halfOfChapeauWidth,
                        blacktoothY + chapeauOffsetY,
                        Picture::cloneImage( blacktoothFree ? chapeau : chapeauTriste ),
                        "image of chapeau for blacktooth"
                );
                planets->addWidget( imageOfChapeau );

                Label* nameOfPlanet = new Label( languageManager->findLanguageStringForAlias( "blacktooth" )->getText() );
                nameOfPlanet->moveTo( blacktoothX + ( imageOfBlacktooth->getWidth() >> 1 ) - ( nameOfPlanet->getWidth() >> 1 ), blacktoothY + labelOffsetY );
                planets->addWidget( nameOfPlanet );
        }

        // Egyptus

        BITMAP* planetEgyptus = Screen::loadPicture( "egyptus.png" );
        if ( planetEgyptus != nilPointer )
        {
                const int egyptusX = ( screenWidth >> 2 ) - ( screenWidth >> 4 ) - ( planetEgyptus->w >> 1 ) ;
                const int egyptusY = ( screenHeight >> 2 ) - ( planetEgyptus->h >> 1 ) + ( topOffsetY >> 1 ) ;

                Picture* imageOfEgyptus = new Picture( egyptusX, egyptusY, planetEgyptus, "planet Egyptus" );
                planets->addWidget( imageOfEgyptus );

                imageOfChapeau = new Picture(
                        egyptusX + ( imageOfEgyptus->getWidth() >> 1 ) - halfOfChapeauWidth,
                        egyptusY + chapeauOffsetY,
                        Picture::cloneImage( egyptusFree ? chapeau : chapeauTriste ),
                        "image of chapeau for egyptus"
                );
                planets->addWidget( imageOfChapeau );

                Label* nameOfPlanet = new Label( languageManager->findLanguageStringForAlias( "egyptus" )->getText() );
                nameOfPlanet->moveTo( egyptusX + ( imageOfEgyptus->getWidth() >> 1 ) - ( nameOfPlanet->getWidth() >> 1 ), egyptusY + labelOffsetY );
                planets->addWidget( nameOfPlanet );
        }

        // Penitentiary

        BITMAP* planetPenitentiary = Screen::loadPicture( "penitentiary.png" );
        if ( planetPenitentiary != nilPointer )
        {
                const int penitentiaryX = ( screenWidth >> 1 ) + ( screenWidth >> 2 ) + ( screenWidth >> 4 ) - ( planetPenitentiary->w >> 1 );
                const int penitentiaryY = ( screenHeight >> 2 ) - ( planetPenitentiary->h >> 1 ) + ( topOffsetY >> 1 ) ;

                Picture* imageOfPenitentiary = new Picture( penitentiaryX, penitentiaryY, planetPenitentiary, "planet Penitentiary" );
                planets->addWidget( imageOfPenitentiary );

                imageOfChapeau = new Picture(
                        penitentiaryX + ( imageOfPenitentiary->getWidth() >> 1 ) - halfOfChapeauWidth,
                        penitentiaryY + chapeauOffsetY,
                        Picture::cloneImage( penitentiaryFree ? chapeau : chapeauTriste ),
                        "image of chapeau for penitentiary"
                );
                planets->addWidget( imageOfChapeau );

                Label* nameOfPlanet = new Label( languageManager->findLanguageStringForAlias( "penitentiary" )->getText() );
                nameOfPlanet->moveTo( penitentiaryX + ( imageOfPenitentiary->getWidth() >> 1 ) - ( nameOfPlanet->getWidth() >> 1 ), penitentiaryY + labelOffsetY );
                planets->addWidget( nameOfPlanet );
        }

        // Byblos

        BITMAP* planetByblos = Screen::loadPicture( "byblos.png" );
        if ( planetByblos != nilPointer )
        {
                const int byblosX = ( screenWidth >> 2 ) - ( screenWidth >> 4 ) - ( planetByblos->w >> 1 ) ;
                const int byblosY = ( screenHeight >> 1 ) + ( screenHeight >> 2 ) - ( planetByblos->h >> 1 ) + ( topOffsetY >> 1 ) ;

                Picture* imageOfByblos = new Picture( byblosX, byblosY, planetByblos, "planet Bookworld" );
                planets->addWidget( imageOfByblos );

                imageOfChapeau = new Picture(
                        byblosX + ( imageOfByblos->getWidth() >> 1 ) - halfOfChapeauWidth,
                        byblosY + chapeauOffsetY,
                        Picture::cloneImage( byblosFree ? chapeau : chapeauTriste ),
                        "image of chapeau for byblos"
                );
                planets->addWidget( imageOfChapeau );

                Label* nameOfPlanet = new Label( languageManager->findLanguageStringForAlias( "byblos" )->getText() );
                nameOfPlanet->moveTo( byblosX + ( imageOfByblos->getWidth() >> 1 ) - ( nameOfPlanet->getWidth() >> 1 ), byblosY + labelOffsetY );
                planets->addWidget( nameOfPlanet );
        }

        // Safari

        BITMAP* planetSafari = Screen::loadPicture( "safari.png" );
        if ( planetSafari != nilPointer )
        {
                const int safariX = ( screenWidth >> 1 ) + ( screenWidth >> 2 ) + ( screenWidth >> 4 ) - ( planetSafari->w >> 1 );
                const int safariY = ( screenHeight >> 1 ) + ( screenHeight >> 2 ) - ( planetSafari->h >> 1 ) + ( topOffsetY >> 1 );

                Picture* imageOfSafari = new Picture( safariX, safariY, planetSafari, "planet Safari" );
                planets->addWidget( imageOfSafari );

                imageOfChapeau = new Picture(
                        safariX + ( imageOfSafari->getWidth() >> 1 ) - halfOfChapeauWidth,
                        safariY + chapeauOffsetY,
                        Picture::cloneImage( safariFree ? chapeau : chapeauTriste ),
                        "image of chapeau for safari"
                );
                planets->addWidget( imageOfChapeau );

                Label* nameOfPlanet = new Label( languageManager->findLanguageStringForAlias( "safari" )->getText() );
                nameOfPlanet->moveTo( safariX + ( imageOfSafari->getWidth() >> 1 ) - ( nameOfPlanet->getWidth() >> 1 ), safariY + labelOffsetY );
                planets->addWidget( nameOfPlanet );
        }

        allegro::destroyBitmap( chapeau );
        allegro::destroyBitmap( chapeauTriste );

        GuiManager::getInstance()->changeScreen( planets, true );
}

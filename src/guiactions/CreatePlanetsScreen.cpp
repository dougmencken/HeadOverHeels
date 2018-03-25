
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

        Label* label = 0;

        // etiqueta fija “ El Imperio Blacktooth ”
        std::string colorOfLabel = "yellow";
        if ( GameManager::getInstance()->isSimpleGraphicSet () ) colorOfLabel = "green";
        label = new Label( languageManager->findLanguageString( "blacktooth-empire" )->getText(), "big", colorOfLabel );
        label->moveTo( ( isomot::ScreenWidth - label->getWidth() ) >> 1, 0 );
        label->setAction( new ContinueGame( getWhereToDraw(), gameInProgress ) );

        planets->addWidget( label );
        planets->setKeyHandler( label );

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
        const int chapeauOffsetY = - ( chapeau->h + 2 );
        const int labelOffsetY = 80;

        // Egyptus
        const int egyptusX = 77; const int egyptusY = 120;
        Picture* imageOfEgyptus = new Picture( egyptusX, egyptusY, Screen::loadPicture( "egyptus.png" ), "planet Egyptus" );
        planets->addWidget( imageOfEgyptus );

        imageOfChapeau = new Picture(
                egyptusX + ( imageOfEgyptus->getWidth() >> 1 ) - halfOfChapeauWidth,
                egyptusY + chapeauOffsetY,
                Picture::cloneImage( egyptusFree ? chapeau : chapeauTriste ),
                "image of chapeau for egyptus"
        );
        planets->addWidget( imageOfChapeau );

        label = new Label( languageManager->findLanguageString( "egyptus" )->getText() );
        label->moveTo( egyptusX + ( imageOfEgyptus->getWidth() >> 1 ) - ( label->getWidth() >> 1 ), egyptusY + labelOffsetY );
        planets->addWidget( label );

        // Penitentiary
        const int penitentiaryX = 463; const int penitentiaryY = egyptusY;
        Picture* imageOfPenitentiary = new Picture( penitentiaryX, penitentiaryY, Screen::loadPicture( "penitentiary.png" ), "planet Penitentiary" );
        planets->addWidget( imageOfPenitentiary );

        imageOfChapeau = new Picture(
                penitentiaryX + ( imageOfPenitentiary->getWidth() >> 1 ) - halfOfChapeauWidth,
                penitentiaryY + chapeauOffsetY,
                Picture::cloneImage( penitentiaryFree ? chapeau : chapeauTriste ),
                "image of chapeau for penitentiary"
        );
        planets->addWidget( imageOfChapeau );

        label = new Label( languageManager->findLanguageString( "penitentiary" )->getText() );
        label->moveTo( penitentiaryX + ( imageOfPenitentiary->getWidth() >> 1 ) - ( label->getWidth() >> 1 ), penitentiaryY + labelOffsetY );
        planets->addWidget( label );

        // Byblos
        const int byblosX = egyptusX; const int byblosY = 370;
        Picture* imageOfByblos = new Picture( byblosX, byblosY, Screen::loadPicture( "byblos.png" ), "planet Bookworld" );
        planets->addWidget( imageOfByblos );

        imageOfChapeau = new Picture(
                byblosX + ( imageOfByblos->getWidth() >> 1 ) - halfOfChapeauWidth,
                byblosY + chapeauOffsetY,
                Picture::cloneImage( byblosFree ? chapeau : chapeauTriste ),
                "image of chapeau for byblos"
        );
        planets->addWidget( imageOfChapeau );

        label = new Label( languageManager->findLanguageString( "byblos" )->getText() );
        label->moveTo( byblosX + ( imageOfByblos->getWidth() >> 1 ) - ( label->getWidth() >> 1 ), byblosY + labelOffsetY );
        planets->addWidget( label );

        // Safari
        const int safariX = penitentiaryX; const int safariY = byblosY;
        Picture* imageOfSafari = new Picture( safariX, safariY, Screen::loadPicture( "safari.png" ), "planet Safari" );
        planets->addWidget( imageOfSafari );

        imageOfChapeau = new Picture(
                safariX + ( imageOfSafari->getWidth() >> 1 ) - halfOfChapeauWidth,
                safariY + chapeauOffsetY,
                Picture::cloneImage( safariFree ? chapeau : chapeauTriste ),
                "image of chapeau for safari"
        );
        planets->addWidget( imageOfChapeau );

        label = new Label( languageManager->findLanguageString( "safari" )->getText() );
        label->moveTo( safariX + ( imageOfSafari->getWidth() >> 1 ) - ( label->getWidth() >> 1 ), safariY + labelOffsetY );
        planets->addWidget( label );

        // Blacktooth
        const int blacktoothX = 283; const int blacktoothY = 250;
        Picture* imageOfBlacktooth = new Picture( blacktoothX, blacktoothY, Screen::loadPicture( "blacktooth.png" ), "planet Blacktooth" );
        planets->addWidget( imageOfBlacktooth );

        imageOfChapeau = new Picture(
                blacktoothX + ( imageOfBlacktooth->getWidth() >> 1 ) - halfOfChapeauWidth,
                blacktoothY + chapeauOffsetY,
                Picture::cloneImage( blacktoothFree ? chapeau : chapeauTriste ),
                "image of chapeau for blacktooth"
        );
        planets->addWidget( imageOfChapeau );

        label = new Label( languageManager->findLanguageString( "blacktooth" )->getText() );
        label->moveTo( blacktoothX + ( imageOfBlacktooth->getWidth() >> 1 ) - ( label->getWidth() >> 1 ), blacktoothY + labelOffsetY );
        planets->addWidget( label );

        allegro::destroyBitmap( chapeau );
        allegro::destroyBitmap( chapeauTriste );

        GuiManager::getInstance()->changeScreen( planets, true );
}

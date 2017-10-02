
#include "CreatePlanetsScreen.hpp"
#include "GameManager.hpp"
#include "LanguageManager.hpp"
#include "LanguageText.hpp"
#include "SoundManager.hpp"
#include "GuiManager.hpp"
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


CreatePlanetsScreen::CreatePlanetsScreen( BITMAP* where, bool gameInProgress ) : Action( )
        , where( where )
        , gameInProgress( gameInProgress )
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

void CreatePlanetsScreen::doIt ()
{
        SoundManager::getInstance()->playOgg( "music/HeadOverHeels.ogg", /* loop */ false );

        LanguageManager* languageManager = GuiManager::getInstance()->getLanguageManager();

        Screen* planets = GuiManager::getInstance()->findOrCreateScreenForAction( this, this->where );

        if ( planets->countWidgets() > 0 )
        {
                planets->freeWidgets() ;
        }

        Label* label = 0;

        // etiqueta fija “ El Imperio Blacktooth ”
        label = new Label( languageManager->findLanguageString( "blacktooth-empire" )->getText(), "big", "yellow" );
        label->moveTo( ( 640 - label->getWidth() ) >> 1, 0 );
        label->setAction( new ContinueGame( this->where, gameInProgress ) );

        planets->addWidget( label );
        planets->setKeyHandler( label );

        BITMAP* planetBlacktooth = Screen::loadPicture( "blacktooth.png" );
        BITMAP* planetSafari = Screen::loadPicture( "safari.png" );
        BITMAP* planetBookworld = Screen::loadPicture( "byblos.png" );
        BITMAP* planetEgyptus = Screen::loadPicture( "egyptus.png" );
        BITMAP* planetPenitentiary = Screen::loadPicture( "penitentiary.png" );

        Picture* imageOfChapeau = 0;
        const int halfOfChapeauWidth = 48 >> 1;
        const int chapeauOffsetY = -50;
        const int labelOffsetY = 80;

        // Egyptus
        const int egyptusX = 77; const int egyptusY = 120;
        Picture* imageOfEgyptus = new Picture( egyptusX, egyptusY, planetEgyptus, "egyptus.png" );
        planets->addWidget( imageOfEgyptus );

        imageOfChapeau = new Picture(
                egyptusX + ( imageOfEgyptus->getWidth() >> 1 ) - halfOfChapeauWidth,
                egyptusY + chapeauOffsetY,
                egyptusFree ? Screen::loadPicture( "crown.png" ) : Screen::loadPicture( "grey-crown.png" ),
                "image of chapeau for egyptus"
        );
        planets->addWidget( imageOfChapeau );

        label = new Label( languageManager->findLanguageString( "egyptus" )->getText() );
        label->moveTo( egyptusX + ( imageOfEgyptus->getWidth() >> 1 ) - ( label->getWidth() >> 1 ), egyptusY + labelOffsetY );
        planets->addWidget( label );

        // Penitentiary
        const int penitentiaryX = 463; const int penitentiaryY = egyptusY;
        Picture* imageOfPenitentiary = new Picture( penitentiaryX, penitentiaryY, planetPenitentiary, "penitentiary.png" );
        planets->addWidget( imageOfPenitentiary );

        imageOfChapeau = new Picture(
                penitentiaryX + ( imageOfPenitentiary->getWidth() >> 1 ) - halfOfChapeauWidth,
                penitentiaryY + chapeauOffsetY,
                penitentiaryFree ? Screen::loadPicture( "crown.png" ) : Screen::loadPicture( "grey-crown.png" ),
                "image of chapeau for penitentiary"
        );
        planets->addWidget( imageOfChapeau );

        label = new Label( languageManager->findLanguageString( "penitentiary" )->getText() );
        label->moveTo( penitentiaryX + ( imageOfPenitentiary->getWidth() >> 1 ) - ( label->getWidth() >> 1 ), penitentiaryY + labelOffsetY );
        planets->addWidget( label );

        // Byblos
        const int byblosX = egyptusX; const int byblosY = 370;
        Picture* imageOfByblos = new Picture( byblosX, byblosY, planetBookworld, "byblos.png" );
        planets->addWidget( imageOfByblos );

        imageOfChapeau = new Picture(
                byblosX + ( imageOfByblos->getWidth() >> 1 ) - halfOfChapeauWidth,
                byblosY + chapeauOffsetY,
                byblosFree ? Screen::loadPicture( "crown.png" ) : Screen::loadPicture( "grey-crown.png" ),
                "image of chapeau for byblos"
        );
        planets->addWidget( imageOfChapeau );

        label = new Label( languageManager->findLanguageString( "byblos" )->getText() );
        label->moveTo( byblosX + ( imageOfByblos->getWidth() >> 1 ) - ( label->getWidth() >> 1 ), byblosY + labelOffsetY );
        planets->addWidget( label );

        // Safari
        const int safariX = penitentiaryX; const int safariY = byblosY;
        Picture* imageOfSafari = new Picture( safariX, safariY, planetSafari, "safari.png" );
        planets->addWidget( imageOfSafari );

        imageOfChapeau = new Picture(
                safariX + ( imageOfSafari->getWidth() >> 1 ) - halfOfChapeauWidth,
                safariY + chapeauOffsetY,
                safariFree ? Screen::loadPicture( "crown.png" ) : Screen::loadPicture( "grey-crown.png" ),
                "image of chapeau for safari"
        );
        planets->addWidget( imageOfChapeau );

        label = new Label( languageManager->findLanguageString( "safari" )->getText() );
        label->moveTo( safariX + ( imageOfSafari->getWidth() >> 1 ) - ( label->getWidth() >> 1 ), safariY + labelOffsetY );
        planets->addWidget( label );

        // Blacktooth
        const int blacktoothX = 283; const int blacktoothY = 250;
        Picture* imageOfBlacktooth = new Picture( blacktoothX, blacktoothY, planetBlacktooth, "blacktooth.png" );
        planets->addWidget( imageOfBlacktooth );

        imageOfChapeau = new Picture(
                blacktoothX + ( imageOfBlacktooth->getWidth() >> 1 ) - halfOfChapeauWidth,
                blacktoothY + chapeauOffsetY,
                blacktoothFree ? Screen::loadPicture( "crown.png" ) : Screen::loadPicture( "grey-crown.png" ),
                "image of chapeau for blacktooth"
        );
        planets->addWidget( imageOfChapeau );

        label = new Label( languageManager->findLanguageString( "blacktooth" )->getText() );
        label->moveTo( blacktoothX + ( imageOfBlacktooth->getWidth() >> 1 ) - ( label->getWidth() >> 1 ), blacktoothY + labelOffsetY );
        planets->addWidget( label );

        GuiManager::getInstance()->changeScreen( planets );
}

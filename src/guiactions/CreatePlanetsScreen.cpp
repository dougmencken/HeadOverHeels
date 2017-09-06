
#include "CreatePlanetsScreen.hpp"
#include "GuiManager.hpp"
#include "GameManager.hpp"
#include "LanguageManager.hpp"
#include "LanguageText.hpp"
#include "SoundManager.hpp"
#include "Screen.hpp"
#include "Icon.hpp"
#include "Label.hpp"
#include "CreateMainMenu.hpp"
#include "ContinueGame.hpp"

using gui::CreatePlanetsScreen ;
using gui::ContinueGame ;
using isomot::GameManager ;
using isomot::SoundManager ;


CreatePlanetsScreen::CreatePlanetsScreen( BITMAP* picture, bool gameInProgress ) : Action( )
        , where( picture )
        , gameInProgress( gameInProgress )
        , blacktoothFree( false )
        , egyptusFree( false )
        , penitentiaryFree( false )
        , byblosFree( false )
        , safariFree( false )
        , chapeau( 0 )
        , chapeauTriste( 0 )
        , planetBlacktooth( 0 )
        , planetSafari( 0 )
        , planetBookworld( 0 )
        , planetEgyptus( 0 )
        , planetPenitentiary( 0 )
{

}

CreatePlanetsScreen::~CreatePlanetsScreen( )
{
        delete planetEgyptus ;
        delete planetPenitentiary ;
        delete planetSafari ;
        delete planetBookworld ;
        delete planetBlacktooth ;

        delete chapeau ;
        delete chapeauTriste ;
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

        this->planetBlacktooth = GameManager::refreshPicture( planetBlacktooth, "blacktooth.png" );
        this->planetSafari = GameManager::refreshPicture( planetSafari, "safari.png" );
        this->planetBookworld = GameManager::refreshPicture( planetBookworld, "byblos.png" );
        this->planetEgyptus = GameManager::refreshPicture( planetEgyptus, "egyptus.png" );
        this->planetPenitentiary = GameManager::refreshPicture( planetPenitentiary, "penitentiary.png" );

        this->chapeau = GameManager::refreshPicture( this->chapeau, "crown.png" );
        this->chapeauTriste = GameManager::refreshPicture( this->chapeauTriste, "grey-crown.png" );

        Icon* imageOfChapeau = 0;
        const int halfOfChapeauWidth = this->chapeau->w >> 1;
        const int chapeauOffsetY = -50;
        const int labelOffsetY = 80;

        // Egyptus
        const int egyptusX = 77; const int egyptusY = 120;
        Icon* imageOfEgyptus = new Icon( egyptusX, egyptusY, planetEgyptus );
        planets->addWidget( imageOfEgyptus );

        imageOfChapeau = new Icon(
                egyptusX + ( imageOfEgyptus->getWidth() >> 1 ) - halfOfChapeauWidth,
                egyptusY + chapeauOffsetY,
                egyptusFree ? chapeau : chapeauTriste
        );
        planets->addWidget( imageOfChapeau );

        label = new Label( languageManager->findLanguageString( "egyptus" )->getText() );
        label->moveTo( egyptusX + ( imageOfEgyptus->getWidth() >> 1 ) - ( label->getWidth() >> 1 ), egyptusY + labelOffsetY );
        planets->addWidget( label );

        // Penitentiary
        const int penitentiaryX = 463; const int penitentiaryY = egyptusY;
        Icon* imageOfPenitentiary = new Icon( penitentiaryX, penitentiaryY, planetPenitentiary );
        planets->addWidget( imageOfPenitentiary );

        imageOfChapeau = new Icon(
                penitentiaryX + ( imageOfPenitentiary->getWidth() >> 1 ) - halfOfChapeauWidth,
                penitentiaryY + chapeauOffsetY,
                penitentiaryFree ? chapeau : chapeauTriste
        );
        planets->addWidget( imageOfChapeau );

        label = new Label( languageManager->findLanguageString( "penitentiary" )->getText() );
        label->moveTo( penitentiaryX + ( imageOfPenitentiary->getWidth() >> 1 ) - ( label->getWidth() >> 1 ), penitentiaryY + labelOffsetY );
        planets->addWidget( label );

        // Byblos
        const int byblosX = egyptusX; const int byblosY = 370;
        Icon* imageOfByblos = new Icon( byblosX, byblosY, planetBookworld );
        planets->addWidget( imageOfByblos );

        imageOfChapeau = new Icon(
                byblosX + ( imageOfByblos->getWidth() >> 1 ) - halfOfChapeauWidth,
                byblosY + chapeauOffsetY,
                byblosFree ? chapeau : chapeauTriste
        );
        planets->addWidget( imageOfChapeau );

        label = new Label( languageManager->findLanguageString( "byblos" )->getText() );
        label->moveTo( byblosX + ( imageOfByblos->getWidth() >> 1 ) - ( label->getWidth() >> 1 ), byblosY + labelOffsetY );
        planets->addWidget( label );

        // Safari
        const int safariX = penitentiaryX; const int safariY = byblosY;
        Icon* imageOfSafari = new Icon( safariX, safariY, planetSafari );
        planets->addWidget( imageOfSafari );

        imageOfChapeau = new Icon(
                safariX + ( imageOfSafari->getWidth() >> 1 ) - halfOfChapeauWidth,
                safariY + chapeauOffsetY,
                safariFree ? chapeau : chapeauTriste
        );
        planets->addWidget( imageOfChapeau );

        label = new Label( languageManager->findLanguageString( "safari" )->getText() );
        label->moveTo( safariX + ( imageOfSafari->getWidth() >> 1 ) - ( label->getWidth() >> 1 ), safariY + labelOffsetY );
        planets->addWidget( label );

        // Blacktooth
        const int blacktoothX = 283; const int blacktoothY = 250;
        Icon* imageOfBlacktooth = new Icon( blacktoothX, blacktoothY, planetBlacktooth );
        planets->addWidget( imageOfBlacktooth );

        imageOfChapeau = new Icon(
                blacktoothX + ( imageOfBlacktooth->getWidth() >> 1 ) - halfOfChapeauWidth,
                blacktoothY + chapeauOffsetY,
                blacktoothFree ? chapeau : chapeauTriste
        );
        planets->addWidget( imageOfChapeau );

        label = new Label( languageManager->findLanguageString( "blacktooth" )->getText() );
        label->moveTo( blacktoothX + ( imageOfBlacktooth->getWidth() >> 1 ) - ( label->getWidth() >> 1 ), blacktoothY + labelOffsetY );
        planets->addWidget( label );

        GuiManager::getInstance()->changeScreen( planets );
}

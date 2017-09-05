
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

        Label* label = 0;
        LanguageText* langString = 0;
        LanguageManager* languageManager = GuiManager::getInstance()->getLanguageManager();

        Screen* planets = GuiManager::getInstance()->findOrCreateScreenForAction( this, this->where );

        if ( planets->countWidgets() > 0 )
        {
                planets->freeWidgets() ;
        }

        // Etiqueta fija: El Imperio Blacktooth
        langString = languageManager->findLanguageString( "blacktooth-empire" );
        label = new Label( 0, 0, langString->getText() );
        label->changeFontAndColor( "big", "yellow" );
        label->changePosition( ( 640 - label->getWidth() ) >> 1, langString->getY() );
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

        // Egyptus
        planets->addWidget( new Icon( 77, 121, planetEgyptus ) );
        planets->addWidget( new Icon( 90, 70, egyptusFree ? chapeau : chapeauTriste ) );
        langString = languageManager->findLanguageString( "egyptus" );
        planets->addWidget( new Label( langString->getX(), langString->getY(), langString->getText() ) );

        // Penitentiary
        planets->addWidget( new Icon( 463, 121, planetPenitentiary ) );
        planets->addWidget( new Icon( 476, 70, penitentiaryFree ? chapeau : chapeauTriste ) );
        langString = languageManager->findLanguageString( "penitentiary" );
        planets->addWidget( new Label( langString->getX(), langString->getY(), langString->getText() ) );

        // Byblos
        planets->addWidget( new Icon( 77, 371, planetBookworld ) );
        planets->addWidget( new Icon( 90, 320, byblosFree ? chapeau : chapeauTriste ) );
        langString = languageManager->findLanguageString( "byblos" );
        planets->addWidget( new Label( langString->getX(), langString->getY(), langString->getText() ) );

        // Safari
        planets->addWidget( new Icon( 463, 371, planetSafari ) );
        planets->addWidget( new Icon( 476, 320, safariFree ? chapeau : chapeauTriste ) );
        langString = languageManager->findLanguageString( "safari" );
        planets->addWidget( new Label( langString->getX(), langString->getY(), langString->getText() ) );

        // Blacktooth
        planets->addWidget( new Icon( 283, 251, planetBlacktooth ) );
        planets->addWidget( new Icon( 296, 200, blacktoothFree ? chapeau : chapeauTriste ) );
        langString = languageManager->findLanguageString( "blacktooth" );
        planets->addWidget( new Label( langString->getX(), langString->getY(), langString->getText() ) );

        GuiManager::getInstance()->changeScreen( planets );
}

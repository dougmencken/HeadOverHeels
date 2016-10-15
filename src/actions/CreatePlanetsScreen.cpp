
#include "CreatePlanetsScreen.hpp"
#include "GuiManager.hpp"
#include "LanguageManager.hpp"
#include "SoundManager.hpp"
#include "Screen.hpp"
#include "Icon.hpp"
#include "Label.hpp"
#include "CreateMainMenu.hpp"
#include "BeginGame.hpp"

using gui::CreatePlanetsScreen;
using gui::BeginGame;
using isomot::SoundManager;


CreatePlanetsScreen::CreatePlanetsScreen( BITMAP* picture, bool gameInProgress )
: Action(),
  where( picture ),
  gameInProgress( gameInProgress ),
  blacktooth( false ),
  egyptus( false ),
  penitentiary( false ),
  byblos( false ),
  safari( false )
{

}

void CreatePlanetsScreen::doIt ()
{
        SoundManager::getInstance()->stopOgg();
        SoundManager::getInstance()->playOgg( "music/HeadOverHeels.ogg", /* loop */ true );

        Label* label = 0;
        LanguageText* langString = 0;
        Screen* planets = new Screen( 0, 0, this->where );
        LanguageManager* languageManager = GuiManager::getInstance()->getLanguageManager();

        // Imagen de fondo
        planets->setBackground( GuiManager::getInstance()->findImage( "background" ) );

        // Etiqueta fija: El Imperio Blacktooth
        langString = languageManager->findLanguageString( "blacktooth-empire" );
        label = new Label( 0, 0, langString->getText() );
        label->changeFont( "big", "yellow" );
        label->changePosition( ( 640 - label->getWidth() ) >> 1, langString->getY() );
        label->setAction( new BeginGame( this->where, gameInProgress ) );
        planets->addWidget( label );

        // Crea la cadena de responsabilidad
        planets->setSucessor( label );

        // Egyptus
        planets->addWidget( new Icon( 77, 121, GuiManager::getInstance()->findImage( "egyptus" ) ) );
        planets->addWidget( new Icon( 90, 70, GuiManager::getInstance()->findImage( this->egyptus ? "crown" : "grey-crown" ) ) );
        langString = languageManager->findLanguageString( "egyptus" );
        planets->addWidget( new Label( langString->getX(), langString->getY(), langString->getText() ) );

        // Penitentiary
        planets->addWidget( new Icon( 463, 121, GuiManager::getInstance()->findImage( "penitentiary" ) ) );
        planets->addWidget( new Icon( 476, 70, GuiManager::getInstance()->findImage( this->penitentiary ? "crown" : "grey-crown" ) ) );
        langString = languageManager->findLanguageString( "penitentiary" );
        planets->addWidget( new Label( langString->getX(), langString->getY(), langString->getText() ) );

        // Byblos
        planets->addWidget( new Icon( 77, 371, GuiManager::getInstance()->findImage( "byblos" ) ) );
        planets->addWidget( new Icon( 90, 320, GuiManager::getInstance()->findImage( this->byblos ? "crown" : "grey-crown" ) ) );
        langString = languageManager->findLanguageString( "byblos" );
        planets->addWidget( new Label( langString->getX(), langString->getY(), langString->getText() ) );

        // Safari
        planets->addWidget( new Icon( 463, 371, GuiManager::getInstance()->findImage( "safari" ) ) );
        planets->addWidget( new Icon( 476, 320, GuiManager::getInstance()->findImage( this->safari ? "crown" : "grey-crown" ) ) );
        langString = languageManager->findLanguageString( "safari" );
        planets->addWidget( new Label( langString->getX(), langString->getY(), langString->getText() ) );

        // Blacktooth
        planets->addWidget( new Icon( 283, 251, GuiManager::getInstance()->findImage( "blacktooth" ) ) );
        planets->addWidget( new Icon( 296, 200, GuiManager::getInstance()->findImage( this->blacktooth ? "crown" : "grey-crown" ) ) );
        langString = languageManager->findLanguageString( "blacktooth" );
        planets->addWidget( new Label( langString->getX(), langString->getY(), langString->getText() ) );

        // Cambia la pantalla mostrada en la interfaz
        GuiManager::getInstance()->changeScreen( planets );
}

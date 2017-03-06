
#include "CreatePlanetsScreen.hpp"
#include "GuiManager.hpp"
#include "LanguageManager.hpp"
#include "SoundManager.hpp"
#include "Screen.hpp"
#include "Icon.hpp"
#include "Label.hpp"
#include "CreateMainMenu.hpp"
#include "ContinueGame.hpp"

using gui::CreatePlanetsScreen;
using gui::ContinueGame;
using isomot::SoundManager;


CreatePlanetsScreen::CreatePlanetsScreen( BITMAP* picture, bool gameInProgress )
: Action(),
  where( picture ),
  gameInProgress( gameInProgress ),
  blacktoothFree( false ),
  egyptusFree( false ),
  penitentiaryFree( false ),
  byblosFree( false ),
  safariFree( false )
{

}

void CreatePlanetsScreen::doIt ()
{
        ///SoundManager::getInstance()->stopAnyOgg();
        SoundManager::getInstance()->playOgg( "music/HeadOverHeels.ogg", /* loop */ false );

        Label* label = 0;
        LanguageText* langString = 0;
        Screen* planets = new Screen( 0, 0, this->where );
        LanguageManager* languageManager = GuiManager::getInstance()->getLanguageManager();

        // Imagen de fondo
        planets->setBackground( GuiManager::getInstance()->findImage( "background" ) );

        // Etiqueta fija: El Imperio Blacktooth
        langString = languageManager->findLanguageString( "blacktooth-empire" );
        label = new Label( 0, 0, langString->getText() );
        label->changeFontAndColor( "big", "yellow" );
        label->changePosition( ( 640 - label->getWidth() ) >> 1, langString->getY() );
        label->setAction( new ContinueGame( this->where, gameInProgress ) );

        planets->addWidget( label );
        planets->setNext( label );

        BITMAP* chapeau = GuiManager::getInstance()->findImage( "crown" ) ;
        BITMAP* chapeauTriste = GuiManager::getInstance()->findImage( "grey-crown" ) ;

        // Egyptus
        planets->addWidget( new Icon( 77, 121, GuiManager::getInstance()->findImage( "egyptus" ) ) );
        planets->addWidget( new Icon( 90, 70, egyptusFree ? chapeau : chapeauTriste ) );
        langString = languageManager->findLanguageString( "egyptus" );
        planets->addWidget( new Label( langString->getX(), langString->getY(), langString->getText() ) );

        // Penitentiary
        planets->addWidget( new Icon( 463, 121, GuiManager::getInstance()->findImage( "penitentiary" ) ) );
        planets->addWidget( new Icon( 476, 70, penitentiaryFree ? chapeau : chapeauTriste ) );
        langString = languageManager->findLanguageString( "penitentiary" );
        planets->addWidget( new Label( langString->getX(), langString->getY(), langString->getText() ) );

        // Byblos
        planets->addWidget( new Icon( 77, 371, GuiManager::getInstance()->findImage( "byblos" ) ) );
        planets->addWidget( new Icon( 90, 320, byblosFree ? chapeau : chapeauTriste ) );
        langString = languageManager->findLanguageString( "byblos" );
        planets->addWidget( new Label( langString->getX(), langString->getY(), langString->getText() ) );

        // Safari
        planets->addWidget( new Icon( 463, 371, GuiManager::getInstance()->findImage( "safari" ) ) );
        planets->addWidget( new Icon( 476, 320, safariFree ? chapeau : chapeauTriste ) );
        langString = languageManager->findLanguageString( "safari" );
        planets->addWidget( new Label( langString->getX(), langString->getY(), langString->getText() ) );

        // Blacktooth
        planets->addWidget( new Icon( 283, 251, GuiManager::getInstance()->findImage( "blacktooth" ) ) );
        planets->addWidget( new Icon( 296, 200, blacktoothFree ? chapeau : chapeauTriste ) );
        langString = languageManager->findLanguageString( "blacktooth" );
        planets->addWidget( new Label( langString->getX(), langString->getY(), langString->getText() ) );

        GuiManager::getInstance()->changeScreen( planets );
}

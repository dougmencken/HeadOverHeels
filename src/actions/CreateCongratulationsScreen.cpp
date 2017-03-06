#include "CreateCongratulationsScreen.hpp"
#include "GuiManager.hpp"
#include "LanguageManager.hpp"
#include "Screen.hpp"
#include "Icon.hpp"
#include "TextField.hpp"
#include "CreateEndScreen.hpp"

using gui::CreateCongratulationsScreen;


CreateCongratulationsScreen::CreateCongratulationsScreen( BITMAP* picture, unsigned short rooms, unsigned short planets )
: Action( ),
  where( picture ),
  rooms( rooms ),
  planets( planets )
{

}

void CreateCongratulationsScreen::doIt ()
{
        LanguageText* langString = 0;
        Screen* screen = new Screen( 0, 0, this->where );
        LanguageManager* languageManager = GuiManager::getInstance()->getLanguageManager();

        // Imagen de fondo
        screen->setBackground( GuiManager::getInstance()->findImage( "background" ) );
        screen->setEscapeAction( new CreateEndScreen( this->where, rooms, planets ) );

        // Head coronado
        screen->addWidget( new Icon( 192, 50, GuiManager::getInstance()->findImage( "crown" ) ) );
        screen->addWidget( new Icon( 192, 100, GuiManager::getInstance()->findImage( "head" ) ) );

        // Heels coronado
        screen->addWidget( new Icon( 400, 50, GuiManager::getInstance()->findImage( "crown" ) ) );
        screen->addWidget( new Icon( 400, 100, GuiManager::getInstance()->findImage( "heels" ) ) );

        // Texto final
        langString = languageManager->findLanguageString( "final-text" );
        TextField* textField = new TextField( langString->getX(), langString->getY(), 640, 480, CenterAlignment );

        for ( size_t i = 0; i < langString->getLinesCount(); i++ )
        {
                LanguageLine* line = langString->getLine( i );
                textField->addLine( line->text, line->font, line->color );
        }

        screen->addWidget( textField );

        GuiManager::getInstance()->changeScreen( screen );
}

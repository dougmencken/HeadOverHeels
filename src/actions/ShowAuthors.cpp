
#include "ShowAuthors.hpp"
#include "GuiManager.hpp"
#include "LanguageManager.hpp"
#include "SoundManager.hpp"

#include "Font.hpp"
#include "Screen.hpp"
#include "Label.hpp"
#include "TextField.hpp"
#include "CreateMainMenu.hpp"

using gui::ShowAuthors;
using gui::Label;
using gui::TextField;
using isomot::SoundManager;


ShowAuthors::ShowAuthors( BITMAP* picture )
: Action(),
  where( picture )
{

}

void ShowAuthors::doIt ()
{
        ///SoundManager::getInstance()->stopAnyOgg();
        SoundManager::getInstance()->playOgg( "music/CreditsTheme.ogg", /* loop */ true );

        Screen* screen = new Screen( 0, 0, this->where );
        screen->setBackground( GuiManager::getInstance()->findImage( "background" ) );
        screen->setEscapeAction( new CreateMainMenu( this->where ) );

        LanguageText* langString = 0;
        LanguageManager* languageManager = GuiManager::getInstance()->getLanguageManager();

        langString = languageManager->findLanguageString( "credits-text" );
        TextField* textField = new TextField( langString->getX(), langString->getY(), 640, 480, CenterAlignment );

        for ( size_t i = 0; i < langString->getLinesCount(); i++ )
        {
                LanguageLine* line = langString->getLine( i );
                textField->addLine( line->text, line->font, line->color );
        }

        screen->addWidget( textField );

        GuiManager::getInstance()->changeScreen( screen );

        // Mientras no se pulse Escape, se mueve el texto hacia arriba
        bool exit = false;
        while ( ! exit )
        {
                textField->changePosition( textField->getX(), textField->getY() - 1 );
                GuiManager::getInstance()->refresh();
                exit = ( keypressed() && key[ KEY_ESC ] );
                sleep( 45 );
        }
}

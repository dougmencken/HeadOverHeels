
#include "ShowAuthors.hpp"
#include "GuiManager.hpp"
#include "LanguageText.hpp"
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

        Screen* screen = GuiManager::getInstance()->findOrCreateScreenForAction( this, this->where );
        if ( screen->countWidgets() == 0 )
        {
                screen->setEscapeAction( new CreateMainMenu( this->where ) );

                LanguageText* langString = 0;
                LanguageManager* languageManager = GuiManager::getInstance()->getLanguageManager();

                langString = languageManager->findLanguageString( "credits-text" );
                this->initialY = where->h;
                this->linesOfCredits = new TextField( 0, initialY, 640, 480, CenterAlignment );

                size_t howManyLines = langString->getLinesCount() ;
                fprintf( stdout, "credits-text has %ld lines\n", howManyLines );
                for ( size_t i = 0; i < howManyLines; i++ )
                {
                        LanguageLine* line = langString->getLine( i );
                        linesOfCredits->addLine( line->text, line->font, line->color );
                }

                screen->addWidget( linesOfCredits );
        }
        else
        {
                // restore the initial position
                linesOfCredits->moveTo( linesOfCredits->getX(), initialY );
        }

        GuiManager::getInstance()->changeScreen( screen );

        // move text up
        bool exit = false;
        while ( ! exit )
        {
                int yNow = linesOfCredits->getY() - 1;
                if ( yNow <= -5000 )
                {
                        // let it loop
                        yNow = initialY ;
                }

                linesOfCredits->moveTo( linesOfCredits->getX(), yNow );
                GuiManager::getInstance()->refresh();

                exit = ( keypressed() && key[ KEY_ESC ] );

                if ( keypressed() && ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && ( key[ KEY_F ] ) )
                {
                        gui::GuiManager::getInstance()->toggleFullScreenVideo ();
                }

                sleep( 25 );
        }
}

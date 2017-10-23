
#include "ShowAuthors.hpp"
#include "GuiManager.hpp"
#include "LanguageText.hpp"
#include "LanguageManager.hpp"
#include "SoundManager.hpp"

#include "Font.hpp"
#include "Screen.hpp"
#include "Label.hpp"
#include "Picture.hpp"
#include "TextField.hpp"
#include "CreateMainMenu.hpp"

using gui::ShowAuthors;
using gui::Label;
using gui::Picture;
using gui::TextField;
using isomot::SoundManager;


ShowAuthors::ShowAuthors( BITMAP* picture )
: Action(),
  where( picture )
{

}

void ShowAuthors::doAction ()
{
        ///SoundManager::getInstance()->stopAnyOgg();
        SoundManager::getInstance()->playOgg( "music/CreditsTheme.ogg", /* loop */ true );

        Screen* screen = GuiManager::getInstance()->findOrCreateScreenForAction( this, this->where );
        if ( screen->countWidgets() == 0 )
        {
                LanguageText* langString = 0;
                LanguageManager* languageManager = GuiManager::getInstance()->getLanguageManager();

                langString = languageManager->findLanguageString( "credits-text" );
                this->initialY = where->h;
                this->linesOfCredits = new TextField( 0, initialY, isomot::ScreenWidth, isomot::ScreenHeight, CenterAlignment );

                size_t howManyLines = langString->getLinesCount() ;
                std::cout << "credits-text has " << howManyLines << " lines" << std::endl ;

                for ( size_t i = 0; i < howManyLines; i++ )
                {
                        LanguageLine* line = langString->getLine( i );
                        linesOfCredits->addLine( line->text, line->font, line->color );
                }

                screen->addWidget( linesOfCredits );
        }
        else
        {
                // restore initial position
                linesOfCredits->moveTo( linesOfCredits->getX(), initialY );
        }

        screen->setEscapeAction( new CreateMainMenu( this->where ) );

        GuiManager::getInstance()->changeScreen( screen );

        Picture* widgetForLoadingScreen = 0;

        // move text up

        const int heightOfCredits = ( ( linesOfCredits->getHeightOfField() + 1 ) >> 1 ) << 1;
        const int verticalSpace = ( this->where->h * 3 ) >> 2;
        const int whenToReloop = - ( heightOfCredits + verticalSpace ) ;

        fprintf( stdout, "height of credits-text is %d\n", heightOfCredits );

        while ( ! screen->getEscapeAction()->hasBegun() )
        {
                int yNow = linesOfCredits->getY() - 1;

                if ( keypressed() && key[ KEY_SPACE ] )
                {
                        if ( key_shifts & KB_SHIFT_FLAG )
                        {
                                if ( key_shifts & KB_ALT_FLAG )
                                        yNow += 2 ;
                                else
                                        yNow ++ ;
                        }
                }

                if ( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && key[ KEY_LEFT ] )
                {
                        linesOfCredits->moveTo( linesOfCredits->getX () - 1, linesOfCredits->getY () );
                }
                if ( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && key[ KEY_RIGHT ] )
                {
                        linesOfCredits->moveTo( linesOfCredits->getX () + 1, linesOfCredits->getY () );
                }
                if ( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && key[ KEY_0_PAD ] )
                {
                        linesOfCredits->moveTo( 0, linesOfCredits->getY () );
                }

                if ( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && key[ KEY_L ] )
                {
                        linesOfCredits->setAlignment( LeftAlignment );
                }
                if ( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && key[ KEY_C ] )
                {
                        linesOfCredits->setAlignment( CenterAlignment );
                }
                if ( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && key[ KEY_R ] )
                {
                        linesOfCredits->setAlignment( RightAlignment );
                }

                if ( yNow <= whenToReloop )
                {
                        // loop it
                        yNow = initialY ;
                }

                linesOfCredits->moveTo( linesOfCredits->getX(), yNow );

                if ( yNow == this->where->h - heightOfCredits && widgetForLoadingScreen == 0 )
                {
                        BITMAP* loadingScreen = load_png( isomot::pathToFile( isomot::sharePath() + "loading-screen.png" ), 0 );
                        if ( loadingScreen != 0 )
                        {
                                widgetForLoadingScreen = new Picture(
                                                ( this->where->w - loadingScreen->w ) >> 1, this->where->h,
                                                loadingScreen,
                                                "loading screen from original speccy version"
                                ) ;
                                screen->addWidget( widgetForLoadingScreen );
                        }
                }
                else if ( yNow < this->where->h - heightOfCredits && widgetForLoadingScreen != 0 )
                {
                        widgetForLoadingScreen->moveTo( widgetForLoadingScreen->getX(), yNow + heightOfCredits );
                }
                else if ( widgetForLoadingScreen != 0 )
                {
                        screen->removeWidget( widgetForLoadingScreen );
                        widgetForLoadingScreen = 0;
                }

                GuiManager::getInstance()->redraw ();

                if ( keypressed() && ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && ( key[ KEY_F ] ) )
                {
                        gui::GuiManager::getInstance()->toggleFullScreenVideo ();
                }

                if ( ! ( keypressed() && key[ KEY_SPACE ] ) )
                {
                        sleep( 20 );
                }

                if ( keypressed() && key[ KEY_ESC ] )
                {
                        clear_keybuf();
                        screen->handleKey( KEY_ESC << 8 );
                }
        }

        if ( widgetForLoadingScreen != 0 )
        {
                screen->removeWidget( widgetForLoadingScreen );
                widgetForLoadingScreen = 0;
        }
}

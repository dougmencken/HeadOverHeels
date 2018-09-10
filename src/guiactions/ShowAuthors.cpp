
#include "ShowAuthors.hpp"
#include "GuiManager.hpp"
#include "LanguageText.hpp"
#include "LanguageManager.hpp"
#include "SoundManager.hpp"

#include "Font.hpp"
#include "Screen.hpp"
#include "Label.hpp"
#include "PictureWidget.hpp"
#include "TextField.hpp"
#include "CreateMainMenu.hpp"

using gui::ShowAuthors;
using gui::Label;
using gui::PictureWidget;
using gui::TextField;
using isomot::SoundManager;


ShowAuthors::ShowAuthors( allegro::Pict* picture ) : Action( picture )
{

}

void ShowAuthors::doAction ()
{
        ///SoundManager::getInstance()->stopAnyOgg();
        SoundManager::getInstance()->playOgg( "music/CreditsTheme.ogg", /* loop */ true );

        int heightOfWhereToDraw = getWhereToDraw()->h ;

        Screen* screen = GuiManager::getInstance()->findOrCreateScreenForAction( this );
        if ( screen->countWidgets() == 0 )
        {
                LanguageText* langString = nilPointer;
                LanguageManager* languageManager = GuiManager::getInstance()->getLanguageManager();

                langString = languageManager->findLanguageStringForAlias( "credits-text" );
                this->initialY = heightOfWhereToDraw;
                this->linesOfCredits = new TextField( isomot::ScreenWidth(), "center" );
                this->linesOfCredits->moveTo( 0, initialY );

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

        screen->setEscapeAction( new CreateMainMenu( getWhereToDraw() ) );

        GuiManager::getInstance()->changeScreen( screen, true );

        PictureWidget* widgetForLoadingScreen = nilPointer;

        // move text up

        const int heightOfCredits = ( ( linesOfCredits->getHeightOfField() + 1 ) >> 1 ) << 1;
        const int verticalSpace = ( getWhereToDraw()->h * 3 ) >> 2;
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
                        linesOfCredits->setAlignment( "left" );
                }
                if ( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && key[ KEY_C ] )
                {
                        linesOfCredits->setAlignment( "center" );
                }
                if ( ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && key[ KEY_R ] )
                {
                        linesOfCredits->setAlignment( "right" );
                }

                if ( yNow <= whenToReloop )
                {
                        // loop it
                        yNow = initialY ;
                }

                linesOfCredits->moveTo( linesOfCredits->getX(), yNow );

                if ( yNow == heightOfWhereToDraw - heightOfCredits && widgetForLoadingScreen == nilPointer )
                {
                        Picture* loadingScreen = new Picture( allegro::loadPNG( isomot::pathToFile( isomot::sharePath() + "loading-screen.png" ) ) );
                        loadingScreen->setName( "image of loading screen from original speccy version" );

                        if ( loadingScreen->getAllegroPict() != nilPointer )
                        {
                                widgetForLoadingScreen = new PictureWidget(
                                                ( getWhereToDraw()->w - loadingScreen->getWidth() ) >> 1, heightOfWhereToDraw,
                                                loadingScreen,
                                                "loading screen from original speccy version"
                                ) ;
                                screen->addWidget( widgetForLoadingScreen );
                        }
                }
                else if ( yNow < heightOfWhereToDraw - heightOfCredits && widgetForLoadingScreen != nilPointer )
                {
                        widgetForLoadingScreen->moveTo( widgetForLoadingScreen->getX(), yNow + heightOfCredits );
                }
                else if ( widgetForLoadingScreen != nilPointer )
                {
                        screen->removeWidget( widgetForLoadingScreen );
                        widgetForLoadingScreen = nilPointer;
                }

                GuiManager::getInstance()->redraw ();

                if ( keypressed() && ( key_shifts & KB_ALT_FLAG ) && ( key_shifts & KB_SHIFT_FLAG ) && ( key[ KEY_F ] ) )
                {
                        gui::GuiManager::getInstance()->toggleFullScreenVideo ();
                }

                if ( ! ( keypressed() && key[ KEY_SPACE ] ) )
                {
                        milliSleep( 20 );
                }

                if ( keypressed() && key[ KEY_ESC ] )
                {
                        clear_keybuf();
                        screen->handleKey( KEY_ESC << 8 );
                }
        }

        if ( widgetForLoadingScreen != nilPointer )
        {
                screen->removeWidget( widgetForLoadingScreen );
                widgetForLoadingScreen = nilPointer;
        }
}

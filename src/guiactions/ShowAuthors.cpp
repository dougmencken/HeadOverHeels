
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


ShowAuthors::ShowAuthors( Picture* picture )
        : Action( picture )
        , linesOfCredits( nilPointer )
        , initialY( 0 )
        , loadingScreen( nilPointer )
{
}

ShowAuthors::~ShowAuthors( )
{
        delete linesOfCredits ;
        delete loadingScreen ;
}

void ShowAuthors::doAction ()
{
        SoundManager::getInstance()->playOgg( "music/CreditsTheme.ogg", /* loop */ true );

        int heightOfWhereToDraw = getWhereToDraw()->getHeight() ;

        Screen* screen = GuiManager::getInstance()->findOrCreateScreenForAction( this );
        if ( screen->countWidgets() == 0 )
        {
                LanguageManager* languageManager = GuiManager::getInstance()->getLanguageManager();
                LanguageText* langString = languageManager->findLanguageStringForAlias( "credits-text" );

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
        const int verticalSpace = ( getWhereToDraw()->getHeight() * 3 ) >> 2;
        const int whenToReloop = - ( heightOfCredits + verticalSpace ) ;

        std::cout << "height of credits-text is " << heightOfCredits << std::endl ;

        while ( ! screen->getEscapeAction()->hasBegun() )
        {
                int yNow = linesOfCredits->getY() - 1;

                if ( allegro::isKeyPushed( "Space" ) && allegro::isShiftKeyPushed() )
                {
                        if ( allegro::isAltKeyPushed() )
                                yNow += 2 ;
                        else
                                yNow ++ ;
                }

                if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "Left" ) )
                {
                        linesOfCredits->moveTo( linesOfCredits->getX () - 1, linesOfCredits->getY () );
                }
                if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "Right" ) )
                {
                        linesOfCredits->moveTo( linesOfCredits->getX () + 1, linesOfCredits->getY () );
                }
                if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "Pad 0" ) )
                {
                        linesOfCredits->moveTo( 0, linesOfCredits->getY () );
                }

                if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "l" ) )
                {
                        linesOfCredits->setAlignment( "left" );
                }
                if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "c" ) )
                {
                        linesOfCredits->setAlignment( "center" );
                }
                if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "r" ) )
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
                        if ( loadingScreen == nilPointer )
                        {
                                smartptr< allegro::Pict > png( allegro::loadPNG( isomot::pathToFile( isomot::sharePath() + "loading-screen.png" ) ) );
                                loadingScreen = new Picture( * png.get() );
                                loadingScreen->setName( "image of loading screen from original speccy version" );
                        }

                        if ( loadingScreen->getAllegroPict().isNotNil() )
                        {
                                widgetForLoadingScreen = new PictureWidget(
                                                ( getWhereToDraw()->getWidth() - loadingScreen->getWidth() ) >> 1, heightOfWhereToDraw,
                                                new Picture( *loadingScreen ),
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

                if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "f" ) )
                {
                        gui::GuiManager::getInstance()->toggleFullScreenVideo ();
                }

                if ( ! allegro::isKeyPushed( "Space" ) )
                {
                        milliSleep( 20 );
                }

                if ( allegro::isKeyPushed( "Escape" ) )
                {
                        allegro::emptyKeyboardBuffer();
                        screen->handleKey( "Escape" );
                }
        }

        if ( widgetForLoadingScreen != nilPointer )
        {
                screen->removeWidget( widgetForLoadingScreen );
        }
}

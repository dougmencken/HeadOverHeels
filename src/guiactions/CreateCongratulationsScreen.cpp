
#include "CreateCongratulationsScreen.hpp"

#include "GameManager.hpp"
#include "GuiManager.hpp"
#include "LanguageManager.hpp"
#include "LanguageText.hpp"
#include "Screen.hpp"
#include "PictureWidget.hpp"
#include "TextField.hpp"
#include "CreateEndScreen.hpp"

using gui::CreateCongratulationsScreen ;
using iso::GameManager ;


CreateCongratulationsScreen::CreateCongratulationsScreen( Picture * picture, unsigned short rooms, unsigned short planets )
        : Action( picture )
        , rooms( rooms )
        , planets( planets )
{

}

CreateCongratulationsScreen::~CreateCongratulationsScreen( )
{

}

void CreateCongratulationsScreen::doAction ()
{
        LanguageManager* languageManager = GuiManager::getInstance().getLanguageManager();
        LanguageText* langString = 0;

        Screen* screen = GuiManager::getInstance().findOrCreateScreenForAction( this );

        if ( screen->countWidgets() == 0 )
        {
                screen->setEscapeAction( new CreateEndScreen( getWhereToDraw(), rooms, planets ) );
        }
        else
        {
                screen->freeWidgets() ;
        }

        // Head coronado
        screen->addWidget( new PictureWidget( 192, 50, PicturePtr( Screen::loadPicture( "crown.png" ) ), "crown.png" ) );
        screen->addPictureOfHeadAt( 192, 100 );

        // Heels coronado
        screen->addWidget( new PictureWidget( 400, 50, PicturePtr( Screen::loadPicture( "crown.png" ) ), "crown.png" ) );
        screen->addPictureOfHeelsAt( 400, 100 );

        // Texto final
        langString = languageManager->findLanguageStringForAlias( "final-text" );
        TextField* textField = new TextField( iso::ScreenWidth(), "center" );
        textField->moveTo( 0, 180 );

        for ( size_t i = 0; i < langString->getLinesCount(); i++ )
        {
                LanguageLine* line = langString->getLine( i );
                textField->addLine( line->text, line->font, line->color );
        }

        screen->addWidget( textField );

        GuiManager::getInstance().changeScreen( screen, true );
}

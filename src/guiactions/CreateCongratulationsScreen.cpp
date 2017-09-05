
#include "CreateCongratulationsScreen.hpp"

#include "GameManager.hpp"
#include "GuiManager.hpp"
#include "LanguageManager.hpp"
#include "LanguageText.hpp"
#include "Screen.hpp"
#include "Icon.hpp"
#include "TextField.hpp"
#include "CreateEndScreen.hpp"

using gui::CreateCongratulationsScreen ;
using isomot::GameManager ;


CreateCongratulationsScreen::CreateCongratulationsScreen( BITMAP* picture, unsigned short rooms, unsigned short planets ) : Action( )
	, where( picture )
	, rooms( rooms )
	, planets( planets )
	, pictureOfCrown( 0 )
{

}

CreateCongratulationsScreen::~CreateCongratulationsScreen( )
{
        delete pictureOfCrown ;
}

void CreateCongratulationsScreen::doIt ()
{
        LanguageManager* languageManager = GuiManager::getInstance()->getLanguageManager();
        LanguageText* langString = 0;

        Screen* screen = GuiManager::getInstance()->findOrCreateScreenForAction( this, this->where );

        if ( screen->countWidgets() == 0 )
        {
                screen->setEscapeAction( new CreateEndScreen( this->where, rooms, planets ) );
        }
        else
        {
                screen->freeWidgets() ;
        }

        this->pictureOfCrown = GameManager::refreshPicture( pictureOfCrown, "crown.png" );

        // Head coronado
        screen->addWidget( new Icon( 192, 50, pictureOfCrown ) );
        screen->addIconOfHeadAt( 192, 100 );

        // Heels coronado
        screen->addWidget( new Icon( 400, 50, pictureOfCrown ) );
        screen->addIconOfHeelsAt( 400, 100 );

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

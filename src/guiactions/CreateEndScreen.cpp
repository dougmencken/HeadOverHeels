
#include "CreateEndScreen.hpp"
#include "GuiManager.hpp"
#include "LanguageText.hpp"
#include "LanguageManager.hpp"
#include "SoundManager.hpp"
#include "Font.hpp"
#include "Screen.hpp"
#include "Menu.hpp"
#include "Label.hpp"
#include "CreateMainMenu.hpp"

using gui::CreateEndScreen;
using isomot::SoundManager;
using isomot::ScreenWidth;


CreateEndScreen::CreateEndScreen( BITMAP* picture, unsigned int rooms, unsigned short planets )
        : Action( picture )
        , rooms( rooms )
        , planets( planets )
{
}

void CreateEndScreen::doAction ()
{
        SoundManager::getInstance()->playOgg( "music/MainTheme.ogg", /* loop */ true );

        Screen* screen = GuiManager::getInstance()->findOrCreateScreenForAction( this );
        if ( screen->countWidgets() > 0 )
        {
                screen->freeWidgets();
        }
        else
        {
                screen->setEscapeAction( new CreateMainMenu( getWhereToDraw() ) );
        }

        screen->placeHeadAndHeels( /* icons */ true, /* copyrights */ false );

        LanguageManager* languageManager = GuiManager::getInstance()->getLanguageManager();
        Label* label = 0;
        std::stringstream ss;

        // score reached by the player
        unsigned int score = this->rooms * 160 + this->planets * 10000;
        ss.str( std::string() );
        ss << score;
        label = new Label( languageManager->findLanguageString( "score" )->getText() + " " + ss.str(), "regular", "yellow" );
        label->moveTo( ( ScreenWidth - label->getWidth() ) >> 1, 300 );
        screen->addWidget( label );

        // count of visited rooms
        std::string exploredRooms = languageManager->findLanguageString( "explored-rooms" )->getText();
        ss.str( std::string() );
        ss << this->rooms;
        size_t prooms = exploredRooms.find( "%d" );
        exploredRooms.replace( prooms, 2, ss.str() );
        label = new Label( exploredRooms, "regular", "cyan" );
        label->moveTo( ( ScreenWidth - label->getWidth() ) >> 1, 340 );
        screen->addWidget( label );

        // count of liberated planets
        std::string liberatedPlanets = languageManager->findLanguageString( "liberated-planets" )->getText();
        ss.str( std::string() );
        ss << this->planets;
        size_t pplanets = liberatedPlanets.find( "%d" );
        liberatedPlanets.replace( pplanets, 2, ss.str() );
        label = new Label( liberatedPlanets, "regular", "white" );
        label->moveTo( ( ScreenWidth - label->getWidth() ) >> 1, 380 );
        screen->addWidget( label );

        // range reached by player
        unsigned int bounds[ ] = {  0, 8000, 20000, 30000, 55000, 84000  };
        std::string ranges[ ] = {  "dummy", "novice", "spy", "master-spy", "hero", "emperor"  };

        if ( score == 0 )
        {
                label = new Label( "fix the game please", "big", "orange" );
                label->moveTo( ( ScreenWidth - label->getWidth() ) >> 1, 180 );
                screen->addWidget( label );
        }
        else
        {
                for ( int i = 5; i >= 0; i-- )
                {
                        if ( score > bounds[ i ] )
                        {
                                label = new Label( languageManager->findLanguageString( ranges[ i ] )->getText(), "big", "multicolor" );
                                label->moveTo( ( ScreenWidth - label->getWidth() ) >> 1, 180 );
                                screen->addWidget( label );

                                break;
                        }
                }
        }

        label->setAction( screen->getEscapeAction () );
        screen->setKeyHandler( label );

        GuiManager::getInstance()->changeScreen( screen, false );
}

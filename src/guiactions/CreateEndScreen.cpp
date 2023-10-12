
#include "CreateEndScreen.hpp"

#include "GameManager.hpp"
#include "GuiManager.hpp"
#include "LanguageText.hpp"
#include "LanguageManager.hpp"
#include "SoundManager.hpp"
#include "Font.hpp"
#include "Screen.hpp"
#include "Menu.hpp"
#include "Label.hpp"
#include "TextField.hpp"
#include "CreateMainMenu.hpp"

#include "screen.hpp"

using gui::CreateEndScreen;
using iso::SoundManager;


CreateEndScreen::CreateEndScreen( unsigned int rooms, unsigned short planets )
        : Action( )
        , rooms( rooms )
        , planets( planets )
{
}

void CreateEndScreen::doAction ()
{
        SoundManager::getInstance().playOgg( "music/MainTheme.ogg", /* loop */ true );

        if ( game::GameManager::getInstance().isSimpleGraphicsSet () )
                Screen::refreshBackground () ; // get the background back

        Screen & screen = * GuiManager::getInstance().findOrCreateScreenForAction( this );
        if ( screen.countWidgets() > 0 )
        {
                screen.freeWidgets();
        }
        else
        {
                screen.setEscapeAction( new CreateMainMenu() );
        }

        screen.placeHeadAndHeels( /* icons */ true, /* copyrights */ false );

        const unsigned int leading = 40 ;
        const unsigned int screenWidth = variables::getScreenWidth();
        const unsigned int screenHeight = variables::getScreenHeight();
        const unsigned int space = ( screenWidth / 20 ) - 10;
        const unsigned int labelsY = screenHeight - ( leading * 3 ) - ( space << 1 ) ;
        const unsigned int resultY = ( screenHeight >> 1 ) - ( screenHeight >> 4 ) - 20 ;

        LanguageManager* languageManager = GuiManager::getInstance().getLanguageManager();

        // score reached by the player
        unsigned int score = this->rooms * 160 + this->planets * 10000;
        Label* scoreLabel = new Label( languageManager->findLanguageStringForAlias( "score" )->getText() + " " + util::number2string( score ), "", "yellow" );
        scoreLabel->moveTo( ( screenWidth - scoreLabel->getWidth() ) >> 1, labelsY );
        screen.addWidget( scoreLabel );

        // count of visited rooms
        std::string exploredRooms = languageManager->findLanguageStringForAlias( "explored-rooms" )->getText();
        exploredRooms.replace( exploredRooms.find( "%d" ), 2, util::number2string( this->rooms ) );
        Label* rooms = new Label( exploredRooms, "", "cyan" );
        rooms->moveTo( ( screenWidth - rooms->getWidth() ) >> 1, labelsY + leading );
        screen.addWidget( rooms );

        // count of liberated planets
        std::string liberatedPlanets = languageManager->findLanguageStringForAlias( "liberated-planets" )->getText();
        liberatedPlanets.replace( liberatedPlanets.find( "%d" ), 2, util::number2string( this->planets ) );
        Label* planets = new Label( liberatedPlanets, "", "white" );
        planets->moveTo( ( screenWidth - planets->getWidth() ) >> 1, labelsY + leading + leading );
        screen.addWidget( planets );

        if ( score == 0 )
        {
                TextField* result = new TextField( screenWidth, "center" );
                result->addLine( "fix the game please", "big", "orange" );
                result->moveTo( 0, resultY );
                screen.addWidget( result );
        }
        else
        {
                // range reached by player
                unsigned int bounds[ ] = {  0, 8000, 20000, 30000, 55000, 84000  };
                std::string ranges[ ] = {  "dummy", "novice", "spy", "master-spy", "hero", "emperor"  };

                for ( int i = 5; i >= 0; i-- )
                {
                        if ( score > bounds[ i ] )
                        {
                                TextField* result = new TextField( screenWidth, "center" );
                                result->addLine( languageManager->findLanguageStringForAlias( ranges[ i ] )->getText(), "big", "multicolor" );
                                result->moveTo( 0, resultY );
                                screen.addWidget( result );

                                break;
                        }
                }
        }

        scoreLabel->setAction( screen.getEscapeAction () );
        screen.setKeyHandler( scoreLabel );

        GuiManager::getInstance().changeScreen( screen, false );
}

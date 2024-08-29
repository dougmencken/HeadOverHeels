
#include "CreateGameOverSlide.hpp"

#include "GamePreferences.hpp"
#include "GameManager.hpp"
#include "GuiManager.hpp"
#include "LanguageStrings.hpp"
#include "SoundManager.hpp"
#include "Font.hpp"
#include "Slide.hpp"
#include "Menu.hpp"
#include "Label.hpp"
#include "TextField.hpp"
#include "CreateMainMenu.hpp"


void gui::CreateGameOverSlide::act ()
{
        if ( GameManager::getInstance().isSimpleGraphicsSet () )
                Slide::refreshBackground () ; // get the background back

        Slide & gameOverSlide = GuiManager::getInstance().findOrCreateSlideForAction( getNameOfAction() );
        if ( ! gameOverSlide.isNewAndEmpty() )
                gameOverSlide.freeWidgets();
        else
                gameOverSlide.setEscapeAction( new CreateMainMenu() );

        gameOverSlide.placeHeadAndHeels( /* icons */ true, /* copyrights */ false );

        const unsigned int leading = 40 ;
        const unsigned int screenWidth = GamePreferences::getScreenWidth();
        const unsigned int screenHeight = GamePreferences::getScreenHeight();
        const unsigned int space = ( screenWidth / 20 ) - 10;
        const unsigned int labelsY = screenHeight - ( leading * 3 ) - ( space << 1 ) ;
        const unsigned int resultY = ( screenHeight >> 1 ) - ( screenHeight >> 4 ) - 20 ;

        LanguageStrings & languageStrings = GuiManager::getInstance().getOrMakeLanguageStrings() ;

        // the score reached by the player
        unsigned int score = this->visitedRooms * 160 + this->liberatedPlanets * 10000 ;
        Label* scoreLabel = new Label ( languageStrings.getTranslatedTextByAlias( "score" ).getText() + " " + util::number2string( score ),
                                        new Font( "yellow" ) );
        scoreLabel->moveTo( ( screenWidth - scoreLabel->getWidth() ) >> 1, labelsY );
        gameOverSlide.addWidget( scoreLabel );

        // the number of the rooms visited
        std::string exploredRooms = languageStrings.getTranslatedTextByAlias( "explored-rooms" ).getText();
        exploredRooms.replace( exploredRooms.find( "%d" ), 2, util::number2string( this->visitedRooms ) );
        Label* rooms = new Label( exploredRooms, new Font( "cyan" ) );
        rooms->moveTo( ( screenWidth - rooms->getWidth() ) >> 1, labelsY + leading );
        gameOverSlide.addWidget( rooms );

        // the number of the planets liberated
        std::string liberatedPlanets = languageStrings.getTranslatedTextByAlias( "liberated-planets" ).getText();
        liberatedPlanets.replace( liberatedPlanets.find( "%d" ), 2, util::number2string( this->liberatedPlanets ) );
        Label* planets = new Label( liberatedPlanets, new Font( "white" ) );
        planets->moveTo( ( screenWidth - planets->getWidth() ) >> 1, labelsY + leading + leading );
        gameOverSlide.addWidget( planets );

        std::cout << "game over with score " << score
                        << " for " << this->visitedRooms << " visited rooms"
                        << " and " << this->liberatedPlanets << " liberated planets"
                        << std::endl ;

        if ( score == 0 ) {
                TextField* result = new TextField( screenWidth, "center" );
                result->appendLine( "fix the game please", "big", "orange" );
                result->moveTo( 0, resultY );
                gameOverSlide.addWidget( result );
        }
        else {
                // the range reached by the player
                unsigned int bounds[ ] = {  0, 8000, 20000, 30000, 55000, 84000  };
                std::string ranges[ ] = {  "dummy", "novice", "spy", "master-spy", "hero", "liberator"  };

                for ( int i = 5; i >= 0; i-- )
                {
                        if ( score > bounds[ i ] )
                        {
                                TextField* result = new TextField( screenWidth, "center" );
                                result->appendLine( languageStrings.getTranslatedTextByAlias( ranges[ i ] ).getText(), "big", "multicolor" );
                                result->moveTo( 0, resultY );
                                gameOverSlide.addWidget( result );

                                break;
                        }
                }
        }

        scoreLabel->setAction( gameOverSlide.getEscapeAction () );
        gameOverSlide.setKeyHandler( scoreLabel );

        SoundManager::getInstance().playOgg( "music/MainTheme.ogg", /* loop */ true );

        gameOverSlide.setTransitionToThisSlideOff() ;
        GuiManager::getInstance().changeSlide( getNameOfAction(), false );
}

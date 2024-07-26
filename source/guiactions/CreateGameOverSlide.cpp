
#include "CreateGameOverSlide.hpp"

#include "GamePreferences.hpp"
#include "GameManager.hpp"
#include "GuiManager.hpp"
#include "LanguageStrings.hpp"
#include "SoundManager.hpp"
#include "Font.hpp"
#include "Screen.hpp"
#include "Menu.hpp"
#include "Label.hpp"
#include "TextField.hpp"
#include "CreateMainMenu.hpp"


void gui::CreateGameOverSlide::act ()
{
        if ( GameManager::getInstance().isSimpleGraphicsSet () )
                Screen::refreshBackground () ; // get the background back

        Screen & slide = * GuiManager::getInstance().findOrCreateSlideForAction( getNameOfAction() );
        if ( ! slide.isNewAndEmpty() )
                slide.freeWidgets();
        else
                slide.setEscapeAction( new CreateMainMenu() );

        slide.placeHeadAndHeels( /* icons */ true, /* copyrights */ false );

        const unsigned int leading = 40 ;
        const unsigned int screenWidth = GamePreferences::getScreenWidth();
        const unsigned int screenHeight = GamePreferences::getScreenHeight();
        const unsigned int space = ( screenWidth / 20 ) - 10;
        const unsigned int labelsY = screenHeight - ( leading * 3 ) - ( space << 1 ) ;
        const unsigned int resultY = ( screenHeight >> 1 ) - ( screenHeight >> 4 ) - 20 ;

        LanguageStrings* languageStrings = GuiManager::getInstance().getLanguageStrings() ;

        // the score reached by the player
        unsigned int score = this->visitedRooms * 160 + this->liberatedPlanets * 10000 ;
        Label* scoreLabel = new Label ( languageStrings->getTranslatedTextByAlias( "score" )->getText() + " " + util::number2string( score ),
                                        new Font( "yellow" ) );
        scoreLabel->moveTo( ( screenWidth - scoreLabel->getWidth() ) >> 1, labelsY );
        slide.addWidget( scoreLabel );

        // the number of the rooms visited
        std::string exploredRooms = languageStrings->getTranslatedTextByAlias( "explored-rooms" )->getText();
        exploredRooms.replace( exploredRooms.find( "%d" ), 2, util::number2string( this->visitedRooms ) );
        Label* rooms = new Label( exploredRooms, new Font( "cyan" ) );
        rooms->moveTo( ( screenWidth - rooms->getWidth() ) >> 1, labelsY + leading );
        slide.addWidget( rooms );

        // the number of the planets liberated
        std::string liberatedPlanets = languageStrings->getTranslatedTextByAlias( "liberated-planets" )->getText();
        liberatedPlanets.replace( liberatedPlanets.find( "%d" ), 2, util::number2string( this->liberatedPlanets ) );
        Label* planets = new Label( liberatedPlanets, new Font( "white" ) );
        planets->moveTo( ( screenWidth - planets->getWidth() ) >> 1, labelsY + leading + leading );
        slide.addWidget( planets );

        std::cout << "game over with score " << score
                        << " for " << this->visitedRooms << " visited rooms"
                        << " and " << this->liberatedPlanets << " liberated planets"
                        << std::endl ;

        if ( score == 0 ) {
                TextField* result = new TextField( screenWidth, "center" );
                result->appendText( "fix the game please", "big", "orange" );
                result->moveTo( 0, resultY );
                slide.addWidget( result );
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
                                result->appendText( languageStrings->getTranslatedTextByAlias( ranges[ i ] )->getText(), "big", "multicolor" );
                                result->moveTo( 0, resultY );
                                slide.addWidget( result );

                                break;
                        }
                }
        }

        scoreLabel->setAction( slide.getEscapeAction () );
        slide.setKeyHandler( scoreLabel );

        SoundManager::getInstance().playOgg( "music/MainTheme.ogg", /* loop */ true );
        GuiManager::getInstance().changeSlide( getNameOfAction(), false );
}

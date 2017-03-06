
#include "CreateEndScreen.hpp"
#include "GuiManager.hpp"
#include "LanguageManager.hpp"
#include "SoundManager.hpp"
#include "Font.hpp"
#include "Screen.hpp"
#include "Menu.hpp"
#include "Icon.hpp"
#include "Label.hpp"
#include "CreateMainMenu.hpp"

using gui::CreateEndScreen;
using isomot::SoundManager;


CreateEndScreen::CreateEndScreen( BITMAP* picture, unsigned short rooms, unsigned short planets )
: Action(),
  where( picture ),
  rooms( rooms ),
  planets( planets )
{
}

void CreateEndScreen::doIt ()
{
        ///SoundManager::getInstance()->stopAnyOgg();
        SoundManager::getInstance()->playOgg( "music/MainTheme.ogg", /* loop */ true );

        Action * mainMenu = new CreateMainMenu( this->where ) ;

        Screen* screen = new Screen( 0, 0, this->where );
        screen->setBackground( GuiManager::getInstance()->findImage( "background" ) );
        screen->setEscapeAction( mainMenu );

        CreateMainMenu::placeHeadAndHeels( screen, /* icons */ true, /* copyrights */ false );

        Label* label = 0;
        LanguageText* langString = 0;
        LanguageManager* languageManager = GuiManager::getInstance()->getLanguageManager();
        std::stringstream ss;

        // Puntuación alcanzada por el jugador
        unsigned int score = this->rooms * 160 + this->planets * 10000;
        langString = languageManager->findLanguageString( "score" );
        ss.str( std::string() );
        ss << score;
        label = new Label( 0, 0, langString->getText() + " " + ss.str(), "regular", "yellow" );
        label->changePosition( ( 640 - label->getWidth() ) >> 1, langString->getY() );
        screen->addWidget( label );

        // Número de salas visitadas
        langString = languageManager->findLanguageString( "explored-rooms" );
        std::string exploredRooms = langString->getText();
        ss.str( std::string() );
        ss << this->rooms;
        size_t prooms = exploredRooms.find( "%d" );
        exploredRooms.replace( prooms, 2, ss.str() );
        label = new Label( 0, 0, exploredRooms, "regular", "cyan" );
        label->changePosition( ( 640 - label->getWidth() ) >> 1, langString->getY() );
        screen->addWidget( label );

        // Número de planetas liberados
        langString = languageManager->findLanguageString( "liberated-planets" );
        std::string liberatedPlanets = langString->getText();
        ss.str( std::string() );
        ss << this->planets;
        size_t pplanets = liberatedPlanets.find( "%d" );
        liberatedPlanets.replace( pplanets, 2, ss.str() );
        label = new Label( 0, 0, liberatedPlanets, "regular", "white" );
        label->changePosition( ( 640 - label->getWidth() ) >> 1, langString->getY() );
        screen->addWidget( label );

        // Rango alcanzado por el jugador
        unsigned int bounds[ ] = {  0, 8000, 20000, 30000, 55000, 84000, 0xffffffff  };
        std::string ranges[ ] = {  "dummy", "novice", "spy", "master-spy", "hero", "emperor"  };

        // Obtiene el rango en función de la puntuación conseguida
        for ( int i = 0; i < 6; i++ )
        {
                if ( score > bounds[ i ] && score <= bounds[ i + 1 ] )
                {
                        langString = languageManager->findLanguageString( ranges[ i ] );
                        label = new Label( 0, 0, langString->getText(), "big", "multicolor" );
                        label->changePosition( ( 640 - label->getWidth() ) >> 1, langString->getY() );
                        screen->addWidget( label );
                }
        }

        label->setAction( mainMenu );
        screen->setNext( label );

        GuiManager::getInstance()->changeScreen( screen );
}


#include "CreateListOfSavedGames.hpp"
#include "GuiManager.hpp"
#include "LanguageText.hpp"
#include "LanguageManager.hpp"
#include "Font.hpp"
#include "Screen.hpp"
#include "Menu.hpp"
#include "Label.hpp"
#include "CreateMainMenu.hpp"
#include "LoadGame.hpp"
#include "SaveGame.hpp"
#include "ContinueGame.hpp"
#include "PlaySound.hpp"
#include "Ism.hpp"

using gui::CreateListOfSavedGames;
using gui::ContinueGame;


CreateListOfSavedGames::CreateListOfSavedGames( BITMAP* picture, bool isLoadMenu )
        : Action( picture )
        , isMenuForLoad( isLoadMenu )
{

}

void CreateListOfSavedGames::doAction ()
{
        Screen* screen = GuiManager::getInstance()->findOrCreateScreenForAction( this );
        if ( screen->countWidgets() > 0 )
        {
                screen->freeWidgets() ;
        }

        if ( isLoadMenu() )
        {
                // return to main menu
                screen->setEscapeAction( new CreateMainMenu( getWhereToDraw() ) );
        }
        else
        {
                // return to play
                screen->setEscapeAction( new ContinueGame( getWhereToDraw(), true ) );
        }

        screen->placeHeadAndHeels( /* icons */ true, /* copyrights */ false );

        LanguageManager* languageManager = GuiManager::getInstance()->getLanguageManager();

        // list of games
        Menu* menu = new Menu( );
        menu->setVerticalOffset( 112 );
        for ( unsigned int fileCount = 1; fileCount <= howManySaves; fileCount++ )
        {
                std::stringstream ss;
                ss << fileCount;
                std::string file = isomot::homePath() + "savegame" + pathSeparator + "saved" + ss.str () + ".xml" ;

                bool fileExists = false;
                std::ifstream in( file.c_str() );
                if ( in.good() ) fileExists = true;

                if ( fileExists )
                {
                        short rooms = 0;
                        short planets = 0;
                        readSomeInfoFromGamefile( file, &rooms, &planets );
                        ss.str( std::string() );
                        ss << rooms << " " << languageManager->findLanguageStringForAlias( "rooms" )->getText() << " "
                                << planets << " " << languageManager->findLanguageStringForAlias( "planets" )->getText();
                        Label* label = new Label( ss.str() );

                        if ( isLoadMenu() )
                                label->setAction( new LoadGame( getWhereToDraw(), fileCount ) );
                        else
                                label->setAction( new SaveGame( getWhereToDraw(), fileCount ) );
                                // very funny to change to LoadGame here by the way to get many heads/heels, just try it

                        menu->addOption( label );
                }
                else
                {
                        std::cout << "save \"" << file << "\" is yet free" << std::endl ;

                        ss.str( std::string() );
                        ss << languageManager->findLanguageStringForAlias( "free-slot" )->getText();
                        Label* labelOfFree = new Label( ss.str() );
                        if ( isLoadMenu() )
                        {
                                labelOfFree->changeColor( "cyan" );
                                labelOfFree->setAction( new PlaySound( isomot::Mistake ) );
                        }
                        else
                        {
                                labelOfFree->changeColor( "orange" );
                                labelOfFree->setAction( new SaveGame( getWhereToDraw(), fileCount ) );
                        }

                        menu->addOption( labelOfFree );
                }
        }

        screen->addWidget( menu );
        screen->setKeyHandler( menu );

        GuiManager::getInstance()->changeScreen( screen, true );
}

void CreateListOfSavedGames::readSomeInfoFromGamefile( const std::string& fileName, short* rooms, short* planets )
{
        try
        {
                sharedsmartptr< sgxml::SaveGameXML > saveGameXML( sgxml::savegame( fileName.c_str() ) );

                *rooms = saveGameXML->exploredRooms().visited().size();
                *planets = ( saveGameXML->freeByblos() ? 1 : 0 ) +
                                ( saveGameXML->freeEgyptus() ? 1 : 0 ) +
                                ( saveGameXML->freePenitentiary() ? 1 : 0 ) +
                                ( saveGameXML->freeSafari() ? 1 : 0 ) +
                                ( saveGameXML->freeBlacktooth() ? 1 : 0 );
        }
        catch ( const xml_schema::exception& e )
        {
                std::cout << "CreateListOfSavedGames::readSomeInfoFromGamefile got " << e << std::endl ;
        }
}

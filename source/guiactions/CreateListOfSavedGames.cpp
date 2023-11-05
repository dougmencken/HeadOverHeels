
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

#include "ospaths.hpp"

#include <fstream>

#include <tinyxml2.h>


namespace gui
{

void CreateListOfSavedGames::doAction ()
{
        Screen& screen = * GuiManager::getInstance().findOrCreateScreenForAction( this );
        if ( screen.countWidgets() > 0 )
        {
                screen.freeWidgets() ;
        }

        if ( isLoadMenu() )
        {
                // return to main menu
                screen.setEscapeAction( new CreateMainMenu() );
        }
        else
        {
                // return to play
                screen.setEscapeAction( new ContinueGame( true ) );
        }

        screen.placeHeadAndHeels( /* icons */ true, /* copyrights */ false );

        LanguageManager* languageManager = GuiManager::getInstance().getLanguageManager();

        // list of games
        Menu* menu = new Menu( );
        menu->setVerticalOffset( 112 );
        for ( unsigned int slot = 1 ; slot <= howManySaves ; ++ slot )
        {
                std::string file = ospaths::homePath() + "savegame" + ospaths::pathSeparator() + "saved." + util::number2string( slot ) ;
                SavedGameInfo gameInfo = readSomeInfoFromTheSavedGame( file );

                if ( gameInfo.howManyRoomsVisited () >= 2 ) // less than 2 rooms means "couldn't read"
                {
                        std::ostringstream ss;
                        ss << gameInfo.howManyRoomsVisited () << " " << languageManager->findLanguageStringForAlias( "rooms" )->getText() << " "
                                << gameInfo.howManyPlanetsLiberated () << " " << languageManager->findLanguageStringForAlias( "planets" )->getText() ;
                        Label* label = new Label( ss.str() );

                        if ( isLoadMenu() )
                                label->setAction( new LoadGame( slot ) );
                        else
                                label->setAction( new SaveGame( slot ) );
                                // very funny to change to LoadGame here, just try it

                        menu->addOption( label );
                }
                else
                {
                # if defined( DEBUG ) && DEBUG
                        std::cout << "slot \"" << file << "\" is free" << std::endl ;
                # endif

                        std::ostringstream ss;
                        ss << languageManager->findLanguageStringForAlias( "free-slot" )->getText();
                        Label* freeLine = new Label( ss.str() );
                        if ( isLoadMenu() )
                        {
                                freeLine->changeColor( "cyan" );
                                freeLine->setAction( new PlaySound( activities::Activity::Mistake ) );
                        }
                        else
                        {
                                freeLine->changeColor( "orange" );
                                freeLine->setAction( new SaveGame( slot ) );
                        }

                        menu->addOption( freeLine );
                }
        }

        screen.addWidget( menu );
        screen.setKeyHandler( menu );

        GuiManager::getInstance().changeScreen( screen, true );
}

SavedGameInfo CreateListOfSavedGames::readSomeInfoFromTheSavedGame( const std::string & fileName )
{
        std::ifstream in( fileName.c_str() );
        if ( ! in.good() ) // there's no such file
                return SavedGameInfo( fileName, 0, 0 );

        tinyxml2::XMLDocument savedGameXml ;
        tinyxml2::XMLError result = savedGameXml.LoadFile( fileName.c_str () );
        if ( result != tinyxml2::XML_SUCCESS )
        {
                std::cerr << "can’t parse file \"" << fileName << "\" with a saved game" << std::endl ;
                return SavedGameInfo( fileName, 0, 0 );
        }

        tinyxml2::XMLElement* root = savedGameXml.FirstChildElement( "savegame" );

        const char * version = root->Attribute( "version" );
        std::string versionOfSave( "unknown" );
        if ( version != nilPointer ) versionOfSave = version ;
        std::cout << "the saved game file \"" << fileName << "\" has version \"" << versionOfSave << "\"" << std::endl ;

        tinyxml2::XMLElement* exploredRooms = root->FirstChildElement( "exploredRooms" ) ;

        unsigned short howManyRoomsExplored = 0;

        if ( exploredRooms != nilPointer )
        {
                for ( tinyxml2::XMLElement* visited = exploredRooms->FirstChildElement( "visited" ) ;
                                visited != nilPointer ;
                                visited = visited->NextSiblingElement( "visited" ) )
                {
                        howManyRoomsExplored ++;
                }
        }

        unsigned short howManyPlanetsLiberated = 0;

        tinyxml2::XMLElement* freeByblos = root->FirstChildElement( "freeByblos" ) ;
        if ( freeByblos != nilPointer )
                if ( std::string( freeByblos->FirstChild()->ToText()->Value() ) == "true" )
                        howManyPlanetsLiberated ++;

        tinyxml2::XMLElement* freeEgyptus = root->FirstChildElement( "freeEgyptus" ) ;
        if ( freeEgyptus != nilPointer )
                if ( std::string( freeEgyptus->FirstChild()->ToText()->Value() ) == "true" )
                        howManyPlanetsLiberated ++;

        tinyxml2::XMLElement* freePenitentiary = root->FirstChildElement( "freePenitentiary" ) ;
        if ( freePenitentiary != nilPointer )
                if ( std::string( freePenitentiary->FirstChild()->ToText()->Value() ) == "true" )
                        howManyPlanetsLiberated ++;

        tinyxml2::XMLElement* freeSafari = root->FirstChildElement( "freeSafari" ) ;
        if ( freeSafari != nilPointer )
                if ( std::string( freeSafari->FirstChild()->ToText()->Value() ) == "true" )
                        howManyPlanetsLiberated ++;

        tinyxml2::XMLElement* freeBlacktooth = root->FirstChildElement( "freeBlacktooth" ) ;
        if ( freeBlacktooth != nilPointer )
                if ( std::string( freeBlacktooth->FirstChild()->ToText()->Value() ) == "true" )
                        howManyPlanetsLiberated ++;

        return SavedGameInfo( fileName, howManyRoomsExplored, howManyPlanetsLiberated );
}

}


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

using gui::CreateListOfSavedGames;
using gui::ContinueGame;


CreateListOfSavedGames::CreateListOfSavedGames( bool isLoadMenu )
        : Action( )
        , isMenuForLoad( isLoadMenu )
{

}

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

                bool fileExists = false;
                std::ifstream in( file.c_str() );
                if ( in.good() ) fileExists = true;

                if ( fileExists )
                {
                        unsigned short rooms = 0;
                        unsigned short planets = 0;
                        readSomeInfoFromGamefile( file, &rooms, &planets );

                        std::ostringstream ss;
                        ss << rooms << " " << languageManager->findLanguageStringForAlias( "rooms" )->getText() << " "
                                << planets << " " << languageManager->findLanguageStringForAlias( "planets" )->getText();
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
                        std::cout << "slot \"" << file << "\" is free" << std::endl ;

                        std::ostringstream ss;
                        ss << languageManager->findLanguageStringForAlias( "free-slot" )->getText();
                        Label* labelOfFree = new Label( ss.str() );
                        if ( isLoadMenu() )
                        {
                                labelOfFree->changeColor( "cyan" );
                                labelOfFree->setAction( new PlaySound( iso::Activity::Mistake ) );
                        }
                        else
                        {
                                labelOfFree->changeColor( "orange" );
                                labelOfFree->setAction( new SaveGame( slot ) );
                        }

                        menu->addOption( labelOfFree );
                }
        }

        screen.addWidget( menu );
        screen.setKeyHandler( menu );

        GuiManager::getInstance().changeScreen( screen, true );
}

bool CreateListOfSavedGames::readSomeInfoFromGamefile( const std::string& fileName,
                                                       unsigned short* visitedRooms, unsigned short* freePlanets )
{
        tinyxml2::XMLDocument saveXml;
        tinyxml2::XMLError result = saveXml.LoadFile( fileName.c_str () );
        if ( result != tinyxml2::XML_SUCCESS )
        {
                std::cerr << "can’t load file \"" << fileName << "\" with saved game" << std::endl ;
                return false;
        }

        tinyxml2::XMLElement* root = saveXml.FirstChildElement( "savegame" );
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

        *visitedRooms = howManyRoomsExplored;

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

        *freePlanets = howManyPlanetsLiberated;

        return true;
}

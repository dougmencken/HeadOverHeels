
#include "CreateListOfSavedGames.hpp"

#include "GuiManager.hpp"
#include "LanguageStrings.hpp"
#include "Font.hpp"
#include "Slide.hpp"
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

void CreateListOfSavedGames::act ()
{
        Slide & savedGamesSlide = GuiManager::getInstance().findOrCreateSlideForAction( getNameOfAction() );

        if ( ! savedGamesSlide.isNewAndEmpty () ) savedGamesSlide.removeAllWidgets() ;

        savedGamesSlide.setEscapeAction( isLoadMenu() ? static_cast< Action * >( /* to the main menu */ new CreateMainMenu() )
                                                      : static_cast< Action * >( /* back to the game */ new ContinueGame( true ) ) );

        savedGamesSlide.placeHeadAndHeels( /* icons */ true, /* copyrights */ false );

        LanguageStrings & languageStrings = GuiManager::getInstance().getOrMakeLanguageStrings() ;

        menu.deleteAllOptions() ;

        // fill the menu
        for ( unsigned int slot = 1 ; slot <= howManySaves ; ++ slot )
        {
                std::string file = ospaths::pathToFile( ospaths::homePath() + "savegame", "saved." + util::number2string( slot ) );
                SavedGameInfo gameInfo = readSomeInfoFromASavedGame( file );

                if ( gameInfo.howManyRoomsVisited () >= 2 )
                {
                        Label* savedGame = menu.addOptionWithText(
                                util::number2string( gameInfo.howManyRoomsVisited () ) + " "
                                        + languageStrings.getTranslatedTextByAlias( "rooms" ).getText()
                                + " "
                                + util::number2string( gameInfo.howManyPlanetsLiberated () ) + " "
                                        + languageStrings.getTranslatedTextByAlias( "planets" ).getText()
                        );

                        if ( isLoadMenu() )
                                savedGame->setAction( new LoadGame( slot ) );
                        else
                                savedGame->setAction( new SaveGame( slot ) ); // very funny to change to LoadGame here, just try it
                }
                else // less than 2 visited rooms means "couldn't read"
                {
                # if defined( DEBUG ) && DEBUG
                        std::cout << "slot \"" << file << "\" is free" << std::endl ;
                # endif

                        Label* freeSlot = menu.addOptionByLanguageTextAlias( "free-slot" );

                        if ( isLoadMenu() ) {
                                freeSlot->getFontToChange().setColor( "cyan" );
                                freeSlot->setAction( new PlaySound( "mistake" ) );
                        }
                        else {
                                freeSlot->getFontToChange().setColor( "orange" );
                                freeSlot->setAction( new SaveGame( slot ) );
                        }
                }
        }

        savedGamesSlide.addWidget( & menu );
        savedGamesSlide.setKeyHandler( & menu );

        GuiManager::getInstance().changeSlide( getNameOfAction(), true );
}

SavedGameInfo CreateListOfSavedGames::readSomeInfoFromASavedGame( const std::string & fileName )
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

        unsigned short howManyPlanetsLiberated = 0 ;

        std::vector< std::string > planets ;
        planets.push_back( "Byblos" );
        planets.push_back( "Egyptus" );
        planets.push_back( "Penitentiary" );
        planets.push_back( "Safari" );
        planets.push_back( "Blacktooth" );

        for ( unsigned int i = 0 ; i < planets.size () ; ++ i ) {
                tinyxml2::XMLElement* planet = root->FirstChildElement( planets[ i ].c_str () ) ;
                if ( planet != nilPointer )
                        if ( std::string( planet->Attribute( "free" ) ) == "yes" )
                                howManyPlanetsLiberated ++ ;
        }

        return SavedGameInfo( fileName, howManyRoomsExplored, howManyPlanetsLiberated );
}

}

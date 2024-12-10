
#include "TheListOfSavedGames.hpp"

#include "Label.hpp"
#include "GuiManager.hpp"
#include "LanguageStrings.hpp"
#include "LoadGame.hpp"
#include "SaveGame.hpp"
#include "PlaySound.hpp"

#include "ospaths.hpp"

#include <fstream>
#include <sstream>

#include <tinyxml2.h>


namespace gui
{

TheListOfSavedGames::TheListOfSavedGames( bool forLoading )
        : Menu( )
        , isListForLoading( forLoading )
{
        // fill the list
        for ( unsigned int slot = 1 ; slot <= TheListOfSavedGames::how_many_slots ; ++ slot )
                addOptionByLanguageTextAlias( "free-slot" ) ;

        updateTheList() ;
}

void TheListOfSavedGames::updateTheList ()
{
        LanguageStrings & languageStrings = GuiManager::getInstance().getOrMakeLanguageStrings() ;

        const std::vector< Label* > & slotLabels = getEveryOption ();
        for ( unsigned int slot = 1 ; slot <= slotLabels.size() ; ++ slot )
        {
                Label * savedGame = slotLabels[ slot - 1 ] ;
                if ( savedGame == nilPointer ) continue ;

                const std::string & pathToSavedGameSlot = ospaths::pathToFile( ospaths::homePath() + "savegame", "saved." + util::number2string( slot ) );
                const SavedGameInfo & gameInfo = readSomeInfoFromASavedGame( pathToSavedGameSlot );
                bool savedGameIsThere = ( gameInfo.howManyRoomsVisited () >= 2 ) ; // less than 2 visited rooms means “couldn’t read”

                if ( savedGameIsThere ) {
                        savedGame->getFontToChange().setColor( "" ) ;

                        std::ostringstream infoString ;
                        infoString << std::dec << gameInfo.howManyRoomsVisited () << " "
                                        << languageStrings.getTranslatedTextByAlias( "rooms" ).getText()
                                        << " "
                                        << std::dec << gameInfo.howManyPlanetsLiberated () << " "
                                        << languageStrings.getTranslatedTextByAlias( "planets" ).getText() ;

                        savedGame->setText( infoString.str() );
                }
                else {
                        savedGame->setText( languageStrings.getTranslatedTextByAlias( "free-slot" ).getText() );
                        savedGame->getFontToChange().setColor( isForLoading() ? "cyan" : "orange" );
                }

                if ( isForLoading() )
                        savedGame->setAction( savedGameIsThere ? static_cast< Action * >( new LoadGame( slot ) )
                                                               : static_cast< Action * >( new PlaySound( "mistake" ) ) ) ;
                else
                        savedGame->setAction( new SaveGame( slot ) ); // very funny to change to LoadGame here, just try it
        }
}

/* static */
SavedGameInfo TheListOfSavedGames::readSomeInfoFromASavedGame( const std::string & fileName )
{
        std::ifstream in( fileName.c_str() );
        if ( ! in.good() ) return SavedGameInfo( fileName, 0, 0 ); // there’s no such file
        in.close() ;

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
                        howManyRoomsExplored ++ ;
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

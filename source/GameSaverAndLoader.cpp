
#include "GameSaverAndLoader.hpp"

#include "GameManager.hpp"
#include "GameInfo.hpp"
#include "MapManager.hpp"
#include "BonusManager.hpp"
#include "Mediator.hpp"
#include "AvatarItem.hpp"
#include "Room.hpp"

#include <algorithm> // std::find


GameSaverAndLoader::GameSaverAndLoader( )
        : fishRoom( std::string() )
        , nameOfCharacterWhoCaughtTheFish( "in~reincarnation" )
        , xFish( 0 )
        , yFish( 0 )
        , zFish( 0 )
        , orientationOFish( "nowhere" )
{

}

void GameSaverAndLoader::ateFish( const std::string & room, const std::string & character, int x, int y, int z, const std::string & orientation )
{
        this->fishRoom = room ;
        this->nameOfCharacterWhoCaughtTheFish = character ;
        this->xFish = x ;
        this->yFish = y ;
        this->zFish = z ;
        this->orientationOFish = orientation ;
}

bool GameSaverAndLoader::loadGame( const std::string & file )
{
        std::cout << "load game \"" << file << "\"" << std::endl ;

        tinyxml2::XMLDocument saveXml;
        tinyxml2::XMLError result = saveXml.LoadFile( file.c_str () );
        if ( result != tinyxml2::XML_SUCCESS ) {
                std::cerr << "can’t load file \"" << file << "\" with saved game" << std::endl ;
                return false ;
        }

        GameManager & gameManager = GameManager::getInstance () ;

        tinyxml2::XMLElement* root = saveXml.FirstChildElement( "savegame" );

        const char * version = root->Attribute( "version" );
        if ( version == nilPointer ) {
                std::cout << "the saved game file \"" << file << "\" has no \'version\' attribute"
                                << ", hence its version is very old" << std::endl ;
                return false ;
        }

        std::string versionOfSave( version );
        std::cout << "the version of the saved game file \"" << file << "\" is " << versionOfSave ;
        if ( versionOfSave == GameSaverAndLoader::Current_Save_Version () ) {
                std::cout << " (current)" << std::endl ;
        } else {
                const std::vector< std::string > & parsableVersions = GameSaverAndLoader::Parsable_Save_Versions () ;
                bool isVersionParsable = ( std::find( parsableVersions.begin(), parsableVersions.end(), versionOfSave ) != parsableVersions.end() );
                if ( isVersionParsable )
                        std::cout << " (not current but parsable)" << std::endl ;
                else {
                        std::cout << ", which can't be read by the current version of game" << std::endl ;
                        return false ;
                }
        }

        // visited rooms

        tinyxml2::XMLElement* exploredRooms = root->FirstChildElement( "exploredRooms" ) ;

        if ( exploredRooms != nilPointer )
        {
                std::vector< std::string > visitedRooms;

                for ( tinyxml2::XMLElement* visited = exploredRooms->FirstChildElement( "visited" ) ;
                                visited != nilPointer ;
                                visited = visited->NextSiblingElement( "visited" ) )
                {
                        visitedRooms.push_back( visited->Attribute( "room" ) );
                }

                MapManager::getInstance().parseVisitedRooms( visitedRooms );
        }

        // liberated planets

        tinyxml2::XMLElement* byblos = root->FirstChildElement( "Byblos" ) ;
        if ( byblos != nilPointer )
                if ( std::string( byblos->Attribute( "free" ) ) == "yes" )
                        gameManager.liberatePlanet( "byblos", true );

        tinyxml2::XMLElement* egyptus = root->FirstChildElement( "Egyptus" ) ;
        if ( egyptus != nilPointer )
                if ( std::string( egyptus->Attribute( "free" ) ) == "yes" )
                        gameManager.liberatePlanet( "egyptus", true );

        tinyxml2::XMLElement* penitentiary = root->FirstChildElement( "Penitentiary" ) ;
        if ( penitentiary != nilPointer )
                if ( std::string( penitentiary->Attribute( "free" ) ) == "yes" )
                        gameManager.liberatePlanet( "penitentiary", true );

        tinyxml2::XMLElement* safari = root->FirstChildElement( "Safari" ) ;
        if ( safari != nilPointer )
                if ( std::string( safari->Attribute( "free" ) ) == "yes" )
                        gameManager.liberatePlanet( "safari", true );

        tinyxml2::XMLElement* blacktooth = root->FirstChildElement( "Blacktooth" ) ;
        if ( blacktooth != nilPointer )
                if ( std::string( blacktooth->Attribute( "free" ) ) == "yes" )
                        gameManager.liberatePlanet( "blacktooth", true );

        // already taken bonuses

        BonusManager::getInstance().clearAbsentBonuses () ; // forget any previous list

        tinyxml2::XMLElement* takenBonuses = root->FirstChildElement( "takenBonuses" ) ;

        if ( takenBonuses != nilPointer )
        {
                for ( tinyxml2::XMLElement* bonus = takenBonuses->FirstChildElement( "bonus" ) ;
                                bonus != nilPointer ;
                                bonus = bonus->NextSiblingElement( "bonus" ) )
                {
                        if ( bonus->Attribute( "of" ) != nilPointer && bonus->Attribute( "in" ) != nilPointer )
                        {
                                std::string kind = bonus->Attribute( "of" );
                                std::string room = bonus->Attribute( "in" );

                                BonusManager::getInstance().markAsAbsent( BonusInRoom( /* which bonus */ kind, /* in which room */ room ) );

                                std::cout << "rebuilding room \"" << room << "\" without bonus \"" << kind << "\"" << std::endl ;
                                MapManager::getInstance().rebuildRoom( MapManager::getInstance().findRoomByFile( room ) );
                        }
                }
        }

        // characters

        continueSavedGame( root->FirstChildElement( "starring" ) );

        return true;
}

/* private */
void GameSaverAndLoader::continueSavedGame ( tinyxml2::XMLElement* characters )
{
        GameManager & gameManager = GameManager::getInstance () ;

        if ( characters != nilPointer )
        {
                gameManager.getIsomot().prepare () ;

                GameInfo & gameInfo = gameManager.getGameInfo () ;

                for ( tinyxml2::XMLElement* character = characters->FirstChildElement( "character" ) ;
                                character != nilPointer ;
                                character = character->NextSiblingElement( "character" ) )
                {
                        const char * name = character->Attribute( "name" );

                        bool isActiveCharacter = false ;
                        tinyxml2::XMLElement* active = character->FirstChildElement( "active" );
                        if ( active != nilPointer && std::string( active->FirstChild()->ToText()->Value() ) == "true" )
                                isActiveCharacter = true;

                        std::string room = "no room" ;
                        tinyxml2::XMLElement* roomFile = character->FirstChildElement( "room" );
                        if ( roomFile != nilPointer )
                                room = roomFile->FirstChild()->ToText()->Value();

                        tinyxml2::XMLElement* xElement = character->FirstChildElement( "x" );
                        tinyxml2::XMLElement* yElement = character->FirstChildElement( "y" );
                        tinyxml2::XMLElement* zElement = character->FirstChildElement( "z" );
                        int x = 0 ;
                        int y = 0 ;
                        int z = 0 ;
                        if ( xElement != nilPointer )
                                x = std::atoi( xElement->FirstChild()->ToText()->Value() );
                        if ( yElement != nilPointer )
                                y = std::atoi( yElement->FirstChild()->ToText()->Value() );
                        if ( zElement != nilPointer )
                                z = std::atoi( zElement->FirstChild()->ToText()->Value() );

                        unsigned int howManyLives = 0 ;
                        tinyxml2::XMLElement* lives = character->FirstChildElement( "lives" );
                        if ( lives != nilPointer )
                                howManyLives = std::atoi( lives->FirstChild()->ToText()->Value() );

                        std::string orientationString = "nowhere" ;
                        tinyxml2::XMLElement* orientation = character->FirstChildElement( "orientation" );
                        if ( orientation != nilPointer )
                                orientationString = orientation->FirstChild()->ToText()->Value() ;

                        std::string entryString = "just wait" ;
                        tinyxml2::XMLElement* entry = character->FirstChildElement( "entry" );
                        if ( entry != nilPointer )
                                entryString = entry->FirstChild()->ToText()->Value() ;

                        bool hasHorn = false ;
                        tinyxml2::XMLElement* horn = character->FirstChildElement( "hasHorn" );
                        if ( horn != nilPointer && std::string( horn->FirstChild()->ToText()->Value() ) == "yes" )
                                hasHorn = true;

                        bool hasHandbag = false ;
                        tinyxml2::XMLElement* handbag = character->FirstChildElement( "hasHandbag" );
                        if ( handbag != nilPointer && std::string( handbag->FirstChild()->ToText()->Value() ) == "yes" )
                                hasHandbag = true;

                        unsigned int howManyDoughnuts = 0 ;
                        tinyxml2::XMLElement* donuts = character->FirstChildElement( "donuts" );
                        if ( donuts != nilPointer )
                                howManyDoughnuts = std::atoi( donuts->FirstChild()->ToText()->Value() );

                        std::string characterName( name );

                        if ( characterName == "headoverheels" )
                        {
                                // formula for lives of the composite character from the lives of two simple characters is
                                // lives H & H = 100 * lives Head + lives Heels

                                unsigned char headLives = 0 ;
                                unsigned char heelsLives = 0 ;

                                while ( howManyLives > 100 )
                                {
                                        howManyLives -= 100;
                                        headLives++;
                                }

                                heelsLives = static_cast< unsigned char >( howManyLives );

                                gameInfo.setHeadLives( headLives );
                                gameInfo.setHeelsLives( heelsLives );
                                gameInfo.setHorn( hasHorn );
                                gameInfo.setDoughnuts( howManyDoughnuts );
                                gameInfo.setHandbag( hasHandbag );
                                gameInfo.setBonusQuickSteps( 0 );
                                gameInfo.setBonusHighJumps( 0 );
                                gameInfo.setHeadShieldPoints( 0 );
                                gameInfo.setHeelsShieldPoints( 0 );
                        }
                        else if ( characterName == "head" )
                        {
                                gameInfo.setHeadLives( howManyLives );
                                gameInfo.setHorn( hasHorn );
                                gameInfo.setDoughnuts( howManyDoughnuts );
                                gameInfo.setBonusQuickSteps( 0 );
                                gameInfo.setHeadShieldPoints( 0 );
                        }
                        else if ( characterName == "heels" )
                        {
                                gameInfo.setHeelsLives( howManyLives );
                                gameInfo.setHandbag( hasHandbag );
                                gameInfo.setBonusHighJumps( 0 );
                                gameInfo.setHeelsShieldPoints( 0 );
                        }

                        std::cout << "continue the previous game" << std::endl ;
                        // no begin.ogg here

                        MapManager::getInstance().beginOldGameWithCharacter(
                                        room, characterName, x, y, z, orientationString, entryString, isActiveCharacter ) ;
                }
        } else
                std::cerr << "can't continue the game without characters" << std::endl ;
}

bool GameSaverAndLoader::saveGame( const std::string& file )
{
        std::cout << "save game \"" << file << "\"" << std::endl ;

        GameManager & gameManager = GameManager::getInstance () ;
        GameInfo & gameInfo = gameManager.getGameInfo () ;

        tinyxml2::XMLDocument saveXml;

        tinyxml2::XMLElement * root = saveXml.NewElement( "savegame" );
        root->SetAttribute( "version", GameSaverAndLoader::Current_Save_Version ().c_str () );
        saveXml.InsertFirstChild( root );

        // rooms visited

        const std::set< std::string > & visitedRooms = MapManager::getInstance().listAllVisitedRooms() ;
        if ( visitedRooms.size () > 0 )
        {
                tinyxml2::XMLElement* exploredRooms = saveXml.NewElement( "exploredRooms" );

                for ( std::set< std::string >::iterator i = visitedRooms.begin () ; i != visitedRooms.end () ; ++ i )
                {
                        tinyxml2::XMLElement* visited = saveXml.NewElement( "visited" );
                        visited->SetAttribute( "room", ( *i ).c_str () );
                        exploredRooms->InsertEndChild( visited );
                }

                root->InsertEndChild( exploredRooms );
        }

        // liberated planets

        bool isByblosFree = gameManager.isFreePlanet( "byblos" ) ;
        if ( isByblosFree ) {
                tinyxml2::XMLElement* freeByblos = saveXml.NewElement( "Byblos" ) ;
                freeByblos->SetAttribute( "free", "yes" );
                root->InsertEndChild( freeByblos );
        }

        bool isEgyptusFree = gameManager.isFreePlanet( "egyptus" ) ;
        if ( isEgyptusFree ) {
                tinyxml2::XMLElement* freeEgyptus = saveXml.NewElement( "Egyptus" ) ;
                freeEgyptus->SetAttribute( "free", "yes" );
                root->InsertEndChild( freeEgyptus );
        }

        bool isPenitentiaryFree = gameManager.isFreePlanet( "penitentiary" ) ;
        if ( isPenitentiaryFree ) {
                tinyxml2::XMLElement* freePenitentiary = saveXml.NewElement( "Penitentiary" ) ;
                freePenitentiary->SetAttribute( "free", "yes" );
                root->InsertEndChild( freePenitentiary );
        }

        bool isSafariFree = gameManager.isFreePlanet( "safari" ) ;
        if ( isSafariFree ) {
                tinyxml2::XMLElement* freeSafari = saveXml.NewElement( "Safari" ) ;
                freeSafari->SetAttribute( "free", "yes" );
                root->InsertEndChild( freeSafari );
        }

        bool isBlacktoothFree = gameManager.isFreePlanet( "blacktooth" ) ;
        if ( isBlacktoothFree ) {
                tinyxml2::XMLElement* freeBlacktooth = saveXml.NewElement( "Blacktooth" ) ;
                freeBlacktooth->SetAttribute( "free", "yes" );
                root->InsertEndChild( freeBlacktooth );
        }

        // taken thus absent bonuses

        const std::vector < BonusInRoom > & takenBonuses = BonusManager::getInstance().getAllTakenBonuses () ;

        if ( takenBonuses.size () > 0 )
        {
                tinyxml2::XMLElement* takenBonusesXml = saveXml.NewElement( "takenBonuses" ) ;

                for ( unsigned int b = 0 ; b < takenBonuses.size () ; ++ b )
                {
                        tinyxml2::XMLElement* bonus = saveXml.NewElement( "bonus" );
                        bonus->SetAttribute( "of", takenBonuses[ b ].getBonus ().c_str() );
                        bonus->SetAttribute( "in", takenBonuses[ b ].getRoom ().c_str() );
                        takenBonusesXml->InsertEndChild( bonus );
                }

                root->InsertEndChild( takenBonusesXml );
        }

        // characters

        tinyxml2::XMLElement* characters = saveXml.NewElement( "starring" ) ;

        // active character
        {
                std::string whoPlaysYet = nameOfCharacterWhoCaughtTheFish;

                tinyxml2::XMLElement* activeCharacter = saveXml.NewElement( "character" );
                activeCharacter->SetAttribute( "name", whoPlaysYet.c_str () );

                tinyxml2::XMLElement* active = saveXml.NewElement( "active" );
                active->SetText( "true" );
                activeCharacter->InsertEndChild( active );

                tinyxml2::XMLElement* roomFile = saveXml.NewElement( "room" );
                roomFile->SetText( this->fishRoom.c_str() );
                activeCharacter->InsertEndChild( roomFile );

                tinyxml2::XMLElement* x = saveXml.NewElement( "x" );
                x->SetText( this->xFish );
                tinyxml2::XMLElement* y = saveXml.NewElement( "y" );
                y->SetText( this->yFish );
                tinyxml2::XMLElement* z = saveXml.NewElement( "z" );
                z->SetText( this->zFish );
                activeCharacter->InsertEndChild( x );
                activeCharacter->InsertEndChild( y );
                activeCharacter->InsertEndChild( z );

                unsigned int howManyLives = 0 ;
                if ( whoPlaysYet == "head" || whoPlaysYet == "heels" )
                {
                        howManyLives = gameInfo.getLivesByName( whoPlaysYet );
                }
                else if ( whoPlaysYet == "headoverheels"  )
                {
                        howManyLives = 100 * gameInfo.getHeadLives() + gameInfo.getHeelsLives() ;
                }

                tinyxml2::XMLElement* lives = saveXml.NewElement( "lives" );
                lives->SetText( howManyLives );
                activeCharacter->InsertEndChild( lives );

                tinyxml2::XMLElement* orientation = saveXml.NewElement( "orientation" );
                orientation->SetText( orientationOFish.c_str () );
                activeCharacter->InsertEndChild( orientation );

                tinyxml2::XMLElement* entry = saveXml.NewElement( "entry" );
                entry->SetText( "just wait" );
                activeCharacter->InsertEndChild( entry );

                if ( whoPlaysYet == "head" || whoPlaysYet == "headoverheels" )
                {
                        bool hasHorn = gameInfo.hasHorn () ;

                        tinyxml2::XMLElement* horn = saveXml.NewElement( "hasHorn" );
                        horn->SetText( hasHorn ? "yes" : "no" );
                        activeCharacter->InsertEndChild( horn );

                        tinyxml2::XMLElement* donuts = saveXml.NewElement( "donuts" );
                        donuts->SetText( gameInfo.getDoughnuts () );
                        activeCharacter->InsertEndChild( donuts );
                }

                if ( whoPlaysYet == "heels" || whoPlaysYet == "headoverheels" )
                {
                        bool hasHandbag = gameInfo.hasHandbag () ;

                        tinyxml2::XMLElement* handbag = saveXml.NewElement( "hasHandbag" );
                        handbag->SetText( hasHandbag ? "yes" : "no" );
                        activeCharacter->InsertEndChild( handbag );
                }

                characters->InsertEndChild( activeCharacter );
        }

        // inactive character, if any

        std::string whoWaitsToPlay = "nobody";

        Room* activeRoom = MapManager::getInstance().getActiveRoom () ;
        Room* secondRoom = MapManager::getInstance().getRoomOfInactiveCharacter () ;
        // there may be no more rooms when there’re no more characters
        // or when other character is in the same room as active character

        if ( secondRoom != nilPointer )
        {
                whoWaitsToPlay = secondRoom->getMediator()->getNameOfActiveCharacter() ;
        }
        else
        if ( activeRoom->getMediator()->getWaitingCharacter() != nilPointer )
        {
                whoWaitsToPlay = activeRoom->getMediator()->getWaitingCharacter()->getKind () ;
        }

        AvatarItemPtr characterToo ;

        if ( whoWaitsToPlay != "nobody" )
        {
                Room* roomWithWaitingGuy = ( secondRoom != nilPointer ) ? secondRoom : activeRoom ;

                std::vector< AvatarItemPtr > charactersOnEntry = roomWithWaitingGuy->getCharactersWhoEnteredRoom () ;

                for ( std::vector< AvatarItemPtr >::const_iterator p = charactersOnEntry.begin (); p != charactersOnEntry.end (); ++ p )
                {
                        if ( ( *p )->getKind () == whoWaitsToPlay )
                        {
                                characterToo = *p ;
                                break;
                        }
                }

                if ( characterToo != nilPointer )
                {
                        tinyxml2::XMLElement* inactiveCharacter = saveXml.NewElement( "character" );
                        inactiveCharacter->SetAttribute( "name", whoWaitsToPlay.c_str () );

                        tinyxml2::XMLElement* roomFile = saveXml.NewElement( "room" );
                        roomFile->SetText( roomWithWaitingGuy->getNameOfRoomDescriptionFile().c_str() );
                        inactiveCharacter->InsertEndChild( roomFile );

                        tinyxml2::XMLElement* x = saveXml.NewElement( "x" );
                        x->SetText( characterToo->getX() );
                        tinyxml2::XMLElement* y = saveXml.NewElement( "y" );
                        y->SetText( characterToo->getY() );
                        tinyxml2::XMLElement* z = saveXml.NewElement( "z" );
                        z->SetText( characterToo->getZ() );
                        inactiveCharacter->InsertEndChild( x );
                        inactiveCharacter->InsertEndChild( y );
                        inactiveCharacter->InsertEndChild( z );

                        tinyxml2::XMLElement* lives = saveXml.NewElement( "lives" );
                        lives->SetText( characterToo->getLives() );
                        inactiveCharacter->InsertEndChild( lives );

                        tinyxml2::XMLElement* orientation = saveXml.NewElement( "orientation" );
                        orientation->SetText( characterToo->getOrientation().c_str () );
                        inactiveCharacter->InsertEndChild( orientation );

                        tinyxml2::XMLElement* entry = saveXml.NewElement( "entry" );
                        entry->SetText( characterToo->getWayOfEntry().c_str () );
                        inactiveCharacter->InsertEndChild( entry );

                        if ( whoWaitsToPlay == "head" )
                        {
                                bool hasHorn = gameInfo.hasHorn () ;

                                tinyxml2::XMLElement* horn = saveXml.NewElement( "hasHorn" );
                                horn->SetText( hasHorn ? "yes" : "no" );
                                inactiveCharacter->InsertEndChild( horn );

                                tinyxml2::XMLElement* donuts = saveXml.NewElement( "donuts" );
                                donuts->SetText( gameInfo.getDoughnuts () );
                                inactiveCharacter->InsertEndChild( donuts );
                        }

                        if ( whoWaitsToPlay == "heels" )
                        {
                                bool hasHandbag = gameInfo.hasHandbag () ;

                                tinyxml2::XMLElement* handbag = saveXml.NewElement( "hasHandbag" );
                                handbag->SetText( hasHandbag ? "yes" : "no" );
                                inactiveCharacter->InsertEndChild( handbag );
                        }

                        characters->InsertEndChild( inactiveCharacter );
                }
        }

        root->InsertEndChild( characters );

        tinyxml2::XMLError result = saveXml.SaveFile( file.c_str () );
        if ( result != tinyxml2::XML_SUCCESS )
        {
                std::cerr << "can’t save game as \"" << file << "\"" << std::endl ;
                return false;
        }

        return true;
}

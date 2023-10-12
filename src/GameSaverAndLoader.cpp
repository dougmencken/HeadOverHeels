
#include "GameSaverAndLoader.hpp"

#include "GameManager.hpp"
#include "GameInfo.hpp"
#include "Isomot.hpp"
#include "MapManager.hpp"
#include "BonusManager.hpp"
#include "Mediator.hpp"
#include "PlayerItem.hpp"
#include "Room.hpp"

#include <algorithm>


namespace game
{

GameSaverAndLoader::GameSaverAndLoader( )
        : fishRoom( std::string() )
        , nameOfCharacterWhoCaughtTheFish( "in~reincarnation" )
        , xFish( 0 )
        , yFish( 0 )
        , zFish( 0 )
        , catchFishWay( "nowhere" )
{

}

void GameSaverAndLoader::ateFish( const std::string& room, const std::string& name, int x, int y, int z, const std::string& orientation )
{
        this->fishRoom = room;
        this->nameOfCharacterWhoCaughtTheFish = name;
        this->xFish = x;
        this->yFish = y;
        this->zFish = z;
        catchFishWay = orientation ;
}

bool GameSaverAndLoader::loadGame( const std::string& file )
{
        std::cout << "load game \"" << file << "\"" << std::endl ;

        tinyxml2::XMLDocument saveXml;
        tinyxml2::XMLError result = saveXml.LoadFile( file.c_str () );
        if ( result != tinyxml2::XML_SUCCESS )
        {
                std::cerr << "can’t load file \"" << file << "\" with saved game" << std::endl ;
                return false;
        }

        GameManager & gameManager = game::GameManager::getInstance () ;

        tinyxml2::XMLElement* root = saveXml.FirstChildElement( "savegame" );

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

                gameManager.getIsomot().getMapManager().parseVisitedRooms( visitedRooms );
        }

        // liberated planets

        tinyxml2::XMLElement* freeByblos = root->FirstChildElement( "freeByblos" ) ;
        if ( freeByblos != nilPointer )
                if ( std::string( freeByblos->FirstChild()->ToText()->Value() ) == "true" )
                        gameManager.liberatePlanet( "byblos", false );

        tinyxml2::XMLElement* freeEgyptus = root->FirstChildElement( "freeEgyptus" ) ;
        if ( freeEgyptus != nilPointer )
                if ( std::string( freeEgyptus->FirstChild()->ToText()->Value() ) == "true" )
                        gameManager.liberatePlanet( "egyptus", false );

        tinyxml2::XMLElement* freePenitentiary = root->FirstChildElement( "freePenitentiary" ) ;
        if ( freePenitentiary != nilPointer )
                if ( std::string( freePenitentiary->FirstChild()->ToText()->Value() ) == "true" )
                        gameManager.liberatePlanet( "penitentiary", false );

        tinyxml2::XMLElement* freeSafari = root->FirstChildElement( "freeSafari" ) ;
        if ( freeSafari != nilPointer )
                if ( std::string( freeSafari->FirstChild()->ToText()->Value() ) == "true" )
                        gameManager.liberatePlanet( "safari", false );

        tinyxml2::XMLElement* freeBlacktooth = root->FirstChildElement( "freeBlacktooth" ) ;
        if ( freeBlacktooth != nilPointer )
                if ( std::string( freeBlacktooth->FirstChild()->ToText()->Value() ) == "true" )
                        gameManager.liberatePlanet( "blacktooth", false );

        // bonuses already taken

        tinyxml2::XMLElement* bonuses = root->FirstChildElement( "bonuses" ) ;

        if ( bonuses != nilPointer )
        {
                for ( tinyxml2::XMLElement* bonus = bonuses->FirstChildElement( "bonus" ) ;
                                bonus != nilPointer ;
                                bonus = bonus->NextSiblingElement( "bonus" ) )
                {
                        if ( bonus->Attribute( "room" ) != nilPointer && bonus->Attribute( "label" ) != nilPointer )
                        {
                                std::string room = bonus->Attribute( "room" );
                                std::string label = bonus->Attribute( "label" );

                                BonusManager::getInstance().markBonusAsAbsent( room, label );

                                std::cout << "rebuilding room \"" << room << "\" without bonus \"" << label << "\"" << std::endl ;
                                gameManager.getIsomot().getMapManager().rebuildRoom( gameManager.getIsomot().getMapManager().findRoomByFile( room ) );
                        }
                }
        }

        // characters

        continueSavedGame( root->FirstChildElement( "players" ) );

        return true;
}

/* private */
void GameSaverAndLoader::continueSavedGame ( tinyxml2::XMLElement* characters )
{
        GameManager & gameManager = GameManager::getInstance () ;
        gameManager.getIsomot().prepare () ;

        if ( characters != nilPointer )
        {
                GameInfo & gameInfo = gameManager.getGameInfo () ;

                for ( tinyxml2::XMLElement* player = characters->FirstChildElement( "player" ) ;
                                player != nilPointer ;
                                player = player->NextSiblingElement( "player" ) )
                {
                        std::string label = player->Attribute( "label" );

                        bool isActiveCharacter = false ;
                        tinyxml2::XMLElement* active = player->FirstChildElement( "active" );
                        if ( active != nilPointer && std::string( active->FirstChild()->ToText()->Value() ) == "true" )
                                isActiveCharacter = true;

                        std::string room = "no room" ;
                        tinyxml2::XMLElement* roomFile = player->FirstChildElement( "room" );
                        if ( roomFile != nilPointer )
                                room = roomFile->FirstChild()->ToText()->Value();

                        tinyxml2::XMLElement* xElement = player->FirstChildElement( "x" );
                        tinyxml2::XMLElement* yElement = player->FirstChildElement( "y" );
                        tinyxml2::XMLElement* zElement = player->FirstChildElement( "z" );
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
                        tinyxml2::XMLElement* lives = player->FirstChildElement( "lives" );
                        if ( lives != nilPointer )
                                howManyLives = std::atoi( lives->FirstChild()->ToText()->Value() );

                        std::string orientationString = "nowhere" ;
                        tinyxml2::XMLElement* orientation = player->FirstChildElement( "orientation" );
                        if ( orientation == nilPointer ) orientation = player->FirstChildElement( "direction" );
                        if ( orientation != nilPointer )
                                orientationString = orientation->FirstChild()->ToText()->Value() ;

                        std::string entryString = "just wait" ;
                        tinyxml2::XMLElement* entry = player->FirstChildElement( "entry" );
                        if ( entry != nilPointer )
                                entryString = entry->FirstChild()->ToText()->Value() ;

                        bool hasHorn = false ;
                        tinyxml2::XMLElement* horn = player->FirstChildElement( "hasHorn" );
                        if ( horn != nilPointer && std::string( horn->FirstChild()->ToText()->Value() ) == "yes" )
                                hasHorn = true;

                        bool hasHandbag = false ;
                        tinyxml2::XMLElement* handbag = player->FirstChildElement( "hasHandbag" );
                        if ( handbag != nilPointer && std::string( handbag->FirstChild()->ToText()->Value() ) == "yes" )
                                hasHandbag = true;

                        unsigned int howManyDoughnuts = 0 ;
                        tinyxml2::XMLElement* donuts = player->FirstChildElement( "donuts" );
                        if ( donuts != nilPointer )
                                howManyDoughnuts = std::atoi( donuts->FirstChild()->ToText()->Value() );

                        if ( label == "headoverheels" )
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
                        else if ( label == "head" )
                        {
                                gameInfo.setHeadLives( howManyLives );
                                gameInfo.setHorn( hasHorn );
                                gameInfo.setDoughnuts( howManyDoughnuts );
                                gameInfo.setBonusQuickSteps( 0 );
                                gameInfo.setHeadShieldPoints( 0 );
                        }
                        else if ( label == "heels" )
                        {
                                gameInfo.setHeelsLives( howManyLives );
                                gameInfo.setHandbag( hasHandbag );
                                gameInfo.setBonusHighJumps( 0 );
                                gameInfo.setHeelsShieldPoints( 0 );
                        }

                        gameManager.getIsomot().getMapManager().beginOldGameWithCharacter(
                                        room, label, x, y, z, orientationString, entryString, isActiveCharacter
                        ) ;
                }
        }

        std::cout << "continue previous game" << std::endl ;
        // no begin.ogg here
}

bool GameSaverAndLoader::saveGame( const std::string& file )
{
        std::cout << "save game \"" << file << "\"" << std::endl ;

        GameManager & gameManager = GameManager::getInstance () ;
        GameInfo & gameInfo = gameManager.getGameInfo () ;

        tinyxml2::XMLDocument saveXml;

        tinyxml2::XMLNode * root = saveXml.NewElement( "savegame" );
        saveXml.InsertFirstChild( root );

        // rooms visited

        const std::set< std::string > & visitedRooms = gameManager.getIsomot().getMapManager().getVisitedRooms() ;
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

        tinyxml2::XMLElement* freeByblos = saveXml.NewElement( "freeByblos" ) ;
        freeByblos->SetText( gameManager.isFreePlanet( "byblos" ) ? "true" : "false" );
        root->InsertEndChild( freeByblos );

        tinyxml2::XMLElement* freeEgyptus = saveXml.NewElement( "freeEgyptus" ) ;
        freeEgyptus->SetText( gameManager.isFreePlanet( "egyptus" ) ? "true" : "false" );
        root->InsertEndChild( freeEgyptus );

        tinyxml2::XMLElement* freePenitentiary = saveXml.NewElement( "freePenitentiary" ) ;
        freePenitentiary->SetText( gameManager.isFreePlanet( "penitentiary" ) ? "true" : "false" );
        root->InsertEndChild( freePenitentiary );

        tinyxml2::XMLElement* freeSafari = saveXml.NewElement( "freeSafari" ) ;
        freeSafari->SetText( gameManager.isFreePlanet( "safari" ) ? "true" : "false" );
        root->InsertEndChild( freeSafari );

        tinyxml2::XMLElement* freeBlacktooth = saveXml.NewElement( "freeBlacktooth" ) ;
        freeBlacktooth->SetText( gameManager.isFreePlanet( "blacktooth" ) ? "true" : "false" );
        root->InsertEndChild( freeBlacktooth );

        // taken bonuses

        std::multimap < std::string /* room */, std::string /* bonus */ > bonusesInRooms;
        BonusManager::getInstance().fillAbsentBonuses( bonusesInRooms );
        if ( bonusesInRooms.size () > 0 )
        {
                tinyxml2::XMLElement* bonuses = saveXml.NewElement( "bonuses" ) ;

                for ( std::multimap< std::string, std::string >::const_iterator b = bonusesInRooms.begin () ; b != bonusesInRooms.end () ; ++ b )
                {
                        tinyxml2::XMLElement* bonus = saveXml.NewElement( "bonus" );
                        bonus->SetAttribute( "room", ( *b ).first.c_str () );
                        bonus->SetAttribute( "label", ( *b ).second.c_str () );
                        bonuses->InsertEndChild( bonus );
                }

                root->InsertEndChild( bonuses );
        }

        // characters

        tinyxml2::XMLElement* players = saveXml.NewElement( "players" ) ;

        // active character
        {
                std::string whoPlaysYet = nameOfCharacterWhoCaughtTheFish;

                tinyxml2::XMLElement* activeCharacter = saveXml.NewElement( "player" );
                activeCharacter->SetAttribute( "label", whoPlaysYet.c_str () );

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
                orientation->SetText( catchFishWay.c_str () );
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

                players->InsertEndChild( activeCharacter );
        }

        // inactive character, if any

        std::string whoWaitsToPlay = "nobody";

        Room* activeRoom = gameManager.getIsomot().getMapManager().getActiveRoom () ;
        Room* secondRoom = gameManager.getIsomot().getMapManager().getRoomOfInactiveCharacter () ;
        // there may be no more rooms when there’re no more characters
        // or when other character is in the same room as active character

        if ( secondRoom != nilPointer )
        {
                whoWaitsToPlay = secondRoom->getMediator()->getLabelOfActiveCharacter() ;
        }
        else
        if ( activeRoom->getMediator()->getWaitingCharacter() != nilPointer )
        {
                whoWaitsToPlay = activeRoom->getMediator()->getWaitingCharacter()->getLabel() ;
        }

        PlayerItemPtr characterToo ;

        if ( whoWaitsToPlay != "nobody" )
        {
                Room* roomWithWaitingGuy = ( secondRoom != nilPointer ) ? secondRoom : activeRoom ;

                std::vector< PlayerItemPtr > charactersOnEntry = roomWithWaitingGuy->getCharactersWhoEnteredRoom () ;

                for ( std::vector< PlayerItemPtr >::const_iterator p = charactersOnEntry.begin (); p != charactersOnEntry.end (); ++ p )
                {
                        if ( ( *p )->getLabel() == whoWaitsToPlay )
                        {
                                characterToo = *p ;
                                break;
                        }
                }

                if ( characterToo != nilPointer )
                {
                        tinyxml2::XMLElement* inactiveCharacter = saveXml.NewElement( "player" );
                        inactiveCharacter->SetAttribute( "label", whoWaitsToPlay.c_str () );

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

                        players->InsertEndChild( inactiveCharacter );
                }
        }

        root->InsertEndChild( players );

        tinyxml2::XMLError result = saveXml.SaveFile( file.c_str () );
        if ( result != tinyxml2::XML_SUCCESS )
        {
                std::cerr << "can’t save game as \"" << file << "\"" << std::endl ;
                return false;
        }

        return true;
}

}


#include "GameFileManager.hpp"
#include "GameManager.hpp"
#include "Isomot.hpp"
#include "MapManager.hpp"
#include "BonusManager.hpp"
#include "Mediator.hpp"
#include "PlayerItem.hpp"
#include "Room.hpp"


namespace isomot
{

GameFileManager::GameFileManager( const GameManager* gameManager, const Isomot* isomot )
        : room( std::string() )
        , nameOfCharacterWhoCaughtTheFish( "in~reincarnation" )
        , x( 0 )
        , y( 0 )
        , z( 0 )
        , direction( NoDirection )
        , gameManager( const_cast< GameManager* >( gameManager ) )
        , isomot( const_cast< Isomot* >( isomot ) )
{

}

GameFileManager::~GameFileManager()
{

}

void GameFileManager::assignFishData( const std::string& room, const std::string& name, int x, int y, int z, const Direction& direction )
{
        this->room = room;
        this->nameOfCharacterWhoCaughtTheFish = name;
        this->x = x;
        this->y = y;
        this->z = z;
        this->direction = direction;
}

void GameFileManager::loadGame( const std::string& fileName )
{
        try
        {
                std::auto_ptr< sgxml::SaveGameXML > saveGameXML( sgxml::savegame( fileName.c_str () ) );

                // visited rooms
                this->isomot->getMapManager()->loadVisitedSequence( saveGameXML->exploredRooms().visited() );

                // bonuses already taken
                BonusManager::getInstance()->load( saveGameXML->bonus().room() );

                if ( saveGameXML->freeByblos() )
                {
                        this->gameManager->liberatePlanet( "byblos", false );
                }
                if ( saveGameXML->freeEgyptus() )
                {
                        this->gameManager->liberatePlanet( "egyptus", false );
                }
                if ( saveGameXML->freePenitentiary() )
                {
                        this->gameManager->liberatePlanet( "penitentiary", false );
                }
                if ( saveGameXML->freeSafari() )
                {
                        this->gameManager->liberatePlanet( "safari", false );
                }
                if ( saveGameXML->freeBlacktooth() )
                {
                        this->gameManager->liberatePlanet( "blacktooth", false );
                }

                this->updateAttributesOfPlayers( saveGameXML->players().player() );

                this->isomot->continueSavedGame( saveGameXML->players().player() );
        }
        catch ( const xml_schema::exception& e )
        {
                std::cout << e << std::endl;
        }
}

void GameFileManager::saveGame( const std::string& fileName )
{
        try
        {
                // Se almacenan las salas visitadas
                sgxml::exploredRooms exploredRooms;
                sgxml::exploredRooms::visited_sequence& sequenceOfRooms( exploredRooms.visited() );
                this->isomot->getMapManager()->saveVisitedSequence( sequenceOfRooms );
                exploredRooms.visited( sequenceOfRooms );

                // Se almacenan los bonus que se hallan cogido
                sgxml::bonus bonus;
                sgxml::bonus::room_sequence& roomSequence( bonus.room() );
                BonusManager::getInstance()->save( roomSequence );
                bonus.room( roomSequence );

                // Se almacena el estado de los jugadores
                sgxml::players players;
                sgxml::players::player_sequence playerSequence( players.player() );

                // active player
                std::string whoPlaysYet = nameOfCharacterWhoCaughtTheFish;

                unsigned short lives = 0;
                if ( ( whoPlaysYet == "head" ) || ( whoPlaysYet == "heels" ) )
                {
                        lives = this->gameManager->getLives( whoPlaysYet );
                }
                else if ( whoPlaysYet == "headoverheels"  )
                {
                        lives = this->gameManager->getLives( "head" ) * 100 + this->gameManager->getLives( "heels" );
                }

                // possession of objects
                std::vector< std::string > tools = this->gameManager->playerTools( whoPlaysYet );

                // store all data
                playerSequence.push_back(
                        sgxml::player
                        (
                                true, // active player
                                this->room,
                                this->x,
                                this->y,
                                this->z,
                                int( this->direction ),
                                JustWait,
                                lives,
                                ( ( whoPlaysYet == "head" ) || ( whoPlaysYet == "headoverheels" ) )
                                        ? std::find( tools.begin (), tools.end (), "horn" ) != tools.end ()
                                        : false,
                                ( ( whoPlaysYet == "heels" ) || ( whoPlaysYet == "headoverheels" ) )
                                        ? std::find( tools.begin (), tools.end (), "handbag" ) != tools.end ()
                                        : false,
                                this->gameManager->getDonuts( whoPlaysYet ),
                                nameOfCharacterWhoCaughtTheFish
                        )
                );

                // there may be no more rooms because there are no more players
                // or because other player is in the same room as active player

                Room* activeRoom = this->isomot->getMapManager()->getActiveRoom();
                Room* secondRoom = this->isomot->getMapManager()->getRoomOfInactivePlayer();

                PlayerItem* inactivePlayer = ( secondRoom != 0 ? secondRoom->getMediator()->getActivePlayer() : activeRoom->getMediator()->getHiddenPlayer() );

                if ( inactivePlayer != 0 )
                {
                        std::string whoWaitsToPlay = inactivePlayer->getLabel();
                        std::vector< std::string > tools = this->gameManager->playerTools( whoWaitsToPlay );
                        PlayerInitialPosition* initialPosition = this->isomot->getMapManager()->findPlayerPosition(
                                secondRoom != 0 ?
                                        secondRoom->getNameOfFileWithDataAboutRoom() :
                                        activeRoom->getNameOfFileWithDataAboutRoom()
                                , whoWaitsToPlay
                        );
                        if ( initialPosition == 0 )
                        {
                                initialPosition = this->isomot->getMapManager()->findPlayerPosition( activeRoom->getNameOfFileWithDataAboutRoom(), "headoverheels" );
                        }
                        playerSequence.push_back(
                                sgxml::player
                                (
                                        false, // inactive player
                                        secondRoom != 0 ? secondRoom->getNameOfFileWithDataAboutRoom() : activeRoom->getNameOfFileWithDataAboutRoom(),
                                        initialPosition->getX(),
                                        initialPosition->getY(),
                                        initialPosition->getZ(),
                                        int( inactivePlayer->getDirection() ),
                                        initialPosition->getEntry(),
                                        this->gameManager->getLives( whoWaitsToPlay ),
                                        whoWaitsToPlay == "head" || whoWaitsToPlay == "headoverheels"
                                                ? std::find( tools.begin(), tools.end(), "horn" ) != tools.end()
                                                : false,
                                        whoWaitsToPlay == "heels" || whoWaitsToPlay == "headoverheels"
                                                ? std::find( tools.begin(), tools.end(), "handbag" ) != tools.end()
                                                : false,
                                        this->gameManager->getDonuts( whoWaitsToPlay ),
                                        inactivePlayer->getLabel()
                                )
                        ) ;
                }

                // store players
                players.player( playerSequence );

                sgxml::SaveGameXML saveGameXML( exploredRooms,
                    this->gameManager->isFreePlanet( "byblos" ),
                    this->gameManager->isFreePlanet( "egyptus" ),
                    this->gameManager->isFreePlanet( "penitentiary" ),
                    this->gameManager->isFreePlanet( "safari" ),
                    this->gameManager->isFreePlanet( "blacktooth" ),
                    bonus,
                    players );

                xml_schema::namespace_infomap map;
                map[ "" ].name = "";
                map[ "" ].schema = "savegame.xsd";

                std::cout << "save game \"" << fileName.c_str() << "\"" << std::endl ;
                std::ofstream outputFile( fileName.c_str() );
                sgxml::savegame( outputFile, saveGameXML, map );
        }
        catch ( const xml_schema::exception& e )
        {
                std::cerr << e << std::endl ;
        }
}

void GameFileManager::updateAttributesOfPlayers( const sgxml::players::player_sequence& players )
{
        for ( sgxml::players::player_const_iterator i = players.begin (); i != players.end (); ++i )
        {
                sgxml::player data = *i;

                if ( data.label() == "headoverheels" )
                {
                        // formula for lives of composite character from lives of simple characters is
                        // lives H & H = 100 * lives Head + lives Heels

                        unsigned short lives = data.lives();
                        unsigned char headLives = 0;
                        unsigned char heelsLives = 0;

                        while ( lives > 100 )
                        {
                                lives -= 100;
                                headLives++;
                        }

                        heelsLives = static_cast< unsigned char >( lives );

                        this->gameManager->setHeadLives( headLives );
                        this->gameManager->setHorn( data.hasHorn() );
                        this->gameManager->setDonuts( data.ammo() );
                        this->gameManager->setHeelsLives( heelsLives );
                        this->gameManager->setHandbag( data.hasHandbag() );
                        this->gameManager->setHighSpeed( 0 );
                        this->gameManager->setHighJumps( 0 );
                        this->gameManager->setHeadShield( 0 );
                        this->gameManager->setHeelsShield( 0 );
                }
                else if ( data.label() == "head" )
                {
                        this->gameManager->setHeadLives( data.lives() );
                        this->gameManager->setHorn( data.hasHorn() );
                        this->gameManager->setDonuts( data.ammo() );
                        this->gameManager->setHighSpeed( 0 );
                        this->gameManager->setHeadShield( 0 );
                }
                else if ( data.label() == "heels" )
                {
                        this->gameManager->setHeelsLives( data.lives() );
                        this->gameManager->setHandbag( data.hasHandbag() );
                        this->gameManager->setHighJumps( 0 );
                        this->gameManager->setHeelsShield( 0 );
                }
        }
}

}

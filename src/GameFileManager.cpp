
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
        , xFish( 0 )
        , yFish( 0 )
        , zFish( 0 )
        , catchFishWay( Nowhere )
        , gameManager( const_cast< GameManager* >( gameManager ) )
        , isomot( const_cast< Isomot* >( isomot ) )
{

}

GameFileManager::~GameFileManager()
{

}

void GameFileManager::assignFishData( const std::string& room, const std::string& name, int x, int y, int z, const Way& way )
{
        this->room = room;
        this->nameOfCharacterWhoCaughtTheFish = name;
        this->xFish = x;
        this->yFish = y;
        this->zFish = z;
        this->catchFishWay = way;
}

void GameFileManager::loadGame( const std::string& fileName )
{
        try
        {
                std::auto_ptr< sgxml::SaveGameXML > saveGameXML( sgxml::savegame( fileName.c_str () ) );

                // visited rooms
                this->isomot->getMapManager()->readVisitedSequence( saveGameXML->exploredRooms().visited() );

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
                sgxml::exploredRooms exploredRooms;
                sgxml::exploredRooms::visited_sequence& sequenceOfRooms( exploredRooms.visited() );
                this->isomot->getMapManager()->storeVisitedSequence( sequenceOfRooms );
                exploredRooms.visited( sequenceOfRooms );

                sgxml::bonus bonus;
                sgxml::bonus::room_sequence& roomSequence( bonus.room() );
                BonusManager::getInstance()->save( roomSequence );
                bonus.room( roomSequence );

                sgxml::players players;
                sgxml::players::player_sequence playerSequence( players.player() );

                // active player
                std::string nameOfWhoPlaysYet = nameOfCharacterWhoCaughtTheFish;

                unsigned short lives = 0;
                if ( nameOfWhoPlaysYet == "head" || nameOfWhoPlaysYet == "heels" )
                {
                        lives = this->gameManager->getLives( nameOfWhoPlaysYet );
                }
                else if ( nameOfWhoPlaysYet == "headoverheels"  )
                {
                        lives = this->gameManager->getLives( "head" ) * 100 + this->gameManager->getLives( "heels" );
                }

                // possession of objects
                std::vector< std::string > tools = this->gameManager->getToolsOwnedByPlayer( nameOfWhoPlaysYet );

                // store all data
                playerSequence.push_back(
                        sgxml::player
                        (
                                true, // active player
                                this->room,
                                this->xFish,
                                this->yFish,
                                this->zFish,
                                this->catchFishWay.getIntegerOfWay(),
                                JustWait,
                                lives,
                                ( nameOfWhoPlaysYet == "head" || nameOfWhoPlaysYet == "headoverheels" )
                                        ? std::find( tools.begin (), tools.end (), "horn" ) != tools.end ()
                                        : false,
                                ( nameOfWhoPlaysYet == "heels" || nameOfWhoPlaysYet == "headoverheels" )
                                        ? std::find( tools.begin (), tools.end (), "handbag" ) != tools.end ()
                                        : false,
                                this->gameManager->getDonuts( nameOfWhoPlaysYet ),
                                nameOfCharacterWhoCaughtTheFish
                        )
                );

                Room* activeRoom = this->isomot->getMapManager()->getActiveRoom();
                Room* secondRoom = this->isomot->getMapManager()->getRoomOfInactivePlayer();

                // there may be no more rooms because there are no more players
                // or because other player is in the same room as active player

                const PlayerItem* whoWaitsToPlay = nilPointer ;

                std::string nameOfWhoWaitsToPlay = "nobody";

                if ( secondRoom != nilPointer )
                {
                        nameOfWhoWaitsToPlay = secondRoom->getMediator()->getLabelOfActiveCharacter();
                }
                else
                if ( activeRoom->getMediator()->getWaitingCharacter() != nilPointer )
                {
                        nameOfWhoWaitsToPlay = activeRoom->getMediator()->getWaitingCharacter()->getLabel();
                }

                if ( nameOfWhoWaitsToPlay != "nobody" )
                {
                        std::list< const PlayerItem * > playersOnEntry = ( secondRoom != nilPointer ?
                                                                                secondRoom->getPlayersWhoEnteredRoom() :
                                                                                activeRoom->getPlayersWhoEnteredRoom() );

                        for ( std::list< const PlayerItem * >::const_iterator p = playersOnEntry.begin (); p != playersOnEntry.end (); ++p )
                        {
                                if ( ( *p )->getLabel() == nameOfWhoWaitsToPlay )
                                {
                                        whoWaitsToPlay = *p ;
                                        break;
                                }
                        }
                }

                if ( whoWaitsToPlay != nilPointer )
                {
                        std::vector< std::string > tools = this->gameManager->getToolsOwnedByPlayer( whoWaitsToPlay->getLabel() );

                        playerSequence.push_back(
                                sgxml::player
                                (
                                        false, // inactive player
                                        secondRoom != nilPointer ? secondRoom->getNameOfFileWithDataAboutRoom() : activeRoom->getNameOfFileWithDataAboutRoom(),
                                        whoWaitsToPlay->getX(),
                                        whoWaitsToPlay->getY(),
                                        whoWaitsToPlay->getZ(),
                                        whoWaitsToPlay->getOrientation().getIntegerOfWay(),
                                        Way( whoWaitsToPlay->getWayOfEntry() ).getIntegerOfWay(),
                                        whoWaitsToPlay->getLives(),
                                        whoWaitsToPlay->getLabel() == "head"
                                                ? std::find( tools.begin(), tools.end(), "horn" ) != tools.end()
                                                : false,
                                        whoWaitsToPlay->getLabel() == "heels"
                                                ? std::find( tools.begin(), tools.end(), "handbag" ) != tools.end()
                                                : false,
                                        this->gameManager->getDonuts( whoWaitsToPlay->getLabel() ),
                                        whoWaitsToPlay->getLabel()
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

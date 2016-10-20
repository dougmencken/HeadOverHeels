
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
: roomId( std::string() ),
  label( 0 ),
  x( 0 ),
  y( 0 ),
  z( 0 ),
  direction( NoDirection ),
  gameManager( const_cast< GameManager* >( gameManager ) ),
  isomot( const_cast< Isomot* >( isomot ) )
{

}

GameFileManager::~GameFileManager()
{

}

void GameFileManager::assignFishData( const std::string& roomId, short label, int x, int y, int z, const Direction& direction )
{
        this->roomId = roomId;
        this->label = label;
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

                // Establece las salas que ya han sido visitadas
                this->isomot->getMapManager()->load( saveGameXML->exploredRooms().visited() );

                // Asigna los bonus que no deben aparecer en esta partida
                BonusManager::getInstance()->load( saveGameXML->bonus().room() );

                if ( saveGameXML->freeByblos() )
                {
                        this->gameManager->liberatePlanet( Byblos, false );
                }
                if ( saveGameXML->freeEgyptus() )
                {
                        this->gameManager->liberatePlanet( Egyptus, false );
                }
                if ( saveGameXML->freePenitentiary() )
                {
                        this->gameManager->liberatePlanet( Penitentiary, false );
                }
                if ( saveGameXML->freeSafari() )
                {
                        this->gameManager->liberatePlanet( Safari, false );
                }
                if ( saveGameXML->freeBlacktooth() )
                {
                        this->gameManager->liberatePlanet( Blacktooth, false );
                }

                // Asigna el estado de los jugadores
                this->assignPlayerStatus( saveGameXML->players().player() );
                // Crea las salas iniciales y la posición de los jugadores
                this->isomot->beginOld( saveGameXML->players().player() );
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
                sgxml::exploredRooms::visited_sequence& visitedSequence( exploredRooms.visited() );
                this->isomot->getMapManager()->save( visitedSequence );
                exploredRooms.visited( visitedSequence );

                // Se almacenan los bonus que se hallan cogido
                sgxml::bonus bonus;
                sgxml::bonus::room_sequence& roomSequence( bonus.room() );
                BonusManager::getInstance()->save( roomSequence );
                bonus.room( roomSequence );

                // Se almacena el estado de los jugadores
                sgxml::players players;
                sgxml::players::player_sequence playerSequence( players.player() );

                // El jugador activo
                PlayerId activePlayerId = PlayerId( this->label );

                // Número de vidas
                unsigned short lives = 0;
                switch ( activePlayerId )
                {
                case Head:
                case Heels:
                lives = this->gameManager->getLives( activePlayerId );
                break;

                case HeadAndHeels:
                lives = this->gameManager->getLives( Head ) * 100 + this->gameManager->getLives( Heels );
                break;

                default:
                ;
                }
                // Posesión de objetos
                std::vector< short > tools = this->gameManager->hasTool( activePlayerId );
                // Almacenamiento de todos los datos
                playerSequence.push_back( sgxml::player( true,
                             this->roomId,
                             this->x,
                             this->y,
                             this->z,
                             int( this->direction ),
                             Wait,
                             lives,
                             activePlayerId == Head || activePlayerId == HeadAndHeels ? std::find( tools.begin (), tools.end (), short( Horn ) ) != tools.end () : false,
                             activePlayerId == Heels || activePlayerId == HeadAndHeels ? std::find( tools.begin (), tools.end (), short( Handbag) ) != tools.end () : false,
                             this->gameManager->getDonuts( activePlayerId ),
                             this->label ) );

                // Es posible que no haya más salas bien porque no haya más jugadores o porque el otro jugador
                // se encuentre en la misma sala que el jugador activo
                Room* hideRoom = this->isomot->getMapManager()->getHideRoom();
                Room* activeRoom = this->isomot->getMapManager()->getActiveRoom();
                PlayerItem* inactivePlayer = ( hideRoom != 0 ? hideRoom->getMediator()->getActivePlayer() : activeRoom->getMediator()->getHiddenPlayer() );

                // Si hay algún otro jugador, se almacenan sus datos
                if ( inactivePlayer != 0 )
                {
                        PlayerId inactivePlayerId = PlayerId( inactivePlayer->getLabel() );
                        std::vector< short > tools = this->gameManager->hasTool( inactivePlayerId );
                        PlayerStartPosition* initialPosition = this->isomot->getMapManager()->findPlayerPosition(
                                hideRoom != 0 ? hideRoom->getIdentifier() : activeRoom->getIdentifier(), PlayerId( inactivePlayer->getLabel() )
                        );
                        if ( initialPosition == 0 )
                        {
                                initialPosition = this->isomot->getMapManager()->findPlayerPosition( activeRoom->getIdentifier(), HeadAndHeels );
                        }
                        playerSequence.push_back( sgxml::player( false,
                                       hideRoom != 0 ? hideRoom->getIdentifier() : activeRoom->getIdentifier(),
                                       initialPosition->getX(),
                                       initialPosition->getY(),
                                       initialPosition->getZ(),
                                       int( inactivePlayer->getDirection() ),
                                       initialPosition->getEntry(),
                                       this->gameManager->getLives( inactivePlayerId ),
                                       inactivePlayerId == Head || inactivePlayerId == HeadAndHeels ? std::find( tools.begin(), tools.end(), short( Horn ) ) != tools.end() : false,
                                       inactivePlayerId == Heels || inactivePlayerId == HeadAndHeels ? std::find( tools.begin(), tools.end(), short( Handbag ) ) != tools.end() : false,
                                       this->gameManager->getDonuts( inactivePlayerId ),
                                       inactivePlayer->getLabel() ) );
                }

                // Almacena los jugadores
                players.player( playerSequence );

                // Creación de la configuración
                sgxml::SaveGameXML saveGameXML( exploredRooms,
                    this->gameManager->isFreePlanet( Byblos ),
                    this->gameManager->isFreePlanet( Egyptus ),
                    this->gameManager->isFreePlanet( Penitentiary ),
                    this->gameManager->isFreePlanet( Safari ),
                    this->gameManager->isFreePlanet( Blacktooth ),
                    bonus,
                    players );

                // Información del esquema que valida la corrección de los datos
                xml_schema::namespace_infomap map;
                map[ "" ].name = "";
                map[ "" ].schema = "savegame.xsd";

                // Creación del archivo
                std::cout << "save game \"" << fileName.c_str() << "\"" << std::endl ;
                std::ofstream outputFile( fileName.c_str() );
                sgxml::savegame( outputFile, saveGameXML, map );
        }
        catch ( const xml_schema::exception& e )
        {
                std::cerr << e << std::endl ;
        }
}

void GameFileManager::assignPlayerStatus( const sgxml::players::player_sequence& playerSequence )
{
        for ( sgxml::players::player_const_iterator i = playerSequence.begin (); i != playerSequence.end (); ++i )
        {
                sgxml::player data = *i;

                unsigned char headLives = 0;
                unsigned char heelsLives = 0;

                // Si se va a crear al jugador compuesto se obtienen las vidas de los jugadores simples
                // Se utiliza la fórmula: Vidas H&H = Vidas Head * 100 + Vidas Heels
                if ( data.label() == HeadAndHeels )
                {
                        unsigned short lives = data.lives();

                        while ( lives > 100 )
                        {
                                lives -= 100;
                                headLives++;
                        }
                        heelsLives = static_cast< unsigned char >( lives );
                }

                switch ( data.label() )
                {
                        case Head:

                                this->gameManager->setHeadLives( data.lives() );
                                this->gameManager->setHorn( data.hasHorn() );
                                this->gameManager->setDonuts( data.ammo() );
                                this->gameManager->setHighSpeed( 0 );
                                this->gameManager->setHeadShield( 0 );
                                break;

                        case Heels:

                                this->gameManager->setHeelsLives( data.lives() );
                                this->gameManager->setHandbag( data.hasHandbag() );
                                this->gameManager->setHighJumps( 0 );
                                this->gameManager->setHeelsShield( 0 );
                                break;

                        case HeadAndHeels:

                                this->gameManager->setHeadLives( headLives );
                                this->gameManager->setHorn( data.hasHorn() );
                                this->gameManager->setDonuts( data.ammo() );
                                this->gameManager->setHeelsLives( heelsLives );
                                this->gameManager->setHandbag( data.hasHandbag() );
                                this->gameManager->setHighSpeed( 0 );
                                this->gameManager->setHighJumps( 0 );
                                this->gameManager->setHeadShield( 0 );
                                this->gameManager->setHeelsShield( 0 );
                                break;
                }
        }
}

}

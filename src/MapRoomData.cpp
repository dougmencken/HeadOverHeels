
#include "MapRoomData.hpp"


namespace isomot
{

MapRoomData::MapRoomData( const std::string& room )
: room( room ),
  visited( false ),
  activePlayer( NoPlayer )
{
}

MapRoomData::~MapRoomData()
{
}

void MapRoomData::addPlayerPosition( const PlayerStartPosition& playerPosition )
{
        this->playersPosition.push_back( playerPosition );
        this->visited = true;
}

void MapRoomData::addPlayerPosition( PlayerStartPosition& playerPosition, PlayerStartPosition& playerPresentPosition )
{
        PlayerStartPosition* position = this->findPlayerPosition( playerPresentPosition.getPlayer() );
        if ( position != 0 )
        {
                if ( playerPosition.getEntry() == position->getEntry() )
                {
                        this->removePlayerPosition( playerPresentPosition.getPlayer() );
                        this->playersPosition.push_back( playerPresentPosition );
                }
        }
        this->playersPosition.push_back( playerPosition );
        this->visited = true;
}

void MapRoomData::removePlayerPosition( const WhichPlayer& player )
{
        // Se comprueba si el jugador compuesto estaba en la sala porque de ser así, al salir alguno
        // de los jugadores simples habrá que alterar el jugador que queda en la sala por el otro jugador
        // simple
        std::list< PlayerStartPosition >::iterator i = std::find_if( playersPosition.begin(), playersPosition.end(), std::bind2nd( EqualPlayerStartPosition(), HeadAndHeels ) );
        if ( i != playersPosition.end() )
        {
                PlayerStartPosition singlePlayerPosition( player == Head ? Heels : Head );
                singlePlayerPosition.assignPosition( ( *i ).getEntry(), ( *i ).getX(), ( *i ).getY(), ( *i ).getZ(), ( *i ).getOrientation() );
                i = playersPosition.erase( std::remove_if( playersPosition.begin(), playersPosition.end(), std::bind2nd( EqualPlayerStartPosition(), HeadAndHeels ) ), playersPosition.end() );
                i = playersPosition.insert( i, singlePlayerPosition );
        }
        // Había sólo un jugador o estaban los dos jugadores simples y abandona uno la sala
        else
        {
                i = playersPosition.erase(std::remove_if(playersPosition.begin(), playersPosition.end(), std::bind2nd(EqualPlayerStartPosition(), player)), playersPosition.end());
                // Si se ha eliminado al jugador activo y queda el otro habrá que seleccionarlo para asumir ese papel
                if ( ! playersPosition.empty() && player == activePlayer && ! playersPosition.empty() )
                {
                        activePlayer = ( i != playersPosition.end() ? ( *i ).getPlayer() : ( *playersPosition.begin() ).getPlayer() );
                }
        }
}

PlayerStartPosition* MapRoomData::findPlayerPosition( const WhichPlayer& player )
{
        std::list< PlayerStartPosition >::iterator i = find_if( playersPosition.begin(), playersPosition.end(), std::bind2nd( EqualPlayerStartPosition(), player ) );
        return ( i != playersPosition.end() ? ( &( *i ) ) : 0 );
}

bool MapRoomData::remainPlayers()
{
        return ! playersPosition.empty () ;
}

std::string MapRoomData::findConnectedRoom( const Direction& exit, Direction* entry ) const
{
        std::string fileName;

        switch ( exit )
        {
                case North:
                        fileName = north;
                        *entry = South;
                        break;

                case South:
                        fileName = south;
                        *entry = North;
                        break;

                case East:
                        fileName = east;
                        *entry = West;
                        break;

                case West:
                        fileName = west;
                        *entry = East;
                        break;

                case Northeast:
                        fileName = northEast;
                        *entry = South;
                        break;

                case Southeast:
                        fileName = southEast;
                        *entry = North;
                        break;

                case Southwest:
                        fileName = southWest;
                        *entry = North;
                        break;

                case Northwest:
                        fileName = northWest;
                        *entry = South;
                        break;

                case Eastnorth:
                        fileName = eastNorth;
                        *entry = West;
                        break;

                case Eastsouth:
                        fileName = eastSouth;
                        *entry = West;
                        break;

                case Westnorth:
                        fileName = westNorth;
                        *entry = East;
                        break;

                case Westsouth:
                        fileName = westSouth;
                        *entry = East;
                        break;

                case Up:
                        fileName = roof;
                        *entry = Down;
                        break;

                case Down:
                        fileName = floor;
                        *entry = Up;
                        break;

                case ByTeleport:
                        fileName = teleport;
                        *entry = ByTeleport;
                        break;

                case ByTeleportToo:
                        fileName = teleport2;
                        *entry = ByTeleportToo;
                        break;

                default:
                        ;
        }

        return fileName;
}

void MapRoomData::adjustEntry( Direction* entry, const std::string& previousRoom )
{
	switch ( *entry )
	{
		case North:
			if ( north.empty() )
			{
				if ( northEast.compare( previousRoom ) == 0 )
				{
					*entry = Northeast;
				}
				else if ( northWest.compare( previousRoom ) == 0 )
				{
					*entry = Northwest;
				}
			}
			break;

		case South:
			if ( south.empty() )
			{
				if ( southEast.compare( previousRoom ) == 0 )
				{
					*entry = Southeast;
				}
				else if ( southWest.compare( previousRoom ) == 0 )
				{
					*entry = Southwest;
				}
			}
			break;

		case East:
			if ( east.empty() )
			{
				if ( eastNorth.compare( previousRoom ) == 0 )
				{
					*entry = Eastnorth;
				}
				else if ( eastSouth.compare( previousRoom ) == 0 )
				{
					*entry = Eastsouth;
				}
			}
			break;

		case West:
			if ( west.empty() )
			{
				if ( westNorth.compare( previousRoom ) == 0 )
				{
					*entry = Westnorth;
				}
				else if ( westSouth.compare( previousRoom ) == 0 )
				{
					*entry = Westsouth;
				}
			}
			break;

		default:
			;
	}
}

void MapRoomData::clearPlayersPosition()
{
	this->playersPosition.clear();
}

void MapRoomData::reset()
{
	this->visited = false;
	this->playersPosition.clear();
}

PlayerStartPosition::PlayerStartPosition( const WhichPlayer& player )
{
	this->player = player;
}

PlayerStartPosition::~PlayerStartPosition()
{
}

void PlayerStartPosition::assignDoor( const Direction& door )
{
	this->entry = door;
}

void PlayerStartPosition::assignPosition( const Direction& entry, int x, int y, int z, const Direction& orientation )
{
	this->entry = entry;
	this->x = x;
	this->y = y;
	this->z = z;
	this->orientation = orientation;
}

bool EqualPlayerStartPosition::operator() ( const PlayerStartPosition& position, const WhichPlayer& player ) const
{
	return ( position.getPlayer() == player );
}

}


#include "MapRoomData.hpp"


namespace isomot
{

MapRoomData::MapRoomData( const std::string& room )
        : room( room )
        , visited( false )
        , nameOfActivePlayer( "in~room" )
{
}

MapRoomData::~MapRoomData()
{
}

void MapRoomData::addPlayerPosition( const PlayerInitialPosition& playerPosition )
{
        this->playersPosition.push_back( playerPosition );
        this->visited = true;
}

void MapRoomData::addPlayerPosition( PlayerInitialPosition& playerPosition, PlayerInitialPosition& playerPresentPosition )
{
        PlayerInitialPosition* position = this->findPlayerPosition( playerPresentPosition.getPlayer() );
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

void MapRoomData::removePlayerPosition( const std::string& player )
{
        // check if composite player is in room ...
        std::list< PlayerInitialPosition >::iterator i = std::find_if( playersPosition.begin(), playersPosition.end(), std::bind2nd( EqualPlayerInitialPosition(), "headoverheels" ) );
        if ( i != playersPosition.end() )
        {
                // ... if so, alter position of other simple player too
                PlayerInitialPosition simplePlayerPosition( player == "head" ? "heels" : "head" );
                simplePlayerPosition.assignPosition( ( *i ).getEntry(), ( *i ).getX(), ( *i ).getY(), ( *i ).getZ(), ( *i ).getOrientation() );
                i = playersPosition.erase( std::remove_if( playersPosition.begin (), playersPosition.end (), std::bind2nd( EqualPlayerInitialPosition(), "headoverheels" ) ), playersPosition.end () );
                i = playersPosition.insert( i, simplePlayerPosition );
        }
        else
        {
                // ... there was only one player or the two players are simple and leaves the same room together
                i = playersPosition.erase( std::remove_if( playersPosition.begin (), playersPosition.end (), std::bind2nd( EqualPlayerInitialPosition(), player ) ), playersPosition.end () );

                // when active player is over, activate other player
                if ( ! playersPosition.empty() && player == nameOfActivePlayer && ! playersPosition.empty() )
                {
                        nameOfActivePlayer = ( i != playersPosition.end() ? ( *i ).getPlayer() : ( *playersPosition.begin() ).getPlayer() );
                }
        }
}

PlayerInitialPosition* MapRoomData::findPlayerPosition( const std::string& player )
{
        std::list< PlayerInitialPosition >::iterator i = find_if( playersPosition.begin(), playersPosition.end(), std::bind2nd( EqualPlayerInitialPosition(), player ) );
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

PlayerInitialPosition::PlayerInitialPosition( const std::string& player )
{
        this->player = player;
}

PlayerInitialPosition::~PlayerInitialPosition()
{
}

void PlayerInitialPosition::assignDoor( const Direction& door )
{
        this->entry = door;
}

void PlayerInitialPosition::assignPosition( const Direction& entry, int x, int y, int z, const Direction& orientation )
{
        this->entry = entry;
        this->x = x;
        this->y = y;
        this->z = z;
        this->orientation = orientation;
}

bool EqualPlayerInitialPosition::operator() ( const PlayerInitialPosition& position, const std::string& player ) const
{
        return ( position.getPlayer() == player );
}

}

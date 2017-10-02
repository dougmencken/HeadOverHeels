
#include "MapRoomData.hpp"


namespace isomot
{

MapRoomData::MapRoomData( const std::string& room )
        : room( room )
        , visited( false )
{
}

MapRoomData::~MapRoomData()
{
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

}

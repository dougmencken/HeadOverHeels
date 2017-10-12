
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

std::string MapRoomData::findConnectedRoom( const Way& wayOfExit, Way* wayOfEntry ) const
{
        std::string fileName;

        switch ( wayOfExit.getIntegerOfWay () )
        {
                case North:
                        fileName = north;
                        *wayOfEntry = South;
                        break;

                case South:
                        fileName = south;
                        *wayOfEntry = North;
                        break;

                case East:
                        fileName = east;
                        *wayOfEntry = West;
                        break;

                case West:
                        fileName = west;
                        *wayOfEntry = East;
                        break;

                case Northeast:
                        fileName = northEast;
                        *wayOfEntry = South;
                        break;

                case Southeast:
                        fileName = southEast;
                        *wayOfEntry = North;
                        break;

                case Southwest:
                        fileName = southWest;
                        *wayOfEntry = North;
                        break;

                case Northwest:
                        fileName = northWest;
                        *wayOfEntry = South;
                        break;

                case Eastnorth:
                        fileName = eastNorth;
                        *wayOfEntry = West;
                        break;

                case Eastsouth:
                        fileName = eastSouth;
                        *wayOfEntry = West;
                        break;

                case Westnorth:
                        fileName = westNorth;
                        *wayOfEntry = East;
                        break;

                case Westsouth:
                        fileName = westSouth;
                        *wayOfEntry = East;
                        break;

                case Up:
                        fileName = roof;
                        *wayOfEntry = Down;
                        break;

                case Down:
                        fileName = floor;
                        *wayOfEntry = Up;
                        break;

                case ByTeleport:
                        fileName = teleport;
                        *wayOfEntry = ByTeleport;
                        break;

                case ByTeleportToo:
                        fileName = teleport2;
                        *wayOfEntry = ByTeleportToo;
                        break;

                default:
                        ;
        }

        return fileName;
}

void MapRoomData::adjustEntry( Way* entry, const std::string& previousRoom )
{
        switch ( entry->getIntegerOfWay() )
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

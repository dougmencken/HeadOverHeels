
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

std::string MapRoomData::findConnectedRoom( const std::string& wayOfExit, Way* wayOfEntry ) const
{
        std::string roomFile;

        switch ( Way( wayOfExit ).getIntegerOfWay () )
        {
                case North:
                        roomFile = north;
                        *wayOfEntry = South;
                        break;

                case South:
                        roomFile = south;
                        *wayOfEntry = North;
                        break;

                case East:
                        roomFile = east;
                        *wayOfEntry = West;
                        break;

                case West:
                        roomFile = west;
                        *wayOfEntry = East;
                        break;

                case Northeast:
                        roomFile = northEast;
                        *wayOfEntry = South;
                        break;

                case Southeast:
                        roomFile = southEast;
                        *wayOfEntry = North;
                        break;

                case Southwest:
                        roomFile = southWest;
                        *wayOfEntry = North;
                        break;

                case Northwest:
                        roomFile = northWest;
                        *wayOfEntry = South;
                        break;

                case Eastnorth:
                        roomFile = eastNorth;
                        *wayOfEntry = West;
                        break;

                case Eastsouth:
                        roomFile = eastSouth;
                        *wayOfEntry = West;
                        break;

                case Westnorth:
                        roomFile = westNorth;
                        *wayOfEntry = East;
                        break;

                case Westsouth:
                        roomFile = westSouth;
                        *wayOfEntry = East;
                        break;

                case Up:
                        roomFile = roof;
                        *wayOfEntry = Down;
                        break;

                case Down:
                        roomFile = floor;
                        *wayOfEntry = Up;
                        break;

                case ByTeleport:
                        roomFile = teleport;
                        *wayOfEntry = ByTeleport;
                        break;

                case ByTeleportToo:
                        roomFile = teleport2;
                        *wayOfEntry = ByTeleportToo;
                        break;

                default:
                        ;
        }

        return roomFile;
}

void MapRoomData::adjustEntry( Way* entry, const std::string& previousRoom )
{
        switch ( entry->getIntegerOfWay() )
        {
                case North:
                        if ( north.empty() )
                        {
                                if ( previousRoom == northEast )
                                {
                                        *entry = Northeast;
                                }
                                else if ( previousRoom == northWest )
                                {
                                        *entry = Northwest;
                                }
                        }
                        break;

                case South:
                        if ( south.empty() )
                        {
                                if ( previousRoom == southEast )
                                {
                                        *entry = Southeast;
                                }
                                else if ( previousRoom == southWest )
                                {
                                        *entry = Southwest;
                                }
                        }
                        break;

                case East:
                        if ( east.empty() )
                        {
                                if ( eastNorth == previousRoom )
                                {
                                        *entry = Eastnorth;
                                }
                                else if ( eastSouth == previousRoom )
                                {
                                        *entry = Eastsouth;
                                }
                        }
                        break;

                case West:
                        if ( west.empty() )
                        {
                                if ( westNorth == previousRoom )
                                {
                                        *entry = Westnorth;
                                }
                                else if ( westSouth == previousRoom )
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

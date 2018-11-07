
#include "RoomConnections.hpp"


namespace iso
{

RoomConnections::RoomConnections( )
{
}

RoomConnections::~RoomConnections( )
{
}

std::string RoomConnections::findConnectedRoom( const std::string& wayOfExit, Way* wayOfEntry ) const
{
        std::string roomFile;

        switch ( Way( wayOfExit ).getIntegerOfWay () )
        {
                case Way::North:
                        roomFile = north;
                        *wayOfEntry = Way::South;
                        break;

                case Way::South:
                        roomFile = south;
                        *wayOfEntry = Way::North;
                        break;

                case Way::East:
                        roomFile = east;
                        *wayOfEntry = Way::West;
                        break;

                case Way::West:
                        roomFile = west;
                        *wayOfEntry = Way::East;
                        break;

                case Way::Northeast:
                        roomFile = northEast;
                        *wayOfEntry = Way::South;
                        break;

                case Way::Southeast:
                        roomFile = southEast;
                        *wayOfEntry = Way::North;
                        break;

                case Way::Southwest:
                        roomFile = southWest;
                        *wayOfEntry = Way::North;
                        break;

                case Way::Northwest:
                        roomFile = northWest;
                        *wayOfEntry = Way::South;
                        break;

                case Way::Eastnorth:
                        roomFile = eastNorth;
                        *wayOfEntry = Way::West;
                        break;

                case Way::Eastsouth:
                        roomFile = eastSouth;
                        *wayOfEntry = Way::West;
                        break;

                case Way::Westnorth:
                        roomFile = westNorth;
                        *wayOfEntry = Way::East;
                        break;

                case Way::Westsouth:
                        roomFile = westSouth;
                        *wayOfEntry = Way::East;
                        break;

                case Way::Up:
                        roomFile = roof;
                        *wayOfEntry = Way::Down;
                        break;

                case Way::Down:
                        roomFile = floor;
                        *wayOfEntry = Way::Up;
                        break;

                case Way::ByTeleport:
                        roomFile = teleport;
                        *wayOfEntry = Way::ByTeleport;
                        break;

                case Way::ByTeleportToo:
                        roomFile = teleport2;
                        *wayOfEntry = Way::ByTeleportToo;
                        break;

                default:
                        ;
        }

        return roomFile;
}

void RoomConnections::adjustEntry( Way* entry, const std::string& previousRoom ) const
{
        switch ( entry->getIntegerOfWay() )
        {
                case Way::North:
                        if ( north.empty() )
                        {
                                if ( previousRoom == northEast )
                                {
                                        *entry = Way::Northeast;
                                }
                                else if ( previousRoom == northWest )
                                {
                                        *entry = Way::Northwest;
                                }
                        }
                        break;

                case Way::South:
                        if ( south.empty() )
                        {
                                if ( previousRoom == southEast )
                                {
                                        *entry = Way::Southeast;
                                }
                                else if ( previousRoom == southWest )
                                {
                                        *entry = Way::Southwest;
                                }
                        }
                        break;

                case Way::East:
                        if ( east.empty() )
                        {
                                if ( eastNorth == previousRoom )
                                {
                                        *entry = Way::Eastnorth;
                                }
                                else if ( eastSouth == previousRoom )
                                {
                                        *entry = Way::Eastsouth;
                                }
                        }
                        break;

                case Way::West:
                        if ( west.empty() )
                        {
                                if ( westNorth == previousRoom )
                                {
                                        *entry = Way::Westnorth;
                                }
                                else if ( westSouth == previousRoom )
                                {
                                        *entry = Way::Westsouth;
                                }
                        }
                        break;

                default:
                        ;
        }
}

}

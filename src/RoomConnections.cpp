
#include "RoomConnections.hpp"


namespace iso
{

std::string RoomConnections::findConnectedRoom( const std::string& wayOfExit, Way* wayOfEntry ) const
{
        std::string roomFile ;

        switch ( Way( wayOfExit ).getIntegerOfWay () )
        {
                case Way::North:
                        roomFile = north ;
                        *wayOfEntry = Way::South;
                        break;

                case Way::South:
                        roomFile = south ;
                        *wayOfEntry = Way::North;
                        break;

                case Way::East:
                        roomFile = east ;
                        *wayOfEntry = Way::West;
                        break;

                case Way::West:
                        roomFile = west ;
                        *wayOfEntry = Way::East;
                        break;

                case Way::Northeast:
                        roomFile = northeast ;
                        *wayOfEntry = Way::South;
                        break;

                case Way::Southeast:
                        roomFile = southeast ;
                        *wayOfEntry = Way::North;
                        break;

                case Way::Southwest:
                        roomFile = southwest ;
                        *wayOfEntry = Way::North;
                        break;

                case Way::Northwest:
                        roomFile = northwest ;
                        *wayOfEntry = Way::South;
                        break;

                case Way::Eastnorth:
                        roomFile = eastnorth ;
                        *wayOfEntry = Way::West;
                        break;

                case Way::Eastsouth:
                        roomFile = eastsouth ;
                        *wayOfEntry = Way::West;
                        break;

                case Way::Westnorth:
                        roomFile = westnorth ;
                        *wayOfEntry = Way::East;
                        break;

                case Way::Westsouth:
                        roomFile = westsouth ;
                        *wayOfEntry = Way::East;
                        break;

                case Way::Up:
                        roomFile = roof ;
                        *wayOfEntry = Way::Down;
                        break;

                case Way::Down:
                        roomFile = floor ;
                        *wayOfEntry = Way::Up;
                        break;

                case Way::ByTeleport:
                        roomFile = teleport ;
                        *wayOfEntry = Way::ByTeleport;
                        break;

                case Way::ByTeleportToo:
                        roomFile = teleport2 ;
                        *wayOfEntry = Way::ByTeleportToo;
                        break;

                default:
                        ;
        }

        return roomFile ;
}

void RoomConnections::adjustEntry( Way* entry, const std::string& previousRoom ) const
{
        switch ( entry->getIntegerOfWay() )
        {
                case Way::North:
                        if ( north.empty() )
                        {
                                if ( previousRoom == northeast )
                                        *entry = Way::Northeast ;
                                else if ( previousRoom == northwest )
                                        *entry = Way::Northwest ;
                        }
                        break;

                case Way::South:
                        if ( south.empty() )
                        {
                                if ( previousRoom == southeast )
                                        *entry = Way::Southeast ;
                                else if ( previousRoom == southwest )
                                        *entry = Way::Southwest ;
                        }
                        break;

                case Way::East:
                        if ( east.empty() )
                        {
                                if ( eastnorth == previousRoom )
                                        *entry = Way::Eastnorth ;
                                else if ( eastsouth == previousRoom )
                                        *entry = Way::Eastsouth ;
                        }
                        break;

                case Way::West:
                        if ( west.empty() )
                        {
                                if ( westnorth == previousRoom )
                                        *entry = Way::Westnorth ;
                                else if ( westsouth == previousRoom )
                                        *entry = Way::Westsouth ;
                        }
                        break;

                default:
                        ;
        }
}

}

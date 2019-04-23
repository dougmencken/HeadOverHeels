
#include "RoomConnections.hpp"


namespace iso
{

std::string RoomConnections::findConnectedRoom( const std::string& wayOfExit, Way* wayOfEntry ) const
{
        switch ( Way( wayOfExit ).getIntegerOfWay () )
        {
                case Way::North:
                        *wayOfEntry = Way::South;
                        break;

                case Way::South:
                        *wayOfEntry = Way::North;
                        break;

                case Way::East:
                        *wayOfEntry = Way::West;
                        break;

                case Way::West:
                        *wayOfEntry = Way::East;
                        break;

                case Way::Northeast:
                        *wayOfEntry = Way::South;
                        break;

                case Way::Southeast:
                        *wayOfEntry = Way::North;
                        break;

                case Way::Southwest:
                        *wayOfEntry = Way::North;
                        break;

                case Way::Northwest:
                        *wayOfEntry = Way::South;
                        break;

                case Way::Eastnorth:
                        *wayOfEntry = Way::West;
                        break;

                case Way::Eastsouth:
                        *wayOfEntry = Way::West;
                        break;

                case Way::Westnorth:
                        *wayOfEntry = Way::East;
                        break;

                case Way::Westsouth:
                        *wayOfEntry = Way::East;
                        break;

                case Way::Above:
                        *wayOfEntry = Way::Below;
                        break;

                case Way::Below:
                        *wayOfEntry = Way::Above;
                        break;

                case Way::ByTeleport:
                        *wayOfEntry = Way::ByTeleport;
                        break;

                case Way::ByTeleportToo:
                        *wayOfEntry = Way::ByTeleportToo;
                        break;

                default:
                        ;
        }

        return getConnectedRoomAt( wayOfExit ) ;
}

void RoomConnections::adjustEntry( Way* entry, const std::string& previousRoom ) const
{
        switch ( entry->getIntegerOfWay() )
        {
                case Way::North:
                        if ( getNorth().empty() )
                        {
                                if ( previousRoom == getNorthEast() )
                                        *entry = Way::Northeast ;
                                else if ( previousRoom == getNorthWest() )
                                        *entry = Way::Northwest ;
                        }
                        break;

                case Way::South:
                        if ( getSouth().empty() )
                        {
                                if ( previousRoom == getSouthEast() )
                                        *entry = Way::Southeast ;
                                else if ( previousRoom == getSouthWest() )
                                        *entry = Way::Southwest ;
                        }
                        break;

                case Way::East:
                        if ( getEast().empty() )
                        {
                                if ( getEastNorth() == previousRoom )
                                        *entry = Way::Eastnorth ;
                                else if ( getEastSouth() == previousRoom )
                                        *entry = Way::Eastsouth ;
                        }
                        break;

                case Way::West:
                        if ( getWest().empty() )
                        {
                                if ( getWestNorth() == previousRoom )
                                        *entry = Way::Westnorth ;
                                else if ( getWestSouth() == previousRoom )
                                        *entry = Way::Westsouth ;
                        }
                        break;

                default:
                        ;
        }
}

}


#include "ConnectedRooms.hpp"


std::string ConnectedRooms::clarifyTheWayOfEntryToABigRoom( const std::string & wayOfEntry, const std::string & previousRoom ) const
{
        if ( wayOfEntry == "north" && getConnectedRoomAt( "north" ).empty() )
        {
                if ( previousRoom == getConnectedRoomAt( "northeast" ) ) return "northeast" ;
                if ( previousRoom == getConnectedRoomAt( "northwest" ) ) return "northwest" ;
        }

        if ( wayOfEntry == "south" && getConnectedRoomAt( "south" ).empty() )
        {
                if ( previousRoom == getConnectedRoomAt( "southeast" ) ) return "southeast" ;
                if ( previousRoom == getConnectedRoomAt( "southwest" ) ) return "southwest" ;
        }

        if ( wayOfEntry == "east" && getConnectedRoomAt( "east" ).empty() )
        {
                if ( getConnectedRoomAt( "eastnorth" ) == previousRoom ) return "eastnorth" ;
                if ( getConnectedRoomAt( "eastsouth" ) == previousRoom ) return "eastsouth" ;
        }

        if ( wayOfEntry == "west" && getConnectedRoomAt( "west" ).empty() )
        {
                if ( getConnectedRoomAt( "westnorth" ) == previousRoom ) return "westnorth" ;
                if ( getConnectedRoomAt( "westsouth" ) == previousRoom ) return "westsouth" ;
        }

        return wayOfEntry ;
}

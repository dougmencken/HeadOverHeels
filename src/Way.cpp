
#include "Way.hpp"


namespace iso
{

Way::Way( unsigned int way )
        : way( way )
{

}

Way::Way( const std::string& stringOfWay )
{
        if ( stringOfWay == "north" )
                way = North;
        else if ( stringOfWay == "south" )
                way = South;
        else if ( stringOfWay == "east" )
                way = East;
        else if ( stringOfWay == "west" )
                way = West;
        else if ( stringOfWay == "northeast" )
                way = Northeast;
        else if ( stringOfWay == "southeast" )
                way = Southeast;
        else if ( stringOfWay == "southwest" )
                way = Southwest;
        else if ( stringOfWay == "northwest" )
                way = Northwest;
        else if ( stringOfWay == "eastnorth" )
                way = Eastnorth;
        else if ( stringOfWay == "eastsouth" )
                way = Eastsouth;
        else if ( stringOfWay == "westnorth" )
                way = Westnorth;
        else if ( stringOfWay == "westsouth" )
                way = Westsouth;
        else if ( stringOfWay == "up" )
                way = Up;
        else if ( stringOfWay == "down" )
                way = Down;
        else if ( stringOfWay == "via teleport" )
                way = ByTeleport;
        else if ( stringOfWay == "via second teleport" )
                way = ByTeleportToo;
        else if ( stringOfWay == "no exit" )
                way = NoExit;
        else if ( stringOfWay == "rebuild room" || stringOfWay == "restart room" )
                way = Restart;
        else if ( stringOfWay == "just wait" )
                way = JustWait;
        else
                way = Nowhere;

}

std::string Way::toString () const
{
        switch ( this->way )
        {
                case South:             return "south" ;
                case West:              return "west" ;
                case North:             return "north" ;
                case East:              return "east" ;

                case Northeast:         return "northeast" ;
                case Southeast:         return "southeast" ;
                case Southwest:         return "southwest" ;
                case Northwest:         return "northwest" ;
                case Eastnorth:         return "eastnorth" ;
                case Eastsouth:         return "eastsouth" ;
                case Westnorth:         return "westnorth" ;
                case Westsouth:         return "westsouth" ;

                case Up:                return "up" ;
                case Down:              return "down" ;

                case ByTeleport:        return "via teleport" ;
                case ByTeleportToo:     return "via second teleport" ;

                case NoExit:            return "no exit" ;

                case Restart:           return "rebuild room" ;

                case JustWait:          return "just wait" ;

                case Nowhere:           return "nowhere" ;
        }

        return "unknown way" ;
}

}

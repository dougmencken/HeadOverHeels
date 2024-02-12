
#include "Way.hpp"


Way::Way( const std::string & stringOfWay )
{
        if ( stringOfWay == "north" )
                way = North ;
        else if ( stringOfWay == "south" )
                way = South ;
        else if ( stringOfWay == "east" )
                way = East ;
        else if ( stringOfWay == "west" )
                way = West ;
        else if ( stringOfWay == "northeast" )
                way = Northeast ;
        else if ( stringOfWay == "southeast" )
                way = Southeast ;
        else if ( stringOfWay == "southwest" )
                way = Southwest ;
        else if ( stringOfWay == "northwest" )
                way = Northwest ;
        else if ( stringOfWay == "eastnorth" )
                way = Eastnorth ;
        else if ( stringOfWay == "eastsouth" )
                way = Eastsouth ;
        else if ( stringOfWay == "westnorth" )
                way = Westnorth ;
        else if ( stringOfWay == "westsouth" )
                way = Westsouth ;
        else if ( stringOfWay == "above" )
                way = Above ;
        else if ( stringOfWay == "below" )
                way = Below ;
        else if ( stringOfWay == "via teleport" )
                way = ByTeleport ;
        else if ( stringOfWay == "via second teleport" )
                way = ByTeleportToo ;
        else if ( stringOfWay == "did not quit" )
                way = DidNotQuit ;
        else if ( stringOfWay == "rebuild room" || stringOfWay == "restart room" )
                way = RestartRoom ;
        else if ( stringOfWay == "just wait" )
                way = JustWait ;
        else
                way = Nowhere ;

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

                case Above:             return "above" ;
                case Below:             return "below" ;

                case ByTeleport:        return "via teleport" ;
                case ByTeleportToo:     return "via second teleport" ;

                case DidNotQuit:        return "did not quit" ;

                case RestartRoom:       return "rebuild room" ;

                case JustWait:          return "just wait" ;

                case Nowhere:           return "nowhere" ;
        }

        return "unknown way" ;
}

/* static */
std::string Way::exitToEntry ( const std::string & wayOfExit )
{
        switch ( Way( wayOfExit ).getIntegerOfWay () )
        {
                case Northeast :
                case Northwest :
                case North :            return "south" ;

                case Southeast :
                case Southwest :
                case South :            return "north" ;

                case Eastnorth :
                case Eastsouth :
                case East :             return "west" ;

                case Westnorth :
                case Westsouth :
                case West :             return "east" ;

                case Above :            return "below" ;

                case Below :            return "above" ;

                ///case ByTeleport :       return "via teleport" ;

                ///case ByTeleportToo :    return "via second teleport" ;
        }

        // here it is either "via teleport" / "via second teleport" or maybe some other way
        // just return that way of exit as the way of entry
        return wayOfExit ;
}

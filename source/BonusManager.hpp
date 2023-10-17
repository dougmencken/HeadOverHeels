// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef BonusManager_hpp_
#define BonusManager_hpp_

#include <map>
#include <string>


namespace iso
{

/**
 * Manage the presence or absence of the bonuses in the game's rooms.
 * The bonuses provide lives and other powerups to characters, in addition to the crowns
 * that are the key elements of the game. A bonus may be absent in the room due to being
 * taken, and if a bonus is taken from a certain room it wouldn’t appear there again
 */

class BonusManager
{

private:

        BonusManager( ) { }

public:

        virtual ~BonusManager( ) {  absentBonuses.clear() ;  }

        static BonusManager & getInstance () ;

        void markBonusAsAbsent ( const std::string& room, const std::string& label ) ;

       /**
        * Is a certain bonus in this room absent
        */
        bool isAbsent ( const std::string& room, const std::string& label ) ;

        void fillAbsentBonuses ( std::multimap < std::string /* room */, std::string /* bonus */ > & bonusesInRooms ) ;

       /**
        * Each bonus in every room is not absent after that
        */
        void reset () {  absentBonuses.clear() ;  }

private:

        std::multimap < std::string /* room */, std::string /* bonus */ > absentBonuses ;

        static BonusManager * instance ;

};

}

#endif

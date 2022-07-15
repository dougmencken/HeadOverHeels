// The free and open source remake of Head over Heels
//
// Copyright © 2022 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
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
 * Manage the presence or absence of bonuses in rooms. Bonuses are elements
 * that provide lives and other powerups to players, in addition to the crowns
 * that are key elements of the game. Bonus may be absent in the room due to being
 * picked up, when a bonus is taken from a particular room it wouldn’t appear again
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

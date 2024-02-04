// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef BonusManager_hpp_
#define BonusManager_hpp_

#include <string>
#include <vector>

class BonusInRoom
{
private:

        std::string bonus ;

        std::string room ;

public:

        explicit BonusInRoom ( const std::string & whichBonus, const std::string & inWhichRoom )
                : bonus ( whichBonus )
                , room ( inWhichRoom )
        {}

        BonusInRoom( const BonusInRoom & toCopy )
                : bonus ( toCopy.bonus )
                ,  room ( toCopy.room )
        {}

        const std::string & getBonus () const {  return bonus ;  }

        const std::string & getRoom () const {  return room ;  }

        bool equals ( const BonusInRoom & that ) const
        {
                return this->bonus == that.bonus && this->room == that.room ;
        }

        bool operator == ( const BonusInRoom & toCompare ) const {  return   equals( toCompare ) ;  }
        bool operator != ( const BonusInRoom & toCompare ) const {  return ! equals( toCompare ) ;  }
} ;

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

        static BonusManager & getInstance () ;

        void markAsAbsent ( const BonusInRoom & bonusTakenInRoom ) ;

        /**
         * Is the bonus absent in the room
         */
        bool isAbsent ( const BonusInRoom & bonusInRoom ) const ;

        const std::vector < BonusInRoom > & getAllTakenBonuses () const {  return takenBonuses ;  }

        void clearAbsentBonuses () {  takenBonuses.clear() ;  }

        virtual ~BonusManager( ) {  clearAbsentBonuses () ;  }

private:

        std::vector < BonusInRoom > takenBonuses ;

        static BonusManager * instance ;

} ;

#endif

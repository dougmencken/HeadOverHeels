
#include "BonusManager.hpp"

#include "util.hpp"


BonusManager * BonusManager::instance = nilPointer ;

BonusManager& BonusManager::getInstance ()
{
        if ( instance == nilPointer )
        {
                instance = new BonusManager();
        }

        return *instance;
}

void BonusManager::markBonusAsAbsent ( const std::string& room, const std::string& label )
{
        if ( ! isAbsent( room, label ) )
        {
                absentBonuses.insert( std::pair< std::string, std::string >( room, label ) );
        }
}

bool BonusManager::isAbsent ( const std::string& room, const std::string& label )
{
        for ( std::multimap< std::string, std::string >::const_iterator b = absentBonuses.begin () ; b != absentBonuses.end () ; ++ b )
        {
                if ( room == b->first && label == b->second ) return true ;
        }

        return false ;
}

void BonusManager::fillAbsentBonuses ( std::multimap < std::string /* room */, std::string /* bonus */ > & bonusesInRooms )
{
        for ( std::multimap< std::string, std::string >::const_iterator b = absentBonuses.begin (); b != absentBonuses.end (); ++ b )
        {
                bonusesInRooms.insert( std::pair< std::string, std::string >( b->first, b->second ) );
        }
}

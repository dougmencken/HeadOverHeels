
#include "BonusManager.hpp"

#include "util.hpp"


BonusManager * BonusManager::instance = nilPointer ;

BonusManager& BonusManager::getInstance ()
{
        if ( instance == nilPointer ) instance = new BonusManager();

        return *instance;
}

void BonusManager::markAsAbsent ( const BonusInRoom & bonusTakenInRoom )
{
        if ( ! isAbsent( bonusTakenInRoom ) )
                takenBonuses.push_back( bonusTakenInRoom );
}

bool BonusManager::isAbsent ( const BonusInRoom & bonusTakenInRoom ) const
{
        for ( unsigned int i = 0 ; i < takenBonuses.size () ; ++ i )
                if ( takenBonuses[ i ].equals( bonusTakenInRoom ) ) return true ;

        return false ;
}

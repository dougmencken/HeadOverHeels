
#include "TransparencyManager.hpp"


namespace isomot
{

TransparencyManager::TransparencyManager()
{
        for ( unsigned int i = 0; i <= 101; i++ )
        {
                this->table.push_back( 0 ) ;
        }
}

TransparencyManager::~TransparencyManager()
{
        this->table.clear() ;
}

void TransparencyManager::add( const unsigned char amount )
{
        if ( amount <= 100 )
        {
                this->table[ amount ]++ ;
        }
}

void TransparencyManager::remove( const unsigned char amount )
{
        if ( amount <= 100 )
        {
                if ( this->table[ amount ] > 0 )
                {
                        this->table[ amount ]-- ;
                }
        }
}

}

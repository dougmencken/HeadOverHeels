
#include "TableOfTransparencies.hpp"


namespace isomot
{

TableOfTransparencies::TableOfTransparencies()
{
        for ( unsigned int i = 0; i <= 101; i++ )
        {
                this->table.push_back( 0 ) ;
        }
}

TableOfTransparencies::~TableOfTransparencies()
{
        this->table.clear() ;
}

void TableOfTransparencies::addToDegreeOfTransparency( const unsigned char amount )
{
        if ( amount <= 100 )
        {
                this->table[ amount ]++ ;
        }
}

void TableOfTransparencies::removeFromDegreeOfTransparency( const unsigned char amount )
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

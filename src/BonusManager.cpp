
#include "BonusManager.hpp"
#include "Ism.hpp"


namespace isomot
{

BonusManager::BonusManager( )
{

}

BonusManager::~BonusManager( )
{
        bonusRooms.clear();
}

BonusManager * BonusManager::instance = nilPointer ;

BonusManager* BonusManager::getInstance ()
{
        if ( instance == nilPointer )
        {
                instance = new BonusManager();
        }

        return instance;
}

void BonusManager::markBonusAsAbsent ( const std::string& fileName, const std::string& label )
{
        // search in bonus rooms
        std::list< BonusRoom >::iterator result = std::find( bonusRooms.begin (), bonusRooms.end (), fileName );

        if ( result != bonusRooms.end () )
        {
                // room is found so just mark this bonus as absent here
                ( *result ).markBonusAsAbsent( label ) ;
        }
        else
        {
                // otherwise create new record for this room and then mark this bonus as absent
                BonusRoom bonusRoom( fileName ) ;
                bonusRoom.markBonusAsAbsent( label ) ;
                bonusRooms.push_back( bonusRoom ) ;
        }
}

bool BonusManager::isPresent ( const std::string& fileName, const std::string& label )
{
        std::list< BonusRoom >::iterator result = std::find( bonusRooms.begin (), bonusRooms.end (), fileName ) ;

        // the bonus is here when this room is absent or when the bonus is not in bonus room
        return ( result == bonusRooms.end () || ( result != bonusRooms.end () && ! ( *result ).isPresent( label ) ) );
}

void BonusManager::load ( const sgxml::bonus::room_sequence& roomSequence )
{
        for ( sgxml::bonus::room_const_iterator i = roomSequence.begin (); i != roomSequence.end (); ++i )
        {
                this->markBonusAsAbsent( ( *i ).filename(), ( *i ).label() );
        }
}

void BonusManager::save ( sgxml::bonus::room_sequence& roomSequence )
{
        for ( std::list< BonusRoom >::iterator r = this->bonusRooms.begin (); r !=this->bonusRooms.end (); ++r )
        {
                for ( std::list< std::string >::iterator b = ( *r ).getBonusItems().begin (); b != ( *r ).getBonusItems().end (); ++b )
                {
                        roomSequence.push_back( sgxml::room( *b, ( *r ).getNameOfFile() ) );
                }
        }
}

void BonusManager::reset ()
{
        this->bonusRooms.clear();
}

BonusRoom::BonusRoom( const std::string& name )
        : nameOfFile( name )
{

}

BonusRoom::~BonusRoom( )
{
        bonusItems.clear();
}

void BonusRoom::markBonusAsAbsent ( const std::string& label )
{
        bonusItems.push_back( label ) ;
}

bool BonusRoom::isPresent ( const std::string& label )
{
        return std::find( bonusItems.begin (), bonusItems.end (), label ) != bonusItems.end () ;
}

bool BonusRoom::operator== ( const std::string& fileName )
{
        return ( this->nameOfFile == fileName ) ;
}

}

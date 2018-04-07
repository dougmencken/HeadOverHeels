
#include "BonusManager.hpp"
#include "Ism.hpp"

#include <algorithm>


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

void BonusManager::markBonusAsAbsent ( const std::string& roomFile, const std::string& label )
{
	// search in bonus rooms
	std::list< BonusRoom >::iterator result = std::find( bonusRooms.begin (), bonusRooms.end (), roomFile );

	if ( result != bonusRooms.end () )
	{
		// room is found so just mark this bonus as absent here
		( *result ).markBonusAsAbsent( label ) ;
	}
	else
	{
		// otherwise create new record for this room and then mark this bonus as absent
		BonusRoom bonusRoom( roomFile ) ;
		bonusRoom.markBonusAsAbsent( label ) ;
		bonusRooms.push_back( bonusRoom ) ;
	}
}

bool BonusManager::isPresent ( const std::string& roomFile, const std::string& label )
{
	std::list< BonusRoom >::iterator result = std::find( bonusRooms.begin (), bonusRooms.end (), roomFile ) ;

	// the bonus is here when this room is absent or when the bonus is not in bonus room
	return ( result == bonusRooms.end () || ( result != bonusRooms.end () && ! ( *result ).isPresent( label ) ) );
}

void BonusManager::parseAbsentBonuses ( const std::multimap < std::string /* room */, std::string /* bonus */ > & bonusesInRooms )
{
	for ( std::multimap< std::string, std::string >::const_iterator b = bonusesInRooms.begin () ; b != bonusesInRooms.end () ; ++ b )
	{
		this->markBonusAsAbsent( ( *b ).first, ( *b ).second );
	}
}

void BonusManager::fillAbsentBonuses ( std::multimap < std::string /* room */, std::string /* bonus */ > & bonusesInRooms )
{
	for ( std::list< BonusRoom >::iterator r = this->bonusRooms.begin (); r !=this->bonusRooms.end (); ++r )
	{
		for ( std::list< std::string >::iterator b = ( *r ).getBonusItems().begin (); b != ( *r ).getBonusItems().end (); ++ b )
		{
			bonusesInRooms.insert( std::pair< std::string, std::string >( ( *r ).getNameOfFile(), *b ) );
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

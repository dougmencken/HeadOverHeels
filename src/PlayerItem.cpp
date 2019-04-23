
#include "PlayerItem.hpp"
#include "Behavior.hpp"
#include "DescriptionOfItem.hpp"
#include "Door.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "GameManager.hpp"
#include "UserControlled.hpp"

#include <algorithm> // std::find


namespace iso
{

PlayerItem::PlayerItem( const DescriptionOfItem* description, int x, int y, int z, const std::string& way )
        : FreeItem( description, x, y, z, way )
        , lives( 0 )
        , highSpeed( 0 )
        , highJumps( 0 )
        , howManyDoughnuts( 0 )
        , wayOfExit( "no exit" )
        , wayOfEntry( "just wait" )
        , shieldTimer( new Timer () )
        , shieldRemaining( 0.0 )
        , descriptionOfTakenItem( nilPointer )
{
}

PlayerItem::PlayerItem( const PlayerItem& playerItem )
        : FreeItem( playerItem )
        , lives( playerItem.lives )
        , highSpeed( playerItem.highSpeed )
        , highJumps( playerItem.highJumps )
        , tools( playerItem.tools )
        , howManyDoughnuts( playerItem.howManyDoughnuts )
        , wayOfExit( playerItem.wayOfExit )
        , wayOfEntry( playerItem.wayOfEntry )
        , shieldTimer( new Timer () )
        , shieldRemaining( 0.0 )
        , descriptionOfTakenItem( nilPointer )
{
}

void PlayerItem::setWayOfExit ( const std::string& way )
{
        this->wayOfExit = way;

        switch ( Way( way ).getIntegerOfWay () )
        {
                case Way::North:
                case Way::South:
                case Way::East:
                case Way::West:
                        setOrientation( way );
                        break;

                case Way::Northeast:
                case Way::Northwest:
                        setOrientation( "north" );
                        break;

                case Way::Southeast:
                case Way::Southwest:
                        setOrientation( "south" );
                        break;

                case Way::Eastnorth:
                case Way::Eastsouth:
                        setOrientation( "east" );
                        break;

                case Way::Westnorth:
                case Way::Westsouth:
                        setOrientation( "west" );
                        break;

                default:
                        ;
        }
}

void PlayerItem::autoMoveOnEntry ( const std::string& wayOfEntry )
{
        setWayOfEntry( wayOfEntry );

        if ( getBehavior() == nilPointer )
        {
                std::cerr << "nil behavior at PlayerItem::autoMoveOnEntry" << std::endl ;
                return;
        }

        switch ( Way( wayOfEntry ).getIntegerOfWay () )
        {
                case Way::North:
                case Way::Northeast:
                case Way::Northwest:
                        getBehavior()->changeActivityOfItem( Activity::AutoMoveSouth );
                        break;

                case Way::South:
                case Way::Southeast:
                case Way::Southwest:
                        getBehavior()->changeActivityOfItem( Activity::AutoMoveNorth );
                        break;

                case Way::East:
                case Way::Eastnorth:
                case Way::Eastsouth:
                        getBehavior()->changeActivityOfItem( Activity::AutoMoveWest );
                        break;

                case Way::West:
                case Way::Westnorth:
                case Way::Westsouth:
                        getBehavior()->changeActivityOfItem( Activity::AutoMoveEast );
                        break;

                case Way::ByTeleport:
                case Way::ByTeleportToo:
                        getBehavior()->changeActivityOfItem( Activity::BeginWayInTeletransport );
                        break;

                case Way::Above:
                        getBehavior()->changeActivityOfItem( Activity::Fall );
                        break;

                default:
                        ;
        }
}

bool PlayerItem::addToPosition( int x, int y, int z )
{
        bool itAutomoves = getBehavior()->getActivityOfItem() == Activity::AutoMoveNorth ||
                                getBehavior()->getActivityOfItem() == Activity::AutoMoveSouth ||
                                getBehavior()->getActivityOfItem() == Activity::AutoMoveEast ||
                                getBehavior()->getActivityOfItem() == Activity::AutoMoveWest ;

        bool itFallsUnderDoor = ( x == 0 && y == 0 && z < 0 && this->isUnderSomeDoor () );

        if ( ! this->isActiveCharacter() && ! itAutomoves && ! itFallsUnderDoor )
        {
                return FreeItem::addToPosition( x, y, z );
        }

        mediator->clearStackOfCollisions( );

        int xBefore = xYet ;
        int yBefore = yYet ;
        int zBefore = zYet ;

        std::pair< int, int > offsetBefore = getOffset() ;

        xYet += x ;
        yYet += y ;
        zYet += z ;

        // look for collision with door

        bool doorCollision = false;

        bool collisionFound = mediator->lookForCollisionsOf( this->getUniqueName() );

        if ( collisionFound )
        {
                while ( ! mediator->isStackOfCollisionsEmpty () )
                {
                        std::string what = mediator->popCollision();

                        // case of move to north wall
                        if ( x < 0 )
                        {
                                // check for hit of north door’s jamb
                                doorCollision = isCollidingWithDoor( "north", what, xBefore, yBefore );
                                if ( ! doorCollision )
                                {
                                        // check for hit of northeast door’s jamb
                                        doorCollision = isCollidingWithDoor( "northeast", what, xBefore, yBefore );
                                        if ( ! doorCollision )
                                        {
                                                // then it’s hit of northwest door’s jamb
                                                doorCollision = isCollidingWithDoor( "northwest", what, xBefore, yBefore );
                                        }
                                }
                        }
                        // case of move to south wall
                        else if ( x > 0 )
                        {
                                // check for hit of south door’s jamb
                                doorCollision = isCollidingWithDoor( "south", what, xBefore, yBefore );
                                if ( ! doorCollision )
                                {
                                        // check for hit of southeast door’s jamb
                                        doorCollision = isCollidingWithDoor( "southeast", what, xBefore, yBefore );
                                        if ( ! doorCollision )
                                        {
                                                // then it’s hit of southwest door’s jamb
                                                doorCollision = isCollidingWithDoor( "southwest", what, xBefore, yBefore );
                                        }
                                }
                        }
                        // case of move to east wall
                        else if ( y < 0 )
                        {
                                doorCollision = isCollidingWithDoor( "east", what, xBefore, yBefore );
                                // check for hit of east door’s jamb
                                if ( ! doorCollision )
                                {
                                        // check for hit of east-north door’s jamb
                                        doorCollision = isCollidingWithDoor( "eastnorth", what, xBefore, yBefore );
                                        if ( ! doorCollision )
                                        {
                                                // so it’s hit of east-south door’s jamb
                                                doorCollision = isCollidingWithDoor( "eastsouth", what, xBefore, yBefore );
                                        }
                                }
                        }
                        // case of move to west wall
                        else if ( y > 0 )
                        {
                                doorCollision = isCollidingWithDoor( "west", what, xBefore, yBefore );
                                // check for hit of west door’s jamb
                                if ( ! doorCollision )
                                {
                                        // check for hit of west-north door’s jamb
                                        doorCollision = isCollidingWithDoor( "westnorth", what, xBefore, yBefore );
                                        if ( ! doorCollision )
                                        {
                                                // so it’s hit of west-south door’s jamb
                                                doorCollision = isCollidingWithDoor( "westsouth", what, xBefore, yBefore );
                                        }
                                }
                        }
                }
        }

        // look for collision with real wall, one which limits the room
        if ( this->getX() < mediator->getRoom()->getLimitAt( "north" )
                        && isNotUnderDoorAt( "north" ) && isNotUnderDoorAt( "northeast" ) && isNotUnderDoorAt( "northwest" ) )
        {
                mediator->pushCollision( "some segment of wall at north" );
        }
        else if ( this->getX() + static_cast< int >( getDescriptionOfItem()->getWidthX() ) > mediator->getRoom()->getLimitAt( "south" )
                        && isNotUnderDoorAt( "south" ) && isNotUnderDoorAt( "southeast" ) && isNotUnderDoorAt( "southwest" ) )
        {
                mediator->pushCollision( "some segment of wall at south" );
        }

        if ( this->getY() - static_cast< int >( getDescriptionOfItem()->getWidthY() ) + 1 < mediator->getRoom()->getLimitAt( "east" )
                        && isNotUnderDoorAt( "east" ) && isNotUnderDoorAt( "eastnorth" ) && isNotUnderDoorAt( "eastsouth" ) )
        {
                mediator->pushCollision( "some segment of wall at east" );
        }
        else if ( this->getY() >= mediator->getRoom()->getLimitAt( "west" )
                        && isNotUnderDoorAt( "west" ) && isNotUnderDoorAt( "westnorth" ) && isNotUnderDoorAt( "westsouth" ) )
        {
                mediator->pushCollision( "some segment of wall at west" );
        }

        collisionFound = ! mediator->isStackOfCollisionsEmpty ();
        if ( ! collisionFound )
        {
                // now it is known that the player may go thru a door
                // look for collision with limits of room

                const std::string doors[ 12 ] =
                        {  "northeast", "northwest", "north", "southeast", "southwest", "south",
                                "eastnorth", "eastsouth", "east", "westnorth", "westsouth", "west"  };

                // check each limit of room
                for ( unsigned int i = 0; i < 12; i++ )
                {
                        if ( isCollidingWithLimitOfRoom( doors[ i ] ) )
                        {
                                mediator->pushCollision( doors[ i ] + " limit" );
                                break;
                        }
                }

                // collision with floor
                if ( this->getZ() < 0 )
                {
                        mediator->pushCollision( "some tile of floor" );
                }

                // collision with ceiling
                if ( z >= 0 && this->getZ() > ( MaxLayers - 1 ) * LayerHeight + ( LayerHeight >> 1 ) )
                {
                        mediator->pushCollision( "ceiling" );
                }

                collisionFound = ! mediator->isStackOfCollisionsEmpty ();
                if ( ! collisionFound )
                {
                        // look for collision with the rest of items in room
                        collisionFound = mediator->lookForCollisionsOf( this->getUniqueName() );
                        if ( ! collisionFound )
                        {
                                // reshade and remask
                                freshBothProcessedImages();
                                setWantShadow( true );
                                setWantMaskTrue();

                                // update offset to point of room’s origin
                                calculateOffset ();

                                // mark to mask free items whose images overlap with character’s image
                                mediator->wantToMaskWithFreeItemAt( *this, offsetBefore );
                                mediator->wantToMaskWithFreeItem( *this );

                                // reshade items
                                mediator->wantShadowFromFreeItemAt( *this, xBefore, yBefore, zBefore );
                                mediator->wantShadowFromFreeItem( *this );

                                // mark to sort container of free items
                                mediator->markToSortFreeItems ();
                        }
                }
        }

        if ( collisionFound && ! doorCollision )
        {
                // restore previous values
                xYet = xBefore ;
                yYet = yBefore ;
                zYet = zBefore ;

                setOffset( offsetBefore );
        }

        return ! collisionFound ;
}

bool PlayerItem::isCollidingWithLimitOfRoom( const std::string& onWhichWay )
{
        bool result = false;

        Door* door = mediator->getRoom()->getDoorAt( onWhichWay );

        switch ( Way( onWhichWay ).getIntegerOfWay () )
        {
                case Way::North:
                        result = ( this->getX() < 0 );
                        break;

                case Way::Northeast:
                case Way::Northwest:
                        result = ( door != nilPointer &&
                                        this->getX() < mediator->getRoom()->getLimitAt( onWhichWay ) &&
                                        door->isUnderDoor( this->getX(), this->getY(), this->getZ() ) );
                        break;

                case Way::South:
                        result = ( this->getX() + static_cast< int >( getDescriptionOfItem()->getWidthX() )
                                        > static_cast< int >( mediator->getRoom()->getTilesX() * mediator->getRoom()->getSizeOfOneTile() ) );
                        break;

                case Way::Southeast:
                case Way::Southwest:
                        result = ( door != nilPointer &&
                                        this->getX() + static_cast< int >( getDescriptionOfItem()->getWidthX() ) > mediator->getRoom()->getLimitAt( onWhichWay ) &&
                                        door->isUnderDoor( this->getX(), this->getY(), this->getZ() ) );
                        break;

                case Way::East:
                        result = ( this->getY() - static_cast< int >( getDescriptionOfItem()->getWidthY() ) + 1 < 0 );
                        break;

                case Way::Eastnorth:
                case Way::Eastsouth:
                        result = ( door != nilPointer &&
                                        this->getY() - static_cast< int >( getDescriptionOfItem()->getWidthY() ) + 1 < mediator->getRoom()->getLimitAt( onWhichWay ) &&
                                        door->isUnderDoor( this->getX(), this->getY(), this->getZ() ) );
                        break;

                case Way::West:
                        result = ( this->getY()
                                        >= static_cast< int >( mediator->getRoom()->getTilesY() * mediator->getRoom()->getSizeOfOneTile() ) );
                        break;

                case Way::Westnorth:
                case Way::Westsouth:
                        result = ( door != nilPointer &&
                                        this->getY() + static_cast< int >( getDescriptionOfItem()->getWidthY() ) > mediator->getRoom()->getLimitAt( onWhichWay ) &&
                                        door->isUnderDoor( this->getX(), this->getY(), this->getZ() ) );
                        break;

                default:
                        ;
        }

        return result;
}

void PlayerItem::behave ()
{
        UserControlled* userBehavior = dynamic_cast< UserControlled* >( getBehavior () );

        if ( userBehavior != nilPointer )
                userBehavior->behave ( );
}

bool PlayerItem::update ()
{
        if ( getBehavior() != nilPointer )
                getBehavior()->update ( );

        return false;
}

void PlayerItem::wait ()
{
        ActivityOfItem activity = getBehavior()->getActivityOfItem();

        // don’t wait while teleporting or loosing life
        if ( activity != Activity::BeginWayOutTeletransport && activity != Activity::WayOutTeletransport &&
                activity != Activity::BeginWayInTeletransport && activity != Activity::WayInTeletransport &&
                activity != Activity::MeetMortalItem && activity != Activity::Vanish )
        {
                // set waiting frame by orientation of item
                changeFrame( firstFrame () );

                getBehavior()->changeActivityOfItem( Activity::Wait );
        }
}

bool PlayerItem::isActiveCharacter () const
{
        return mediator->getActiveCharacter()->getUniqueName() == this->getUniqueName() ;
}

void PlayerItem::fillWithData( const GameManager & data )
{
        setLives( data.getLives( getLabel() ) );

        setHighJumps( data.getHighJumps() );
        setHighSpeed( data.getHighSpeed() );
        setShieldTime( data.getShield( getLabel() ) );
        data.fillToolsOwnedByCharacter( getLabel(), this->tools );
        setDoughnuts( data.getDonuts( getLabel() ) );
}

void PlayerItem::addLives( unsigned char lives )
{
        if ( this->lives < 100 )
        {
                this->lives += lives;
                GameManager::getInstance().addLives( this->getLabel (), lives );
        }
}

void PlayerItem::loseLife()
{
        setWayOfExit( "rebuild room" );

        if ( this->lives > 0 )
        {
                this->lives--;
                GameManager::getInstance().loseLife( getOriginalLabel () /* current label is "bubbles" */ );
        }

        GameManager::getInstance().emptyHandbag();
}

void PlayerItem::takeTool( const std::string& label )
{
        this->tools.push_back( label );
        GameManager::getInstance().takeMagicItem( label );
}

void PlayerItem::addDoughnuts( const unsigned short howMany )
{
        howManyDoughnuts += howMany;
        GameManager::getInstance().setDonuts( howManyDoughnuts );
}

void PlayerItem::useDoughnut()
{
        if ( howManyDoughnuts > 0 )
        {
                howManyDoughnuts-- ;
                GameManager::getInstance().consumeDonut();
        }
}

void PlayerItem::activateHighSpeed()
{
        if ( getOriginalLabel() == "head" )
        {
                highSpeed = 99 ;
                GameManager::getInstance().addHighSpeed( getOriginalLabel(), highSpeed );
        }
}

void PlayerItem::decreaseHighSpeed()
{
        if ( highSpeed > 0 )
        {
                highSpeed-- ;
                GameManager::getInstance().decreaseHighSpeed( getOriginalLabel() );
        }
}

void PlayerItem::addHighJumps( unsigned char howMany )
{
        if ( getOriginalLabel() == "heels" )
        {
                highJumps += howMany;
                GameManager::getInstance().addHighJumps( getOriginalLabel(), highJumps );
        }
}

void PlayerItem::decreaseHighJumps()
{
        if ( highJumps > 0 )
        {
                highJumps-- ;
                GameManager::getInstance().decreaseHighJumps( getOriginalLabel() );
        }
}

void PlayerItem::activateShield()
{
        shieldTimer->reset();
        shieldTimer->go();
        shieldRemaining = 25.0;
        GameManager::getInstance().addShield( getOriginalLabel(), shieldRemaining );
}

void PlayerItem::decreaseShield()
{
        shieldRemaining = 25.0 - shieldTimer->getValue();

        if ( shieldRemaining < 0 )
        {
                shieldRemaining = 0.0;
                shieldTimer->stop();
        }

        GameManager::getInstance().modifyShield( getOriginalLabel(), shieldRemaining );
}

void PlayerItem::liberatePlanet ()
{
        std::string scenery = this->mediator->getRoom()->getScenery();

        if ( scenery == "jail" || scenery == "blacktooth" || scenery == "market" )
        {
                GameManager::getInstance().liberatePlanet( "blacktooth" );
        }
        else if ( scenery == "egyptus" )
        {
                GameManager::getInstance().liberatePlanet( "egyptus" );
        }
        else if ( scenery == "penitentiary" )
        {
                GameManager::getInstance().liberatePlanet( "penitentiary" );
        }
        else if ( scenery == "safari" )
        {
                GameManager::getInstance().liberatePlanet( "safari" );
        }
        else if ( scenery == "byblos" )
        {
                GameManager::getInstance().liberatePlanet( "byblos" );
        }
}

void PlayerItem::placeItemInBag ( const std::string& labelOfItem, const std::string& behavior )
{
        const DescriptionOfItem* description = getDescriptionOfItem()->getItemDescriptions()->getDescriptionByLabel( labelOfItem ) ;
        this->descriptionOfTakenItem = description ;
        this->behaviorOfTakenItem = behavior ;
}

void PlayerItem::save ()
{
        GameManager::getInstance().eatFish( *this, this->mediator->getRoom() );
}

void PlayerItem::saveAt ( int x, int y, int z )
{
        GameManager::getInstance().eatFish( *this, this->mediator->getRoom(), x, y, z );
}

void PlayerItem::setShieldTime ( float seconds )
{
        shieldTimer->reset();

        if ( seconds > 0 && ! hasShield() )
        {
                shieldTimer->go();
        }

        shieldRemaining = seconds;
}

bool PlayerItem::hasTool( const std::string& label ) const
{
        return std::find( this->tools.begin (), this->tools.end (), label ) != this->tools.end ();
}

}

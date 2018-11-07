
#include "PlayerItem.hpp"
#include "Behavior.hpp"
#include "ItemData.hpp"
#include "ItemDataManager.hpp"
#include "Door.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "GameManager.hpp"
#include "UserControlled.hpp"

#include <algorithm> // std::find


namespace iso
{

PlayerItem::PlayerItem( const ItemData* itemData, int x, int y, int z, const Way& way )
        : FreeItem( itemData, x, y, z, way )
        , lives( 0 )
        , highSpeed( 0 )
        , highJumps( 0 )
        , howManyDoughnuts( 0 )
        , wayOfExit( "no exit" )
        , wayOfEntry( "just wait" )
        , shieldTimer( new Timer () )
        , shieldRemaining( 0.0 )
        , takenItemData( nilPointer )
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
        , takenItemData( nilPointer )
{

}

PlayerItem::~PlayerItem()
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
                        setOrientation( Way( "north" ) );
                        break;

                case Way::Southeast:
                case Way::Southwest:
                        setOrientation( Way( "south" ) );
                        break;

                case Way::Eastnorth:
                case Way::Eastsouth:
                        setOrientation( Way( "east" ) );
                        break;

                case Way::Westnorth:
                case Way::Westsouth:
                        setOrientation( Way( "west" ) );
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

                case Way::Up:
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

        // copy item before moving it
        PlayerItemPtr copyOfItem ( new PlayerItem ( *this ) );

        this->xPos += x;
        this->yPos += y;
        this->zPos += z;

        // look for collision with door

        bool doorCollision = false;

        bool collisionFound = mediator->lookForCollisionsOf( this->getUniqueName() );

        if ( collisionFound )
        {
                while ( ! mediator->isStackOfCollisionsEmpty () )
                {
                        std::string what = mediator->popCollision();

                        int oldX = copyOfItem->getX();
                        int oldY = copyOfItem->getY();

                        // case of move to north wall
                        if ( x < 0 )
                        {
                                // check for hit of north door’s jamb
                                doorCollision = isCollidingWithDoor( "north", what, oldX, oldY );
                                if ( ! doorCollision )
                                {
                                        // check for hit of northeast door’s jamb
                                        doorCollision = isCollidingWithDoor( "northeast", what, oldX, oldY );
                                        if ( ! doorCollision )
                                        {
                                                // then it’s hit of northwest door’s jamb
                                                doorCollision = isCollidingWithDoor( "northwest", what, oldX, oldY );
                                        }
                                }
                        }
                        // case of move to south wall
                        else if ( x > 0 )
                        {
                                // check for hit of south door’s jamb
                                doorCollision = isCollidingWithDoor( "south", what, oldX, oldY );
                                if ( ! doorCollision )
                                {
                                        // check for hit of southeast door’s jamb
                                        doorCollision = isCollidingWithDoor( "southeast", what, oldX, oldY );
                                        if ( ! doorCollision )
                                        {
                                                // then it’s hit of southwest door’s jamb
                                                doorCollision = isCollidingWithDoor( "southwest", what, oldX, oldY );
                                        }
                                }
                        }
                        // case of move to east wall
                        else if ( y < 0 )
                        {
                                doorCollision = isCollidingWithDoor( "east", what, oldX, oldY );
                                // check for hit of east door’s jamb
                                if ( ! doorCollision )
                                {
                                        // check for hit of east-north door’s jamb
                                        doorCollision = isCollidingWithDoor( "eastnorth", what, oldX, oldY );
                                        if ( ! doorCollision )
                                        {
                                                // so it’s hit of east-south door’s jamb
                                                doorCollision = isCollidingWithDoor( "eastsouth", what, oldX, oldY );
                                        }
                                }
                        }
                        // case of move to west wall
                        else if ( y > 0 )
                        {
                                doorCollision = isCollidingWithDoor( "west", what, oldX, oldY );
                                // check for hit of west door’s jamb
                                if ( ! doorCollision )
                                {
                                        // check for hit of west-north door’s jamb
                                        doorCollision = isCollidingWithDoor( "westnorth", what, oldX, oldY );
                                        if ( ! doorCollision )
                                        {
                                                // so it’s hit of west-south door’s jamb
                                                doorCollision = isCollidingWithDoor( "westsouth", what, oldX, oldY );
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
        else if ( this->getX() + static_cast< int >( getDataOfItem()->getWidthX() ) > mediator->getRoom()->getLimitAt( "south" )
                        && isNotUnderDoorAt( "south" ) && isNotUnderDoorAt( "southeast" ) && isNotUnderDoorAt( "southwest" ) )
        {
                mediator->pushCollision( "some segment of wall at south" );
        }

        if ( this->getY() - static_cast< int >( getDataOfItem()->getWidthY() ) + 1 < mediator->getRoom()->getLimitAt( "east" )
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

                collisionFound = ! mediator->isStackOfCollisionsEmpty ();
                if ( ! collisionFound )
                {
                        // look for collision with the rest of items in room
                        collisionFound = mediator->lookForCollisionsOf( this->getUniqueName() );
                        if ( ! collisionFound )
                        {
                                // reshade and remask
                                binBothProcessedImages();
                                setWantShadow( true );
                                setWantMaskTrue();

                                // for item with image, mark to mask free items whose images overlap with its image
                                if ( this->rawImage != nilPointer )
                                {
                                        // get how many pixels is this image from point of room’s origin
                                        this->offset.first = ( ( getX() - getY() ) << 1 ) + getWidthX() + getDataOfItem()->getWidthY() - ( this->rawImage->getWidth() >> 1 ) - 1;
                                        this->offset.second = getX() + getY() + getWidthX() - this->rawImage->getHeight() - getZ();

                                        // for both the previous position and the current position
                                        mediator->remaskWithFreeItem( *copyOfItem );
                                        mediator->remaskWithFreeItem( *this );
                                }
                                else
                                {
                                        this->offset.first = this->offset.second = 0;
                                }

                                // reshade items
                                // for both previous position and current position
                                mediator->reshadeWithFreeItem( *copyOfItem );
                                mediator->reshadeWithFreeItem( *this );

                                // mark to sort container of free items
                                mediator->needsToSortFreeItems ();
                        }
                }
        }

        // restore previous values for collision which is not collision with door
        if ( collisionFound && ! doorCollision )
        {
                this->xPos = copyOfItem->getX();
                this->yPos = copyOfItem->getY();
                this->zPos = copyOfItem->getZ();

                this->offset = copyOfItem->getOffset();
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
                        result = ( this->getX() + static_cast< int >( getDataOfItem()->getWidthX() )
                                        > static_cast< int >( mediator->getRoom()->getTilesX() * mediator->getRoom()->getSizeOfOneTile() ) );
                        break;

                case Way::Southeast:
                case Way::Southwest:
                        result = ( door != nilPointer &&
                                        this->getX() + static_cast< int >( getDataOfItem()->getWidthX() ) > mediator->getRoom()->getLimitAt( onWhichWay ) &&
                                        door->isUnderDoor( this->getX(), this->getY(), this->getZ() ) );
                        break;

                case Way::East:
                        result = ( this->getY() - static_cast< int >( getDataOfItem()->getWidthY() ) + 1 < 0 );
                        break;

                case Way::Eastnorth:
                case Way::Eastsouth:
                        result = ( door != nilPointer &&
                                        this->getY() - static_cast< int >( getDataOfItem()->getWidthY() ) + 1 < mediator->getRoom()->getLimitAt( onWhichWay ) &&
                                        door->isUnderDoor( this->getX(), this->getY(), this->getZ() ) );
                        break;

                case Way::West:
                        result = ( this->getY()
                                        >= static_cast< int >( mediator->getRoom()->getTilesY() * mediator->getRoom()->getSizeOfOneTile() ) );
                        break;

                case Way::Westnorth:
                case Way::Westsouth:
                        result = ( door != nilPointer &&
                                        this->getY() + static_cast< int >( getDataOfItem()->getWidthY() ) > mediator->getRoom()->getLimitAt( onWhichWay ) &&
                                        door->isUnderDoor( this->getX(), this->getY(), this->getZ() ) );
                        break;

                default:
                        ;
        }

        return result;
}

void PlayerItem::behave ()
{
        UserControlled* userBehavior = dynamic_cast< UserControlled* >( this->behavior );
        if ( userBehavior != nilPointer )
        {
                userBehavior->behave ( );
        }
}

bool PlayerItem::update ()
{
        if ( behavior != nilPointer )
        {
                behavior->update ( );
        }

        return false;
}

void PlayerItem::wait ()
{
        ActivityOfItem activity = this->behavior->getActivityOfItem();

        // don’t wait while teleporting or loosing life
        if ( activity != Activity::BeginWayOutTeletransport && activity != Activity::WayOutTeletransport &&
                activity != Activity::BeginWayInTeletransport && activity != Activity::WayInTeletransport &&
                activity != Activity::MeetMortalItem && activity != Activity::Vanish )
        {
                // get waiting frame by orientation of item
                unsigned int orientOccident = ( orientation.getIntegerOfWay() == Way::Nowhere ? 0 : orientation.getIntegerOfWay() );
                size_t frame = getDataOfItem()->howManyFrames() * orientOccident ;
                if ( frame >= howManyMotions() ) frame = 0;

                if ( this->rawImage != nilPointer && this->rawImage != getMotionAt( frame ) )
                {
                        changeImage( getMotionAt( frame ) );

                        if ( this->shadow != nilPointer )
                        {
                                changeShadow( getShadowAt( frame ) );
                        }
                }

                this->behavior->changeActivityOfItem( Activity::Wait );
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
        const ItemData* itemData = getDataOfItem()->getItemDataManager()->findDataByLabel( labelOfItem ) ;
        this->takenItemData = itemData;
        this->takenItemBehavior = behavior;
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

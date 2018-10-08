
#include "PlayerItem.hpp"
#include "Behavior.hpp"
#include "ItemData.hpp"
#include "Door.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "GameManager.hpp"
#include "UserControlled.hpp"

#include <algorithm> // std::find


namespace isomot
{

PlayerItem::PlayerItem( ItemData* itemData, int x, int y, int z, const Way& way )
        : FreeItem( itemData, x, y, z, way )
        , lives( 0 )
        , highSpeed( 0 )
        , highJumps( 0 )
        , shield( 0.0 )
        , howManyDoughnuts( 0 )
        , exit( "no exit" )
        , entry( "just wait" )
        , shieldTimer( nilPointer )
        , shieldTime( 25.0 )
        , takenItemData( nilPointer )
        , originalDataOfItem( itemData )
{

}

PlayerItem::PlayerItem( const PlayerItem& playerItem )
        : FreeItem( playerItem )
        , lives( playerItem.lives )
        , highSpeed( playerItem.highSpeed )
        , highJumps( playerItem.highJumps )
        , shield( playerItem.shield )
        , tools( playerItem.tools )
        , howManyDoughnuts( playerItem.howManyDoughnuts )
        , exit( playerItem.exit )
        , entry( playerItem.entry )
        , shieldTimer( nilPointer )
        , shieldTime( playerItem.shieldTime )
        , takenItemData( nilPointer )
        , originalDataOfItem( playerItem.getOriginalDataOfItem() )
{

}

PlayerItem::~PlayerItem()
{
        delete shieldTimer;
}

void PlayerItem::setWayOfExit ( const std::string& way )
{
        this->exit = way;

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

        // copy item before moving
        PlayerItem copyOfItem( *this );

        this->xPos += x;
        this->yPos += y;
        this->zPos += z;

        // look for collision with door

        bool doorCollision = false;

        bool collisionFound = mediator->findCollisionWithItem( this );

        if ( collisionFound )
        {
                while ( ! mediator->isStackOfCollisionsEmpty () )
                {
                        std::string what = mediator->popCollision();

                        int oldX = copyOfItem.getX();
                        int oldY = copyOfItem.getY();

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
                        collisionFound = mediator->findCollisionWithItem( this );
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
                                        mediator->remaskWithFreeItem( &copyOfItem );
                                        mediator->remaskWithFreeItem( this );
                                }
                                else
                                {
                                        this->offset.first = this->offset.second = 0;
                                }

                                // reshade items
                                // for both previous position and current position
                                mediator->reshadeWithFreeItem( &copyOfItem );
                                mediator->reshadeWithFreeItem( this );

                                // rearrange list of free items
                                mediator->activateFreeItemsSorting();
                        }
                }
        }

        // restore previous values for collision which is not collision with door
        if ( collisionFound && ! doorCollision )
        {
                this->xPos = copyOfItem.getX();
                this->yPos = copyOfItem.getY();
                this->zPos = copyOfItem.getZ();

                this->offset = copyOfItem.getOffset();
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
                size_t frame = ( getDataOfItem()->howManyMotions() - getDataOfItem()->howManyExtraFrames() ) / getDataOfItem()->howManyFramesForOrientations() * orientOccident ;
                if ( frame >= getDataOfItem()->howManyMotions() ) frame = 0;

                if ( this->rawImage != nilPointer && /* if images differ */ this->rawImage != getDataOfItem()->getMotionAt( frame ) )
                {
                        changeImage( getDataOfItem()->getMotionAt( frame ) );

                        if ( this->shadow != nilPointer )
                        {
                                changeShadow( getDataOfItem()->getShadowAt( frame ) );
                        }
                }

                this->behavior->changeActivityOfItem( Activity::Wait );
        }
}

bool PlayerItem::isActiveCharacter ()
{
        return mediator->getActiveCharacter() == this ;
}

void PlayerItem::fillWithData( const GameManager * data )
{
        setLives( data->getLives( getLabel() ) );

        setHighJumps( data->getHighJumps() );
        setHighSpeed( data->getHighSpeed() );
        setShieldTime( data->getShield( getLabel() ) );
        data->toolsOwnedByCharacter( getLabel(), this->tools );
        setDoughnuts( data->getDonuts( getLabel() ) );
}

void PlayerItem::addLives( unsigned char lives )
{
        if ( this->lives < 100 )
        {
                this->lives += lives;
                GameManager::getInstance()->addLives( this->getLabel (), lives );
        }
}

void PlayerItem::loseLife()
{
        if ( this->lives > 0 )
        {
                this->lives--;
                GameManager::getInstance()->loseLife( getOriginalLabel () /* current label is "bubbles" */ );
        }

        GameManager::getInstance()->emptyHandbag();
}

void PlayerItem::takeTool( const std::string& label )
{
        this->tools.push_back( label );
        GameManager::getInstance()->takeMagicItem( label );
}

void PlayerItem::addDoughnuts( const unsigned short howMany )
{
        this->howManyDoughnuts += howMany;
        GameManager::getInstance()->setDonuts( this->howManyDoughnuts );
}

void PlayerItem::useDoughnut()
{
        if ( this->howManyDoughnuts > 0 )
        {
                this->howManyDoughnuts--;
                GameManager::getInstance()->consumeDonut();
        }
}

void PlayerItem::activateHighSpeed()
{
        if ( this->getLabel() == "head" )
        {
                this->highSpeed = 99;
                GameManager::getInstance()->addHighSpeed( this->getLabel(), 99 );
        }
}

void PlayerItem::decreaseHighSpeed()
{
        if ( this->highSpeed > 0 )
        {
                this->highSpeed--;
                GameManager::getInstance()->decreaseHighSpeed( this->getLabel() );
        }
}

void PlayerItem::addHighJumps( unsigned char highJumps )
{
        if ( this->getLabel() == "heels" )
        {
                this->highJumps += highJumps;
                GameManager::getInstance()->addHighJumps( this->getLabel(), highJumps );
        }
}

void PlayerItem::decreaseHighJumps()
{
        if ( this->highJumps > 0 )
        {
                this->highJumps--;
                GameManager::getInstance()->decreaseHighJumps( this->getLabel() );
        }
}

void PlayerItem::activateShield()
{
        this->shieldTimer = new Timer();
        this->shieldTimer->go();
        this->shieldTime = 25.0;
        this->shield = shieldTime - shieldTimer->getValue();
        GameManager::getInstance()->addShield( this->getLabel(), this->shield );
}

void PlayerItem::decreaseShield()
{
        if ( this->shieldTimer != nilPointer )
        {
                this->shield = shieldTime - shieldTimer->getValue();
                GameManager::getInstance()->modifyShield( this->getLabel(), this->shield );
        }

        if ( this->shield < 0 )
        {
                this->shieldTime = 0.0;
                this->shield = 0;
                this->shieldTimer->stop();
                delete this->shieldTimer;
                this->shieldTimer = nilPointer;
        }
}

void PlayerItem::liberatePlanet ()
{
        std::string scenery = this->mediator->getRoom()->getScenery();

        if ( scenery == "jail" || scenery == "blacktooth" || scenery == "market" )
        {
                GameManager::getInstance()->liberatePlanet( "blacktooth" );
        }
        else if ( scenery == "egyptus" )
        {
                GameManager::getInstance()->liberatePlanet( "egyptus" );
        }
        else if ( scenery == "penitentiary" )
        {
                GameManager::getInstance()->liberatePlanet( "penitentiary" );
        }
        else if ( scenery == "safari" )
        {
                GameManager::getInstance()->liberatePlanet( "safari" );
        }
        else if ( scenery == "byblos" )
        {
                GameManager::getInstance()->liberatePlanet( "byblos" );
        }
}

void PlayerItem::placeItemInBag ( ItemData* itemData, const std::string& behavior )
{
        this->takenItemData = itemData;
        this->takenItemBehavior = behavior;
}

void PlayerItem::save ()
{
        GameManager::getInstance()->eatFish( this, this->mediator->getRoom() );
}

void PlayerItem::saveAt ( int x, int y, int z )
{
        GameManager::getInstance()->eatFish( this, this->mediator->getRoom(), x, y, z );
}

void PlayerItem::setShieldTime ( double shield )
{
        this->shieldTime = shield;

        if ( this->shieldTime > 0 && this->shieldTimer == nilPointer )
        {
                this->shieldTimer = new Timer();
                this->shieldTimer->go();
        }
}

bool PlayerItem::hasTool( const std::string& label ) const
{
        return std::find( this->tools.begin (), this->tools.end (), label ) != this->tools.end ();
}

}


#include "PlayerItem.hpp"
#include "Behavior.hpp"
#include "ItemData.hpp"
#include "Door.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "GameManager.hpp"
#include "UserControlled.hpp"


namespace isomot
{

PlayerItem::PlayerItem( ItemData* itemData, int x, int y, int z, const Way& way )
        : FreeItem( itemData, x, y, z, way )
        , lives( 0 )
        , highSpeed( 0 )
        , highJumps( 0 )
        , shield( 0 )
        , howManyDoughnuts( 0 )
        , exit( NoExit )
        , entry( JustWait )
        , shieldTimer( 0 )
        , shieldTime( 25.0 )
        , takenItemData( 0 )
        , takenItemImage( 0 )
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
        , shieldTimer( 0 )
        , shieldTime( playerItem.shieldTime )
        , takenItemData( 0 )
        , takenItemImage( 0 )
        , originalDataOfItem( playerItem.getOriginalDataOfItem() )
{

}

PlayerItem::~PlayerItem()
{
        if ( this->shieldTimer != 0 )
        {
                delete shieldTimer;
        }
}

void PlayerItem::setWayOfExit ( const Way& way )
{
        this->exit = way;

        switch ( way.getIntegerOfWay () )
        {
                case North:
                case South:
                case East:
                case West:
                        setOrientation( way );
                        break;

                case Northeast:
                case Northwest:
                        setOrientation( North );
                        break;

                case Southeast:
                case Southwest:
                        setOrientation( South );
                        break;

                case Eastnorth:
                case Eastsouth:
                        setOrientation( East );
                        break;

                case Westnorth:
                case Westsouth:
                        setOrientation( West );
                        break;

                default:
                        ;
        }
}

void PlayerItem::autoMoveOnEntry ( const Way& wayOfEntry )
{
        setWayOfEntry( wayOfEntry );

        if ( getBehavior() == 0 )
        {
                std::cerr << "nil behavior at PlayerItem::autoMoveOnEntry" << std::endl ;
                return;
        }

        switch ( wayOfEntry.getIntegerOfWay () )
        {
                case North:
                case Northeast:
                case Northwest:
                        getBehavior()->changeActivityOfItem( AutoMoveSouth );
                        break;

                case South:
                case Southeast:
                case Southwest:
                        getBehavior()->changeActivityOfItem( AutoMoveNorth );
                        break;

                case East:
                case Eastnorth:
                case Eastsouth:
                        getBehavior()->changeActivityOfItem( AutoMoveWest );
                        break;

                case West:
                case Westnorth:
                case Westsouth:
                        getBehavior()->changeActivityOfItem( AutoMoveEast );
                        break;

                case ByTeleport:
                case ByTeleportToo:
                        getBehavior()->changeActivityOfItem( BeginWayInTeletransport );
                        break;

                case Up:
                        getBehavior()->changeActivityOfItem( Fall );
                        break;

                default:
                        ;
        }
}

bool PlayerItem::updatePosition( int newX, int newY, int newZ, const Coordinate& whatToChange, const ChangeOrAdd& how )
{
        bool itAutomoves = getBehavior()->getActivityOfItem() == AutoMoveNorth ||
                                getBehavior()->getActivityOfItem() == AutoMoveSouth ||
                                getBehavior()->getActivityOfItem() == AutoMoveEast ||
                                getBehavior()->getActivityOfItem() == AutoMoveWest ;

        if ( ! this->isActivePlayer() && ! itAutomoves )
        {
                return FreeItem::updatePosition( newX, newY, newZ, whatToChange, how );
        }

        mediator->clearStackOfCollisions( );

        bool collisionFound = false;

        // copy item before moving
        PlayerItem oldPlayerItem( *this );

        if ( whatToChange & CoordinateX )
        {
                this->x = newX + this->x * how;
        }
        if ( whatToChange & CoordinateY )
        {
                this->y = newY + this->y * how;
        }
        if ( whatToChange & CoordinateZ )
        {
                this->z = newZ + this->z * how;
        }

        // look for collision with door

        bool doorCollision = false;

        collisionFound = mediator->findCollisionWithItem( this );
        if ( collisionFound )
        {
                // direction of movement
                int wayX = ( ( whatToChange & CoordinateX ) != 0 || x != 0 ? newX : 0 );
                int wayY = ( ( whatToChange & CoordinateY ) != 0 || y != 0 ? newY : 0 );

                while ( ! mediator->isStackOfCollisionsEmpty () )
                {
                        int id = mediator->popCollision();

                        // case of move to north wall
                        if ( wayX < 0 )
                        {
                                // check for hit of north door’s jamb
                                if ( ! ( doorCollision = isCollidingWithDoor( North, id, oldPlayerItem ) ) )
                                {
                                        // check for hit of northeast door’s jamb
                                        if ( ! ( doorCollision = isCollidingWithDoor( Northeast, id, oldPlayerItem ) ) )
                                        {
                                                // then it’s hit of northwest door’s jamb
                                                isCollidingWithDoor( Northwest, id, oldPlayerItem );
                                        }
                                }
                        }
                        // case of move to south wall
                        else if ( wayX > 0 )
                        {
                                // check for hit of south door’s jamb
                                if ( ! ( doorCollision = isCollidingWithDoor( South, id, oldPlayerItem ) ) )
                                {
                                        // check for hit of southeast door’s jamb
                                        if ( ! ( doorCollision = isCollidingWithDoor( Southeast, id, oldPlayerItem ) ) )
                                        {
                                                // then it’s hit of southwest door’s jamb
                                                doorCollision = isCollidingWithDoor( Southwest, id, oldPlayerItem );
                                        }
                                }
                        }
                        // case of move to east wall
                        else if ( wayY < 0 )
                        {
                                // check for hit of east door’s jamb
                                if ( ! ( doorCollision = isCollidingWithDoor( East, id, oldPlayerItem ) ) )
                                {
                                        // check for hit of east-north door’s jamb
                                        if ( ! ( doorCollision = isCollidingWithDoor( Eastnorth, id, oldPlayerItem ) ) )
                                        {
                                                // so it’s hit of east-south door’s jamb
                                                isCollidingWithDoor( Eastsouth, id, oldPlayerItem );
                                        }
                                }
                        }
                        // case of move to west wall
                        else if ( wayY > 0 )
                        {
                                // check for hit of west door’s jamb
                                if ( ! ( doorCollision = isCollidingWithDoor( West, id, oldPlayerItem ) ) )
                                {
                                        // check for hit of west-north door’s jamb
                                        if ( ! ( doorCollision = isCollidingWithDoor( Westnorth, id, oldPlayerItem ) ) )
                                        {
                                                // so it’s hit of west-south door’s jamb
                                                isCollidingWithDoor( Westsouth, id, oldPlayerItem );
                                        }
                                }
                        }
                }
        }

        // look for collision with real wall, one which limits the room
        if ( this->x < mediator->getRoom()->getLimitAt( Way( "north" ) )
                        && isNotUnderDoor( North ) && isNotUnderDoor( Northeast ) && isNotUnderDoor( Northwest ) )
        {
                mediator->pushCollision( NorthWall );
        }
        else if ( this->x + static_cast< int >( getDataOfItem()->getWidthX() ) > mediator->getRoom()->getLimitAt( Way( "south" ) )
                        && isNotUnderDoor( South ) && isNotUnderDoor( Southeast ) && isNotUnderDoor( Southwest ) )
        {
                mediator->pushCollision( SouthWall );
        }

        if ( this->y - static_cast< int >( getDataOfItem()->getWidthY() ) + 1 < mediator->getRoom()->getLimitAt( Way( "east" ) )
                        && isNotUnderDoor( East ) && isNotUnderDoor( Eastnorth ) && isNotUnderDoor( Eastsouth ) )
        {
                mediator->pushCollision( EastWall );
        }
        else if ( this->y >= mediator->getRoom()->getLimitAt( Way( "west" ) )
                        && isNotUnderDoor( West ) && isNotUnderDoor( Westnorth ) && isNotUnderDoor( Westsouth ) )
        {
                mediator->pushCollision( WestWall );
        }

        collisionFound = ! mediator->isStackOfCollisionsEmpty ();
        if ( ! collisionFound )
        {
                // now it is known that the player may go thru a door
                // look for collision with limits of room

                const Way ways[ 12 ] =
                        {  Northeast, Northwest, North, Southeast, Southwest, South,
                                Eastnorth, Eastsouth, East, Westnorth, Westsouth, West  };

                const SpecialId borders[ 12 ] =
                        { NortheastBorder, NorthwestBorder, NorthBorder,
                                SoutheastBorder, SouthwestBorder, SouthBorder,
                                        EastnorthBorder, EastsouthBorder, EastBorder,
                                                WestnorthBorder, WestsouthBorder, WestBorder };

                // check each limit of room
                for ( int i = 0; i < 12; i++ )
                {
                        if ( isCollidingWithRoomBorder( ways[ i ] ) )
                        {
                                mediator->pushCollision( borders[ i ] );
                                break;
                        }
                }

                // collision with floor
                if ( this->z < 0 )
                {
                        mediator->pushCollision( Floor );
                }

                collisionFound = ! mediator->isStackOfCollisionsEmpty ();
                if ( ! collisionFound )
                {
                        // look for collision with the rest of items in room
                        collisionFound = mediator->findCollisionWithItem( this );
                        if ( ! collisionFound )
                        {
                                // for item with image, mark to mask free items whose images overlap with its image
                                if ( this->rawImage )
                                {
                                        // get how many pixels is this image from point of room’s origin
                                        this->offset.first = ( ( this->x - this->y ) << 1 ) + getDataOfItem()->getWidthX() + getDataOfItem()->getWidthY() - ( this->rawImage->w >> 1 ) - 1;
                                        this->offset.second = this->x + this->y + getDataOfItem()->getWidthX() - this->rawImage->h - this->z;

                                        // for both the previous position and the current position
                                        mediator->remaskFreeItem( &oldPlayerItem );
                                        mediator->remaskFreeItem( this );
                                }
                                else
                                {
                                        this->offset.first = this->offset.second = 0;
                                }

                                // reshade items
                                if ( mediator->getDegreeOfShading() < 256 )
                                {
                                        // for both the previous position and the current position
                                        mediator->reshadeFreeItem( &oldPlayerItem );
                                        mediator->reshadeFreeItem( this );
                                }

                                // reshade and remask
                                this->myShady = WantShadow;
                                this->myMask = WantMask;

                                // rearrange list of free items
                                mediator->activateFreeItemsSorting();
                        }
                }
        }

        // restore previous values for collision which is not collision with door
        if ( collisionFound && ! doorCollision )
        {
                this->x = oldPlayerItem.getX();
                this->y = oldPlayerItem.getY();
                this->z = oldPlayerItem.getZ();

                this->offset = oldPlayerItem.getOffset();
        }

        return ! collisionFound ;
}

bool PlayerItem::isCollidingWithDoor( const Way& way, int id, const PlayerItem& previousPosition )
{
        Door* door = mediator->getRoom()->getDoorAt( way );
        int oldX = this->x;
        int oldY = this->y;

        switch ( way.getIntegerOfWay() )
        {
                case North:
                case Northeast:
                case Northwest:
                case South:
                case Southeast:
                case Southwest:
                        // for rooms with north or south door
                        if ( door )
                        {
                                // move player right when player hits left jamb
                                if ( door->getLeftJamb()->getId() == id && this->y <= door->getLeftJamb()->getY() )
                                {
                                        this->y--;
                                        this->x = previousPosition.getX();
                                }
                                // move player left when player collides with right jamb
                                else if ( door->getRightJamb()->getId() == id &&
                                                this->y - static_cast< int >( getDataOfItem()->getWidthY() )
                                                        >= door->getRightJamb()->getY() - static_cast< int >( door->getRightJamb()->getWidthY() ) )
                                {
                                        this->y++;
                                        this->x = previousPosition.getX();
                                }
                        }
                        break;

                case East:
                case Eastnorth:
                case Eastsouth:
                case West:
                case Westnorth:
                case Westsouth:
                        // for rooms with east or west door
                        if ( door )
                        {
                                // move player right when player hits left jamb
                                if ( door->getLeftJamb()->getId() == id && this->x >= door->getLeftJamb()->getX() )
                                {
                                        this->x++;
                                        this->y = previousPosition.getY();
                                }
                                // move player left when player collides with right jamb
                                else if ( door->getRightJamb()->getId() == id &&
                                                this->x - static_cast< int >( getDataOfItem()->getWidthX() )
                                                        <= door->getRightJamb()->getX() + static_cast< int >( door->getRightJamb()->getWidthX() ) )
                                {
                                        this->x--;
                                        this->y = previousPosition.getY();
                                }
                        }
                        break;

                default:
                        ;
        }

        return ( oldX != this->x || oldY != this->y );
}

bool PlayerItem::isNotUnderDoor( const Way& way )
{
        bool result = false;
        Door* door = mediator->getRoom()->getDoorAt( way );

        switch ( way.getIntegerOfWay () )
        {
                case North:
                case Northeast:
                case Northwest:
                case East:
                case Eastnorth:
                case Eastsouth:
                case South:
                case Southeast:
                case Southwest:
                case West:
                case Westnorth:
                case Westsouth:
                        result = ( ! door || ( door && ! door->isUnderDoor( this->x, this->y, this->z ) ) );
                        break;

                default:
                        ;
        }

        return result;
}

bool PlayerItem::isCollidingWithRoomBorder( const Way& way )
{
        bool result = false;
        Door* door = mediator->getRoom()->getDoorAt( way );

        switch ( way.getIntegerOfWay () )
        {
                case North:
                        result = ( this->x < 0 );
                        break;

                case Northeast:
                case Northwest:
                        result = ( door &&
                                        this->x < mediator->getRoom()->getLimitAt( way ) &&
                                        door->isUnderDoor( this->x, this->y, this->z ) );
                        break;

                case South:
                        result = ( this->x + static_cast< int >( getDataOfItem()->getWidthX() )
                                        > static_cast< int >( mediator->getRoom()->getTilesX() * mediator->getRoom()->getSizeOfOneTile() ) );
                        break;

                case Southeast:
                case Southwest:
                        result = ( door &&
                                        this->x + static_cast< int >( getDataOfItem()->getWidthX() ) > mediator->getRoom()->getLimitAt( way ) &&
                                        door->isUnderDoor( this->x, this->y, this->z ) );
                        break;

                case East:
                        result = ( this->y - static_cast< int >( getDataOfItem()->getWidthY() ) + 1 < 0 );
                        break;

                case Eastnorth:
                case Eastsouth:
                        result = ( door &&
                                        this->y - static_cast< int >( getDataOfItem()->getWidthY() ) + 1 < mediator->getRoom()->getLimitAt( way ) &&
                                        door->isUnderDoor( this->x, this->y, this->z ) );
                        break;

                case West:
                        result = ( this->y
                                        >= static_cast< int >( mediator->getRoom()->getTilesY() * mediator->getRoom()->getSizeOfOneTile() ) );
                        break;

                case Westnorth:
                case Westsouth:
                        result = ( door &&
                                        this->y + static_cast< int >( getDataOfItem()->getWidthY() ) > mediator->getRoom()->getLimitAt( way ) &&
                                        door->isUnderDoor( this->x, this->y, this->z ) );
                        break;

                default:
                        ;
        }

        return result;
}

void PlayerItem::behave ()
{
        UserControlled* userBehavior = dynamic_cast< UserControlled* >( this->behavior );
        if ( userBehavior != 0 )
        {
                userBehavior->behave ( );
        }
}

bool PlayerItem::update ()
{
        if ( behavior != 0 )
        {
                behavior->update ( );
        }

        return false;
}

void PlayerItem::wait ()
{
        ActivityOfItem activity = this->behavior->getActivityOfItem();

        // don’t wait while teleporting or loosing life
        if ( activity != BeginWayOutTeletransport && activity != WayOutTeletransport &&
                activity != BeginWayInTeletransport && activity != WayInTeletransport &&
                activity != MeetMortalItem && activity != Vanish )
        {
                // get waiting frame by orientation of item
                unsigned int orientOccident = ( orientation.getIntegerOfWay() == Nowhere ? 0 : orientation.getIntegerOfWay() );
                size_t frame = ( getDataOfItem()->howManyMotions() - getDataOfItem()->howManyExtraFrames() ) / getDataOfItem()->howManyFramesForOrientations() * orientOccident ;
                if ( frame >= getDataOfItem()->howManyMotions() ) frame = 0;

                if ( this->rawImage != 0 && /* if images differ */ this->rawImage != getDataOfItem()->getMotionAt( frame ) )
                {
                        changeImage( getDataOfItem()->getMotionAt( frame ) );

                        if ( this->shadow != 0 )
                        {
                                changeShadow( getDataOfItem()->getShadowAt( frame ) );
                        }
                }

                this->behavior->changeActivityOfItem( Wait );
        }
}

bool PlayerItem::isActivePlayer ()
{
        return mediator->getActivePlayer() == this ;
}

void PlayerItem::fillWithData( const GameManager * data )
{
        setLives( data->getLives( getLabel() ) );
        setHighJumps( data->getHighJumps() );
        setHighSpeed( data->getHighSpeed() );
        setShieldTime( data->getShield( getLabel() ) );
        setTools( data->getToolsOwnedByPlayer( getLabel() ) );
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
                GameManager::getInstance()->loseLife( this->getLabel () );
        }
        GameManager::getInstance()->emptyHandbag( this->getLabel () );
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
        this->shieldTimer = new HPC();
        this->shieldTimer->start();
        this->shieldTime = 25.0;
        this->shield = shieldTime - shieldTimer->getValue();
        GameManager::getInstance()->addShield( this->getLabel(), this->shield );
}

void PlayerItem::decreaseShield()
{
        if ( this->shieldTimer != 0 )
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
                this->shieldTimer = 0;
        }
}

void PlayerItem::liberatePlanet ()
{
        std::string scenery = this->mediator->getRoom()->getScenery();

        if ( scenery.compare( "jail" ) == 0 || scenery.compare( "blacktooth" ) == 0 || scenery.compare( "market" ) == 0 )
        {
                GameManager::getInstance()->liberatePlanet( "blacktooth" );
        }
        else if ( scenery.compare( "egyptus" ) == 0 )
        {
                GameManager::getInstance()->liberatePlanet( "egyptus" );
        }
        else if ( scenery.compare( "penitentiary" ) == 0 )
        {
                GameManager::getInstance()->liberatePlanet( "penitentiary" );
        }
        else if ( scenery.compare( "safari" ) == 0 )
        {
                GameManager::getInstance()->liberatePlanet( "safari" );
        }
        else if ( scenery.compare( "byblos" ) == 0 )
        {
                GameManager::getInstance()->liberatePlanet( "byblos" );
        }
}

void PlayerItem::assignTakenItem ( ItemData* itemData, BITMAP* itemImage, const std::string& behavior )
{
        GameManager::getInstance()->setItemTaken( itemImage );

        this->takenItemData = itemData;
        this->takenItemImage = itemImage;
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

        if ( this->shieldTime > 0 && this->shieldTimer == 0 )
        {
                this->shieldTimer = new HPC();
                this->shieldTimer->start();
        }
}

bool PlayerItem::hasTool( const std::string& label ) const
{
        return std::find( this->tools.begin (), this->tools.end (), label ) != this->tools.end ();
}

}

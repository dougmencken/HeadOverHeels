
#include "PlayerItem.hpp"
#include "Behavior.hpp"
#include "ItemData.hpp"
#include "Door.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "GameManager.hpp"
#include "SoundManager.hpp"
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
        , takenItemImage( nilPointer )
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
        , takenItemImage( nilPointer )
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

        if ( ! this->isActiveCharacter() && ! itAutomoves )
        {
                return FreeItem::updatePosition( newX, newY, newZ, whatToChange, how );
        }

        mediator->clearStackOfCollisions( );

        bool collisionFound = false;

        // copy item before moving
        PlayerItem copyOfItem( *this );

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
                        std::string what = mediator->popCollision();

                        int oldX = copyOfItem.getX();
                        int oldY = copyOfItem.getY();

                        // case of move to north wall
                        if ( wayX < 0 )
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
                        else if ( wayX > 0 )
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
                        else if ( wayY < 0 )
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
                        else if ( wayY > 0 )
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
        if ( this->x < mediator->getRoom()->getLimitAt( "north" )
                        && isNotUnderDoor( "north" ) && isNotUnderDoor( "northeast" ) && isNotUnderDoor( "northwest" ) )
        {
                mediator->pushCollision( "some segment of north wall" );
        }
        else if ( this->x + static_cast< int >( getDataOfItem()->getWidthX() ) > mediator->getRoom()->getLimitAt( "south" )
                        && isNotUnderDoor( "south" ) && isNotUnderDoor( "southeast" ) && isNotUnderDoor( "southwest" ) )
        {
                mediator->pushCollision( "some segment of south wall" );
        }

        if ( this->y - static_cast< int >( getDataOfItem()->getWidthY() ) + 1 < mediator->getRoom()->getLimitAt( "east" )
                        && isNotUnderDoor( "east" ) && isNotUnderDoor( "eastnorth" ) && isNotUnderDoor( "eastsouth" ) )
        {
                mediator->pushCollision( "some segment of east wall" );
        }
        else if ( this->y >= mediator->getRoom()->getLimitAt( "west" )
                        && isNotUnderDoor( "west" ) && isNotUnderDoor( "westnorth" ) && isNotUnderDoor( "westsouth" ) )
        {
                mediator->pushCollision( "some segment of west wall" );
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
                if ( this->z < 0 )
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
                                this->myMask = WantRemask;

                                // for item with image, mark to mask free items whose images overlap with its image
                                if ( this->rawImage != nilPointer )
                                {
                                        // get how many pixels is this image from point of room’s origin
                                        this->offset.first = ( ( this->x - this->y ) << 1 ) + getWidthX() + getDataOfItem()->getWidthY() - ( this->rawImage->w >> 1 ) - 1;
                                        this->offset.second = this->x + this->y + getWidthX() - this->rawImage->h - this->z;

                                        // for both the previous position and the current position
                                        mediator->remaskWithFreeItem( &copyOfItem );
                                        mediator->remaskWithFreeItem( this );
                                }
                                else
                                {
                                        this->offset.first = this->offset.second = 0;
                                }

                                // reshade items
                                if ( mediator->getDegreeOfShading() < 256 )
                                {
                                        // for both the previous position and the current position
                                        mediator->reshadeWithFreeItem( &copyOfItem );
                                        mediator->reshadeWithFreeItem( this );
                                }

                                // rearrange list of free items
                                mediator->activateFreeItemsSorting();
                        }
                }
        }

        // restore previous values for collision which is not collision with door
        if ( collisionFound && ! doorCollision )
        {
                this->x = copyOfItem.getX();
                this->y = copyOfItem.getY();
                this->z = copyOfItem.getZ();

                this->offset = copyOfItem.getOffset();
        }

        return ! collisionFound ;
}

bool PlayerItem::isCollidingWithDoor( const std::string& way, const std::string& name, const int previousX, const int previousY )
{
        Door* door = mediator->getRoom()->getDoorAt( way );
        int oldX = this->x;
        int oldY = this->y;

        switch ( Way( way ).getIntegerOfWay() )
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
                                if ( door->getLeftJamb()->getUniqueName() == name && this->y <= door->getLeftJamb()->getY() )
                                {
                                        this->y--;
                                        this->x = previousX;
                                }
                                // move player left when player collides with right jamb
                                else if ( door->getRightJamb()->getUniqueName() == name &&
                                                this->y - static_cast< int >( getDataOfItem()->getWidthY() )
                                                        >= door->getRightJamb()->getY() - static_cast< int >( door->getRightJamb()->getWidthY() ) )
                                {
                                        this->y++;
                                        this->x = previousX;
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
                                if ( door->getLeftJamb()->getUniqueName() == name && this->x >= door->getLeftJamb()->getX() )
                                {
                                        this->x++;
                                        this->y = previousY;
                                }
                                // move player left when player collides with right jamb
                                else if ( door->getRightJamb()->getUniqueName() == name &&
                                                this->x - static_cast< int >( getDataOfItem()->getWidthX() )
                                                        <= door->getRightJamb()->getX() + static_cast< int >( door->getRightJamb()->getWidthX() ) )
                                {
                                        this->x--;
                                        this->y = previousY;
                                }
                        }
                        break;

                default:
                        ;
        }

        if ( oldX != this->x || oldY != this->y )
        {
                isomot::SoundManager::getInstance()->play ( "door", isomot::Collision, /* loop */ false );
                return true ;
        }

        return false ;
}

bool PlayerItem::isNotUnderDoor( const std::string& way )
{
        bool result = false;
        Door* door = mediator->getRoom()->getDoorAt( way );

        switch ( Way( way ).getIntegerOfWay () )
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

bool PlayerItem::isCollidingWithLimitOfRoom( const std::string& onWhichWay )
{
        bool result = false;

        Door* door = mediator->getRoom()->getDoorAt( onWhichWay );

        switch ( Way( onWhichWay ).getIntegerOfWay () )
        {
                case North:
                        result = ( this->x < 0 );
                        break;

                case Northeast:
                case Northwest:
                        result = ( door &&
                                        this->x < mediator->getRoom()->getLimitAt( onWhichWay ) &&
                                        door->isUnderDoor( this->x, this->y, this->z ) );
                        break;

                case South:
                        result = ( this->x + static_cast< int >( getDataOfItem()->getWidthX() )
                                        > static_cast< int >( mediator->getRoom()->getTilesX() * mediator->getRoom()->getSizeOfOneTile() ) );
                        break;

                case Southeast:
                case Southwest:
                        result = ( door &&
                                        this->x + static_cast< int >( getDataOfItem()->getWidthX() ) > mediator->getRoom()->getLimitAt( onWhichWay ) &&
                                        door->isUnderDoor( this->x, this->y, this->z ) );
                        break;

                case East:
                        result = ( this->y - static_cast< int >( getDataOfItem()->getWidthY() ) + 1 < 0 );
                        break;

                case Eastnorth:
                case Eastsouth:
                        result = ( door &&
                                        this->y - static_cast< int >( getDataOfItem()->getWidthY() ) + 1 < mediator->getRoom()->getLimitAt( onWhichWay ) &&
                                        door->isUnderDoor( this->x, this->y, this->z ) );
                        break;

                case West:
                        result = ( this->y
                                        >= static_cast< int >( mediator->getRoom()->getTilesY() * mediator->getRoom()->getSizeOfOneTile() ) );
                        break;

                case Westnorth:
                case Westsouth:
                        result = ( door &&
                                        this->y + static_cast< int >( getDataOfItem()->getWidthY() ) > mediator->getRoom()->getLimitAt( onWhichWay ) &&
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
        if ( activity != BeginWayOutTeletransport && activity != WayOutTeletransport &&
                activity != BeginWayInTeletransport && activity != WayInTeletransport &&
                activity != MeetMortalItem && activity != Vanish )
        {
                // get waiting frame by orientation of item
                unsigned int orientOccident = ( orientation.getIntegerOfWay() == Nowhere ? 0 : orientation.getIntegerOfWay() );
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

                this->behavior->changeActivityOfItem( Wait );
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

        GameManager::getInstance()->emptyHandbag( getOriginalLabel () );
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


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

PlayerItem::PlayerItem( ItemData* itemData, int x, int y, int z, const Direction& direction )
        : FreeItem( itemData, x, y, z, direction )
        , lives( 0 )
        , highSpeed( 0 )
        , highJumps( 0 )
        , shield( 0 )
        , howManyDoughnuts( 0 )
        , exit( NoExit )
        , orientation( NoDirection )
        , entry( JustWait )
        , shieldTimer( 0 )
        , shieldTime( 25.0 )
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
        , orientation( playerItem.orientation )
        , entry( playerItem.entry )
        , shieldTimer( 0 )
        , shieldTime( playerItem.shieldTime )
{

}

PlayerItem::~PlayerItem()
{
        if ( this->shieldTimer != 0 )
        {
                delete shieldTimer;
        }
}

void PlayerItem::autoMoveOnEntry ( const Direction& entry )
{
        setDirectionOfEntry( entry );

        if ( getBehavior() == 0 )
        {
                std::cerr << "nil behavior at PlayerItem::autoMoveOnEntry" << std::endl ;
                return;
        }

        switch ( entry )
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

bool PlayerItem::changeData( int value, int x, int y, int z, const Datum& datum, const WhatToDo& how )
{
        bool itAutomoves = getBehavior()->getActivityOfItem() == AutoMoveNorth ||
                                getBehavior()->getActivityOfItem() == AutoMoveSouth ||
                                getBehavior()->getActivityOfItem() == AutoMoveEast ||
                                getBehavior()->getActivityOfItem() == AutoMoveWest ;

        if ( ! this->isActivePlayer() && ! itAutomoves )
        {
                return FreeItem::changeData( value, x, y, z, datum, how );
        }

        mediator->clearStackOfCollisions( );

        bool collisionFound = false;

        // copy item before moving
        PlayerItem oldPlayerItem( *this );

        switch ( datum )
        {
                case CoordinateX:
                        this->x = value + this->x * how;
                        break;

                case CoordinateY:
                        this->y = value + this->y * how;
                        break;

                case CoordinateZ:
                        this->z = value + this->z * how;
                        break;

                case CoordinatesXYZ:
                        this->x = x + this->x * how;
                        this->y = y + this->y * how;
                        this->z = z + this->z * how;
                        break;

                case WidthX:
                        getDataOfFreeItem()->widthX = value + getDataOfFreeItem()->widthX * how;
                        break;

                case WidthY:
                        getDataOfFreeItem()->widthY = value + getDataOfFreeItem()->widthY * how;
                        break;

                case Height:
                        getDataOfFreeItem()->height = value + getDataOfFreeItem()->height * how;
                        break;
        }

        // look for collision with door

        bool doorCollision = false;

        collisionFound = mediator->findCollisionWithItem( this );
        if ( collisionFound )
        {
                // direction of movement
                int wayX = ( datum == CoordinateX || x != 0 ? value : 0 );
                int wayY = ( datum == CoordinateY || y != 0 ? value : 0 );

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
        if ( this->x < mediator->getBound( North )
                && isNotUnderDoor( North ) && isNotUnderDoor( Northeast ) && isNotUnderDoor( Northwest ) )
        {
                mediator->pushCollision( NorthWall );
        }
        else if ( this->x + getDataOfFreeItem()->widthX > mediator->getBound( South )
                && isNotUnderDoor( South ) && isNotUnderDoor( Southeast ) && isNotUnderDoor( Southwest ) )
        {
                mediator->pushCollision( SouthWall );
        }

        if ( this->y - getDataOfFreeItem()->widthY + 1 < mediator->getBound( East )
                && isNotUnderDoor( East ) && isNotUnderDoor( Eastnorth ) && isNotUnderDoor( Eastsouth ) )
        {
                mediator->pushCollision( EastWall );
        }
        else if ( this->y >= mediator->getBound( West )
                && isNotUnderDoor( West ) && isNotUnderDoor( Westnorth ) && isNotUnderDoor( Westsouth ) )
        {
                mediator->pushCollision( WestWall );
        }

        collisionFound = ! mediator->isStackOfCollisionsEmpty ();
        if ( ! collisionFound )
        {
                // now it is known that the player may go thru a door
                // look for collision with limits of room

                const Direction directions[ 12 ] =
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
                        if ( isCollidingWithRoomBorder( directions[ i ] ) )
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
                                        this->offset.first = ( ( this->x - this->y ) << 1 ) + getDataOfFreeItem()->widthX + getDataOfFreeItem()->widthY - ( this->rawImage->w >> 1 ) - 1;
                                        this->offset.second = this->x + this->y + getDataOfFreeItem()->widthX - this->rawImage->h - this->z;

                                        // for both the previous position and the current position
                                        mediator->remaskWithItem( &oldPlayerItem );
                                        mediator->remaskWithItem( this );
                                }
                                else
                                {
                                        this->offset.first = this->offset.second = 0;
                                }

                                // reshade items
                                if ( mediator->getDegreeOfShading() < 256 )
                                {
                                        // for both the previous position and the current position
                                        mediator->reshadeWithItem( &oldPlayerItem );
                                        mediator->reshadeWithItem( this );
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
                this->dataOfItem->widthX = oldPlayerItem.getWidthX();
                this->dataOfItem->widthY = oldPlayerItem.getWidthY();
                this->dataOfItem->height = oldPlayerItem.getHeight();
                this->offset = oldPlayerItem.getOffset();
        }

        return ! collisionFound ;
}

bool PlayerItem::isCollidingWithDoor( const Direction& direction, int id, const PlayerItem& previousPosition )
{
        Door* door = mediator->getDoor( direction );
        int oldX = this->x;
        int oldY = this->y;

        switch ( direction )
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
                                else if ( door->getRightJamb()->getId() == id && this->y - getDataOfFreeItem()->widthY >= door->getRightJamb()->getY() - door->getRightJamb()->getWidthY() )
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
                                else if ( door->getRightJamb()->getId() == id && this->x - getDataOfFreeItem()->widthX <= door->getRightJamb()->getX() + door->getRightJamb()->getWidthX() )
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

bool PlayerItem::isNotUnderDoor( const Direction& direction )
{
        bool result = false;
        Door* door = mediator->getDoor( direction );

        switch ( direction )
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

bool PlayerItem::isCollidingWithRoomBorder( const Direction& direction )
{
        bool result = false;
        Door* door = mediator->getDoor( direction );

        switch ( direction )
        {
                case North:
                        result = ( this->x < 0 );
                        break;

                case Northeast:
                case Northwest:
                        result = ( door && this->x < mediator->getBound( direction ) && door->isUnderDoor( this->x, this->y, this->z ) );
                        break;

                case South:
                        result = ( this->x + getDataOfFreeItem()->widthX > static_cast< int >( mediator->getRoom()->getTilesX() * mediator->getRoom()->getSizeOfOneTile() ) );
                        break;

                case Southeast:
                case Southwest:
                        result = ( door && this->x + getDataOfFreeItem()->widthX > mediator->getBound( direction ) && door->isUnderDoor( this->x, this->y, this->z ) );
                        break;

                case East:
                        result = ( this->y - getDataOfFreeItem()->widthY + 1 < 0 );
                        break;

                case Eastnorth:
                case Eastsouth:
                        result = ( door && this->y - getDataOfFreeItem()->widthY + 1 < mediator->getBound( direction ) && door->isUnderDoor( this->x, this->y, this->z ) );
                        break;

                case West:
                        result = ( this->y >= static_cast< int >( mediator->getRoom()->getTilesY() * mediator->getRoom()->getSizeOfOneTile() ) );
                        break;

                case Westnorth:
                case Westsouth:
                        result = ( door && this->y + getDataOfFreeItem()->widthY > mediator->getBound( direction ) && door->isUnderDoor( this->x, this->y, this->z ) );
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

        // Si el jugador se está teletransportando o está muriendo no se podrá detener
        if ( activity != BeginWayOutTeletransport && activity != WayOutTeletransport &&
                activity != BeginWayInTeletransport && activity != WayInTeletransport &&
                activity != MeetMortalItem && activity != Vanish )
        {
                // El fotograma de parada es distinto segn la orientación del elemento
                int currentFrame = ( getDataOfFreeItem()->motion.size() - getDataOfFreeItem()->extraFrames ) / getDataOfFreeItem()->directionFrames * static_cast < int > ( direction );

                // if images differ
                if ( this->rawImage != 0 && this->rawImage != getDataOfFreeItem ()->motion[ currentFrame ] )
                {
                        changeImage( getDataOfFreeItem ()->motion[ currentFrame ] );

                        if ( this->shadow != 0 )
                        {
                                changeShadow( getDataOfFreeItem ()->shadows[ currentFrame ] );
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
        if( this->lives < 100 )
        {
                this->lives += lives;
                GameManager::getInstance()->addLives( this->getLabel (), lives );
        }
}

void PlayerItem::loseLife()
{
        if( this->lives > 0 )
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

void PlayerItem::assignTakenItem ( ItemData* itemData, BITMAP* takenItemImage, const std::string& behavior )
{
        dynamic_cast< UserControlled* >( this->behavior )->assignTakenItem( itemData, takenItemImage, behavior );
}

std::string PlayerItem::consultTakenItem ( ItemData* itemData )
{
        itemData = dynamic_cast< UserControlled * >( this->behavior )->getTakenItemData ();
        return dynamic_cast< UserControlled * >( this->behavior )->getTakenItemBehavior ();
}

BITMAP* PlayerItem::consultTakenItemImage ()
{
        return dynamic_cast< UserControlled * >( this->behavior )->getTakenItemImage ();
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


#include "AvatarItem.hpp"

#include "Behavior.hpp"
#include "Door.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "GameManager.hpp"
#include "GameInfo.hpp"
#include "PlayerControlled.hpp"
#include "ItemDescriptions.hpp"

#include <algorithm> // std::find


AvatarItem::AvatarItem( const DescriptionOfItem & description, int x, int y, int z, const std::string & heading )
        : FreeItem( description, x, y, z, heading )
        , wayOfExit( "" )
        , wayOfEntry( "" )
        , shieldTimer( new Timer () )
        , descriptionOfTakenItem( nilPointer )
{
        characterToBehaviour ();
}

AvatarItem::AvatarItem( const AvatarItem & toCopy )
        : FreeItem( toCopy )
        , wayOfExit( toCopy.wayOfExit )
        , wayOfEntry( toCopy.wayOfEntry )
        , shieldTimer( new Timer () )
        , descriptionOfTakenItem( nilPointer )
{
        characterToBehaviour ();

        if ( GameManager::getInstance().getGameInfo().getShieldPointsByName( getOriginalKind() ) > 0 ) {
                std::cout << "character \"" << getOriginalKind() << "\" continues to be immune for "
                                                << toCopy.getShieldSeconds() << " seconds" << std::endl ;
                this->shieldTimer->copyValueOf( * toCopy.shieldTimer );
        }
}

/* private */ void AvatarItem::characterToBehaviour ()
{
        if ( getOriginalKind() == "head" )
                setBehaviorOf( "behavior of Head" );
        else
        if ( getOriginalKind() == "heels" )
                setBehaviorOf( "behavior of Heels" );
        else
        if ( getOriginalKind() == "headoverheels" )
                setBehaviorOf( "behavior of Head over Heels" );
}

/* private */ void AvatarItem::autoMoveOnEntry ()
{
        assert( getBehavior() != nilPointer );

        switch ( Way( this->wayOfEntry ).getIntegerOfWay () )
        {
                case Way::North:
                case Way::Northeast:
                case Way::Northwest:
                        getBehavior()->setCurrentActivity( activities::Activity::AutomovingSouth );
                        break;

                case Way::South:
                case Way::Southeast:
                case Way::Southwest:
                        getBehavior()->setCurrentActivity( activities::Activity::AutomovingNorth );
                        break;

                case Way::East:
                case Way::Eastnorth:
                case Way::Eastsouth:
                        getBehavior()->setCurrentActivity( activities::Activity::AutomovingWest );
                        break;

                case Way::West:
                case Way::Westnorth:
                case Way::Westsouth:
                        getBehavior()->setCurrentActivity( activities::Activity::AutomovingEast );
                        break;

                case Way::ByTeleport:
                case Way::ByTeleportToo:
                        getBehavior()->setCurrentActivity( activities::Activity::EndTeletransportation );
                        break;

                /* case Way::Above: // will fall without this anyhow
                        getBehavior()->setCurrentActivity( activities::Activity::Falling );
                        break; */

                default:
                        getBehavior()->setCurrentActivity( activities::Activity::Waiting );
                        break;
        }
}

void AvatarItem::setWayOfEntry ( const std::string & way )
{
        std::cout << "setting the way of entry = \"" << way << "\" for " << getUniqueName() << std::endl ;

        this->wayOfEntry = way ;
        autoMoveOnEntry() ;
}

void AvatarItem::setWayOfExit ( const std::string & way )
{
        std::cout << "setting the way of exit = \"" << way << "\" for " << getUniqueName() << std::endl ;

        this->wayOfExit = way ;

        switch ( Way( way ).getIntegerOfWay () )
        {
                case Way::North:
                case Way::South:
                case Way::East:
                case Way::West:
                        changeHeading( way );
                        break;

                case Way::Northeast:
                case Way::Northwest:
                        changeHeading( "north" );
                        break;

                case Way::Southeast:
                case Way::Southwest:
                        changeHeading( "south" );
                        break;

                case Way::Eastnorth:
                case Way::Eastsouth:
                        changeHeading( "east" );
                        break;

                case Way::Westnorth:
                case Way::Westsouth:
                        changeHeading( "west" );
                        break;

                default:
                        ;
        }
}

bool AvatarItem::addToPosition( int x, int y, int z )
{
        // inactive character automoves when they both enter the same room
        // and couldn’t go out the door with the parent FreeItem’s addToPosition method
        const activities::Activity & activity = getBehavior()->getCurrentActivity() ;
        bool autowalksThruDoor = ( activity == activities::Activity::AutomovingNorth || activity == activities::Activity::AutomovingSouth
                                        || activity == activities::Activity::AutomovingEast || activity == activities::Activity::AutomovingWest );

        ////// can’t get what’s wrong with falling down near a door as uncontrolled FreeItem
        //////bool fallsUnderDoor = ( x == 0 && y == 0 && z < 0 && this->isUnderSomeDoor () );

        if ( ! this->isActiveCharacter() && ! autowalksThruDoor /* && ! fallsUnderDoor */ )
                return FreeItem::addToPosition( x, y, z );

        getMediator()->clearCollisions ();

        const int xBefore = getX() ;
        const int yBefore = getY() ;
        const int zBefore = getZ() ;

        const int imageOffsetXBefore = getImageOffsetX ();
        const int imageOffsetYBefore = getImageOffsetY ();

        setX( xBefore + x );
        setY( yBefore + y );
        setZ( zBefore + z );

        // look for a collision with a door’s jamb

        bool jambCollision = false;

        bool collisionFound = getMediator()->collectCollisionsWith( this->getUniqueName() );
        if ( collisionFound )
        {
                while ( getMediator()->isThereAnyCollision () )
                {
                        std::string collision = getMediator()->popCollision() ;

                        if ( x < 0 ) // moving north
                        {
                                // see if the character hits a north door’s jamb
                                jambCollision = isCollidingWithJamb( "north", collision, xBefore, yBefore );
                                if ( ! jambCollision ) {
                                        // then maybe the character hits a north-east door’s jamb
                                        jambCollision = isCollidingWithJamb( "northeast", collision, xBefore, yBefore );
                                        if ( ! jambCollision ) {
                                                // or a north-west door’s jamb
                                                jambCollision = isCollidingWithJamb( "northwest", collision, xBefore, yBefore );
                                        }
                                }
                        }
                        else if ( x > 0 ) // moving south
                        {
                                // see if the character hits a south door’s jamb
                                jambCollision = isCollidingWithJamb( "south", collision, xBefore, yBefore );
                                if ( ! jambCollision ) {
                                        // then maybe the character hits a south-east door’s jamb
                                        jambCollision = isCollidingWithJamb( "southeast", collision, xBefore, yBefore );
                                        if ( ! jambCollision ) {
                                                // or a south-west door’s jamb
                                                jambCollision = isCollidingWithJamb( "southwest", collision, xBefore, yBefore );
                                        }
                                }
                        }
                        else if ( y < 0 ) // moving east
                        {
                                // see if the character hits an east door’s jamb
                                jambCollision = isCollidingWithJamb( "east", collision, xBefore, yBefore );
                                if ( ! jambCollision ) {
                                        // maybe the character hits an east-north door’s jamb
                                        jambCollision = isCollidingWithJamb( "eastnorth", collision, xBefore, yBefore );
                                        if ( ! jambCollision ) {
                                                // or an east-south door’s jamb
                                                jambCollision = isCollidingWithJamb( "eastsouth", collision, xBefore, yBefore );
                                        }
                                }
                        }
                        else if ( y > 0 ) // moving west
                        {
                                // see if the character hits a west door’s jamb
                                jambCollision = isCollidingWithJamb( "west", collision, xBefore, yBefore );
                                if ( ! jambCollision ) {
                                        // maybe the character hits a west-north door’s jamb
                                        jambCollision = isCollidingWithJamb( "westnorth", collision, xBefore, yBefore );
                                        if ( ! jambCollision ) {
                                                // or a west-south door’s jamb
                                                jambCollision = isCollidingWithJamb( "westsouth", collision, xBefore, yBefore );
                                        }
                                }
                        }
                }
        }

        // look for a collision with a wall

        if ( getX() < getMediator()->getRoom()->getLimitAt( "north" )
                        && isNotUnderDoorAt( "north" ) && isNotUnderDoorAt( "northeast" ) && isNotUnderDoorAt( "northwest" ) )
        {
                getMediator()->addCollisionWith( "some segment of the north wall" );
        }
        else if ( getX() + getWidthX() > getMediator()->getRoom()->getLimitAt( "south" )
                        && isNotUnderDoorAt( "south" ) && isNotUnderDoorAt( "southeast" ) && isNotUnderDoorAt( "southwest" ) )
        {
                getMediator()->addCollisionWith( "some segment of the south wall" );
        }

        if ( getY() - getWidthY() + 1 < getMediator()->getRoom()->getLimitAt( "east" )
                        && isNotUnderDoorAt( "east" ) && isNotUnderDoorAt( "eastnorth" ) && isNotUnderDoorAt( "eastsouth" ) )
        {
                getMediator()->addCollisionWith( "some segment of the east wall" );
        }
        else if ( getY() >= getMediator()->getRoom()->getLimitAt( "west" )
                        && isNotUnderDoorAt( "west" ) && isNotUnderDoorAt( "westnorth" ) && isNotUnderDoorAt( "westsouth" ) )
        {
                getMediator()->addCollisionWith( "some segment of the west wall" );
        }

        collisionFound = getMediator()->isThereAnyCollision ();
        if ( ! collisionFound )
        {
                // the character may go thru a door
                for ( unsigned int i = 0 ; i < Room::Sides ; ++ i ) {
                        if ( isWalkingThroughDoorAt( Room::Sides_Of_Room[ i ] ) ) {
                                getMediator()->addCollisionWith( Room::Sides_Of_Room[ i ] + " door" );
                                break;
                        }
                }

                // collision with the floor
                if ( this->getZ() < 0 ) {
                        getMediator()->addCollisionWith( "some tile of floor" );
                }

                // collision with the ceiling
                if ( z >= 0 && this->getZ() > ( Room::MaxLayers - 1 ) * Room::LayerHeight + ( Room::LayerHeight >> 1 ) ) {
                        getMediator()->addCollisionWith( "ceiling" );
                }

                collisionFound = getMediator()->isThereAnyCollision ();
                if ( ! collisionFound ) {
                        // look for collisions with other items in the room
                        collisionFound = getMediator()->collectCollisionsWith( this->getUniqueName() );
                        if ( ! collisionFound ) {
                                // reshade and remask
                                freshBothProcessedImages();
                                setWantShadow( true );
                                setWantMaskTrue();

                                // mark to mask the free items whose images overlap with the character’s image
                                getMediator()->wantToMaskWithFreeItemImageAt( *this, imageOffsetXBefore, imageOffsetYBefore );
                                getMediator()->wantToMaskWithFreeItem( *this );

                                // reshade other items
                                getMediator()->wantShadowFromFreeItemAt( *this, xBefore, yBefore, zBefore );
                                getMediator()->wantShadowFromFreeItem( *this );

                                // re~sort
                                getMediator()->markToSortFreeItems ();
                        }
                }
        }

        if ( collisionFound && ! jambCollision ) {
                // restore previous values
                setX( xBefore );
                setY( yBefore );
                setZ( zBefore );
        }

        return ! collisionFound ;
}

bool AvatarItem::isWalkingThroughDoorAt( const std::string & where )
{
        Door* door = getMediator()->getRoom()->getDoorOn( where );
        if ( door == nilPointer ) return false ;

        bool walksThruDoor = false ;

        const unsigned int oneTile = getMediator()->getRoom()->getSizeOfOneTile() ;

        switch ( Way( where ).getIntegerOfWay () )
        {
                case Way::North:
                        walksThruDoor = ( getX() < 0 );
                        break;

                case Way::Northeast:
                case Way::Northwest:
                        walksThruDoor = ( getX() < getMediator()->getRoom()->getLimitAt( where )
                                                && door->isUnderDoor( *this ) );
                        break;

                case Way::South:
                        walksThruDoor = ( getX() + getWidthX() > static_cast< int >( getMediator()->getRoom()->getTilesOnX() * oneTile ) );
                        break;

                case Way::Southeast:
                case Way::Southwest:
                        walksThruDoor = ( getX() + getWidthX() > getMediator()->getRoom()->getLimitAt( where )
                                                && door->isUnderDoor( *this ) );
                        break;

                case Way::East:
                        walksThruDoor = ( getY() - getWidthY() + 1 < 0 );
                        break;

                case Way::Eastnorth:
                case Way::Eastsouth:
                        walksThruDoor = ( getY() - getWidthY() + 1 < getMediator()->getRoom()->getLimitAt( where )
                                                && door->isUnderDoor( *this ) );
                        break;

                case Way::West:
                        walksThruDoor = ( getY() >= static_cast< int >( getMediator()->getRoom()->getTilesOnY() * oneTile ) );
                        break;

                case Way::Westnorth:
                case Way::Westsouth:
                        walksThruDoor = ( getY() + getWidthY() > getMediator()->getRoom()->getLimitAt( where )
                                                && door->isUnderDoor( *this ) );
                        break;

                default:
                        ;
        }

        return walksThruDoor ;
}

void AvatarItem::behaveCharacter ()
{
        if ( getBehavior() != nilPointer )
                dynamic_cast< behaviors::PlayerControlled & >( * getBehavior() ).behave ();
}

void AvatarItem::wait ()
{
        Activity activity = getBehavior()->getCurrentActivity() ;

        // don’t wait while teleporting or loosing life
        if ( activity != activities::Activity::BeginTeletransportation && activity != activities::Activity::EndTeletransportation
                        && activity != activities::Activity::MetLethalItem && activity != activities::Activity::Vanishing )
        {
                // set waiting frame by angular orientation
                changeFrame( firstFrame () );

                getBehavior()->setCurrentActivity( activities::Activity::Waiting );
        }
}

bool AvatarItem::isActiveCharacter () const
{
        if ( getMediator()->getActiveCharacter() == nilPointer ) return false ;
        return getMediator()->getActiveCharacter()->getUniqueName() == this->getUniqueName() ;
}

unsigned char AvatarItem::getLives() const
{
        const std::string & character = this->getOriginalKind() ;
        GameInfo & gameInfo = GameManager::getInstance().getGameInfo () ;
        return gameInfo.getLivesByName( character ) ;
}

void AvatarItem::addLives( unsigned char lives )
{
        const std::string & character = this->getOriginalKind() ;
        GameInfo & gameInfo = GameManager::getInstance().getGameInfo () ;
        gameInfo.addLivesByName( character, lives );
}

void AvatarItem::loseLife ()
{
        GameManager::getInstance().loseLifeAndContinue( getOriginalKind() // the current kind is "bubbles"
                                                        , getMediator()->getRoom() );
}

void AvatarItem::takeMagicTool( const std::string & tool )
{
        GameManager::getInstance().getGameInfo().takeMagicTool( tool );
}

unsigned short AvatarItem::getDonuts () const
{
        return GameManager::getInstance().getGameInfo ().getDoughnuts () ;
}

void AvatarItem::addDonuts( unsigned short howMany )
{
        GameManager::getInstance().getGameInfo ().addDoughnuts( howMany );
}

void AvatarItem::useDoughnutHorn ()
{
        GameManager::getInstance().getGameInfo ().consumeOneDoughnut () ;
}

unsigned short AvatarItem::getQuickSteps () const
{
        return GameManager::getInstance().getGameInfo ().getBonusQuickSteps () ;
}

void AvatarItem::activateBonusQuickSteps ()
{
        if ( this->getOriginalKind() == "head" )
        {
                GameInfo & gameInfo = GameManager::getInstance().getGameInfo () ;

                static const unsigned short bonusHighSpeedSteps = 99 ;
                gameInfo.addQuickStepsByName( this->getOriginalKind(), bonusHighSpeedSteps );
        }
}

void AvatarItem::decrementBonusQuickSteps ()
{
        GameManager::getInstance().getGameInfo().decrementQuickStepsByName( this->getOriginalKind() );
}

unsigned short AvatarItem::getHighJumps () const
{
        return GameManager::getInstance().getGameInfo ().getBonusHighJumps () ;
}

void AvatarItem::addBonusHighJumps( unsigned char howMany )
{
        if ( getOriginalKind() == "heels" )
        {
                GameManager::getInstance().getGameInfo().addHighJumpsByName( this->getOriginalKind(), howMany );
        }
}

void AvatarItem::decrementBonusHighJumps ()
{
        GameManager::getInstance().getGameInfo().decrementHighJumpsByName( this->getOriginalKind() );
}

short AvatarItem::getShieldPoints () const
{
        return GameManager::getInstance().getGameInfo().getShieldPointsByName( getOriginalKind() );
}

double AvatarItem::getShieldSeconds () const
{
        return GameManager::getInstance().getGameInfo().getShieldSecondsByName( getOriginalKind() );
}

void AvatarItem::activateShield ()
{
        GameInfo & gameInfo = GameManager::getInstance().getGameInfo () ;
        gameInfo.setShieldPointsByName( getOriginalKind(), 99 );

        shieldTimer->go () ;
}

void AvatarItem::activateShieldForSeconds ( double seconds )
{
        GameInfo & gameInfo = GameManager::getInstance().getGameInfo () ;
        gameInfo.setShieldSecondsByName( getOriginalKind(), seconds );

        if ( seconds > 0 && ! hasShield() ) shieldTimer->go () ;
}

void AvatarItem::decrementShieldOverTime ()
{
        double shieldSecondsRemaining = GameInfo::fullShieldTimeInSeconds - shieldTimer->getValue() ;

        if ( shieldSecondsRemaining < 0 ) {
                shieldSecondsRemaining = 0 ;
                shieldTimer->stop () ;
        }

        GameManager::getInstance().getGameInfo().setShieldSecondsByName( getOriginalKind(), shieldSecondsRemaining );
}

void AvatarItem::liberateCurrentPlanet ()
{
        const std::string & scenery = getMediator()->getRoom()->getScenery ();

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

void AvatarItem::placeItemInBag ( const std::string & kindOfItem, const std::string & behavior )
{
        this->descriptionOfTakenItem = ItemDescriptions::descriptions ().getDescriptionByKind( kindOfItem ) ;
        this->behaviorOfTakenItem = behavior ;
}

void AvatarItem::emptyBag ()
{
        this->descriptionOfTakenItem = nilPointer ;
        this->behaviorOfTakenItem = "" ;
}

void AvatarItem::saveGame ()
{
        GameManager::getInstance().eatFish( *this, getMediator()->getRoom() );
}

void AvatarItem::metamorphInto( const std::string & newKind, const std::string & initiatedBy )
{
        // when the composite character morphs into bubbles, it’s actually double bubbles
        bool doubleBubbles = ( isHeadOverHeels() && newKind == "bubbles" );
        Item::metamorphInto( doubleBubbles ? "double-bubbles" : newKind, initiatedBy );
}

bool AvatarItem::hasTool( const std::string & tool ) const
{
        GameInfo & gameInfo = GameManager::getInstance().getGameInfo () ;
        const std::string & character = this->getOriginalKind() ;

        if ( tool == "horn" && gameInfo.hasHorn() )
                if (  character == "head" || character == "headoverheels" )
                        return true ;

        if ( tool == "handbag" && gameInfo.hasHandbag() )
                if ( character == "heels" || character == "headoverheels" )
                        return true ;

        return false ;
}

bool AvatarItem::isNotUnderDoorAt( const std::string & where )
{
        Door* door = getMediator()->getRoom()->getDoorOn( where );

        return ( door == nilPointer || ! door->isUnderDoor( *this ) );
}

/*** bool AvatarItem::isUnderSomeDoor ()
{
        const std::map < std::string, Door* > & doors = getMediator()->getRoom()->getDoors();

        for ( std::map < std::string, Door* >::const_iterator iter = doors.begin () ; iter != doors.end (); ++ iter )
        {
                Door* door = iter->second ;
                if ( door != nilPointer && door->isUnderDoor( *this ) )
                        return true;
        }

        return false;
} ***/

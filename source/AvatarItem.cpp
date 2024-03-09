
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


AvatarItem::AvatarItem( const DescriptionOfItem* description, int x, int y, int z, const std::string & heading )
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

        mediator->clearCollisions ();

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

        bool collisionFound = mediator->collectCollisionsWith( this->getUniqueName() );
        if ( collisionFound )
        {
                while ( mediator->isThereAnyCollision () )
                {
                        std::string collision = mediator->popCollision() ;

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

        if ( getX() < mediator->getRoom()->getLimitAt( "north" )
                        && isNotUnderDoorAt( "north" ) && isNotUnderDoorAt( "northeast" ) && isNotUnderDoorAt( "northwest" ) )
        {
                mediator->addCollisionWith( "some segment of the north wall" );
        }
        else if ( getX() + getWidthX() > mediator->getRoom()->getLimitAt( "south" )
                        && isNotUnderDoorAt( "south" ) && isNotUnderDoorAt( "southeast" ) && isNotUnderDoorAt( "southwest" ) )
        {
                mediator->addCollisionWith( "some segment of the south wall" );
        }

        if ( getY() - getWidthY() + 1 < mediator->getRoom()->getLimitAt( "east" )
                        && isNotUnderDoorAt( "east" ) && isNotUnderDoorAt( "eastnorth" ) && isNotUnderDoorAt( "eastsouth" ) )
        {
                mediator->addCollisionWith( "some segment of the east wall" );
        }
        else if ( getY() >= mediator->getRoom()->getLimitAt( "west" )
                        && isNotUnderDoorAt( "west" ) && isNotUnderDoorAt( "westnorth" ) && isNotUnderDoorAt( "westsouth" ) )
        {
                mediator->addCollisionWith( "some segment of the west wall" );
        }

        collisionFound = mediator->isThereAnyCollision ();
        if ( ! collisionFound )
        {
                // the character may go thru a door

                static const std::string doors[ 12 ] =
                        {  "northeast", "northwest", "north", "southeast", "southwest", "south",
                                "eastnorth", "eastsouth", "east", "westnorth", "westsouth", "west"  };

                for ( unsigned int i = 0; i < 12; i++ ) {
                        if ( isWalkingThroughDoorAt( doors[ i ] ) ) {
                                mediator->addCollisionWith( doors[ i ] + " door" );
                                break;
                        }
                }

                // collision with the floor
                if ( this->getZ() < 0 ) {
                        mediator->addCollisionWith( "some tile of floor" );
                }

                // collision with the ceiling
                if ( z >= 0 && this->getZ() > ( Room::MaxLayers - 1 ) * Room::LayerHeight + ( Room::LayerHeight >> 1 ) ) {
                        mediator->addCollisionWith( "ceiling" );
                }

                collisionFound = mediator->isThereAnyCollision ();
                if ( ! collisionFound ) {
                        // look for collisions with other items in the room
                        collisionFound = mediator->collectCollisionsWith( this->getUniqueName() );
                        if ( ! collisionFound ) {
                                // reshade and remask
                                freshBothProcessedImages();
                                setWantShadow( true );
                                setWantMaskTrue();

                                // mark to mask the free items whose images overlap with the character’s image
                                mediator->wantToMaskWithFreeItemImageAt( *this, imageOffsetXBefore, imageOffsetYBefore );
                                mediator->wantToMaskWithFreeItem( *this );

                                // reshade other items
                                mediator->wantShadowFromFreeItemAt( *this, xBefore, yBefore, zBefore );
                                mediator->wantShadowFromFreeItem( *this );

                                // re~sort
                                mediator->markToSortFreeItems ();
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
        Door* door = mediator->getRoom()->getDoorAt( where );
        if ( door == nilPointer ) return false ;

        bool walksThruDoor = false ;

        const unsigned int oneTile = mediator->getRoom()->getSizeOfOneTile() ;

        switch ( Way( where ).getIntegerOfWay () )
        {
                case Way::North:
                        walksThruDoor = ( getX() < 0 );
                        break;

                case Way::Northeast:
                case Way::Northwest:
                        walksThruDoor = ( getX() < mediator->getRoom()->getLimitAt( where )
                                                && door->isUnderDoor( *this ) );
                        break;

                case Way::South:
                        walksThruDoor = ( getX() + getWidthX() > static_cast< int >( mediator->getRoom()->getTilesX() * oneTile ) );
                        break;

                case Way::Southeast:
                case Way::Southwest:
                        walksThruDoor = ( getX() + getWidthX() > mediator->getRoom()->getLimitAt( where )
                                                && door->isUnderDoor( *this ) );
                        break;

                case Way::East:
                        walksThruDoor = ( getY() - getWidthY() + 1 < 0 );
                        break;

                case Way::Eastnorth:
                case Way::Eastsouth:
                        walksThruDoor = ( getY() - getWidthY() + 1 < mediator->getRoom()->getLimitAt( where )
                                                && door->isUnderDoor( *this ) );
                        break;

                case Way::West:
                        walksThruDoor = ( getY() >= static_cast< int >( mediator->getRoom()->getTilesY() * oneTile ) );
                        break;

                case Way::Westnorth:
                case Way::Westsouth:
                        walksThruDoor = ( getY() + getWidthY() > mediator->getRoom()->getLimitAt( where )
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
        return mediator->getActiveCharacter()->getUniqueName() == this->getUniqueName() ;
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
        setWayOfExit( "rebuild room" );

        const std::string & character = this->getOriginalKind() ;
        GameManager & gameManager = GameManager::getInstance () ;
        GameInfo & gameInfo = gameManager.getGameInfo () ;
        unsigned char lives = gameInfo.getLivesByName( character ) ;

        if ( ! gameManager.areLivesInexhaustible () && lives > 0 )
                gameInfo.loseLifeByName( getOriginalKind () /* the current kind is "bubbles" */ );

        gameManager.emptyHandbag () ;
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

                unsigned short bonusHighSpeedSteps = 99 ;
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

void AvatarItem::activateShield ()
{
        shieldTimer->reset () ;
        shieldTimer->go () ;

        GameInfo & gameInfo = GameManager::getInstance().getGameInfo () ;
        const std::string & character = this->getOriginalKind() ;
        gameInfo.setShieldPointsByName( character, 99 );
}

void AvatarItem::activateShieldForSeconds ( double seconds )
{
        shieldTimer->reset () ;

        if ( seconds > 0 && ! hasShield() )
        {
                shieldTimer->go () ;
        }

        GameInfo & gameInfo = GameManager::getInstance().getGameInfo () ;
        const std::string & character = this->getOriginalKind() ;
        gameInfo.setShieldSecondsByName( character, seconds );
}

void AvatarItem::decrementShieldOverTime ()
{
        GameInfo & gameInfo = GameManager::getInstance().getGameInfo () ;
        double shieldSecondsRemaining = GameInfo::fullShieldTimeInSeconds - shieldTimer->getValue() ;

        if ( shieldSecondsRemaining < 0 )
        {
                shieldSecondsRemaining = 0 ;
                shieldTimer->stop () ;
        }

        const std::string & character = this->getOriginalKind() ;
        gameInfo.setShieldSecondsByName( character, shieldSecondsRemaining );
}

void AvatarItem::liberateCurrentPlanet ()
{
        const std::string & scenery = this->mediator->getRoom()->getScenery ();

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
        this->behaviorOfTakenItem = "still" ;
}

void AvatarItem::saveGame ()
{
        GameManager::getInstance().eatFish( *this, this->mediator->getRoom() );
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

bool AvatarItem::hasShield () const
{
        GameInfo & gameInfo = GameManager::getInstance().getGameInfo () ;
        return gameInfo.getShieldPointsByName( this->getOriginalKind() ) > 0 ;
}

bool AvatarItem::isNotUnderDoorAt( const std::string & where )
{
        Door* door = mediator->getRoom()->getDoorAt( where );

        return ( door == nilPointer || ! door->isUnderDoor( *this ) );
}

/*** bool AvatarItem::isUnderSomeDoor ()
{
        const std::map < std::string, Door* > & doors = mediator->getRoom()->getDoors();

        for ( std::map < std::string, Door* >::const_iterator iter = doors.begin () ; iter != doors.end (); ++ iter )
        {
                Door* door = iter->second ;
                if ( door != nilPointer && door->isUnderDoor( *this ) )
                        return true;
        }

        return false;
} ***/

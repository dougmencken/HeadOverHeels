
#include "UserControlled.hpp"
#include "DescriptionOfItem.hpp"
#include "Room.hpp"
#include "Mediator.hpp"
#include "MoveKindOfActivity.hpp"
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "JumpKindOfActivity.hpp"
#include "Doughnut.hpp"
#include "GameManager.hpp"
#include "SoundManager.hpp"


namespace iso
{

UserControlled::UserControlled( const ItemPtr & item, const std::string & behavior )
        : Behavior( item, behavior )
        , jumpPhase( 0 )
        , jumpPhases( 0 )
        , highJumpPhases( 0 )
        , automaticSteps( 16 /* pasos automáticos */ )
        , automaticStepsThruDoor( automaticSteps )
        , highSpeedSteps( 0 )
        , shieldSteps( 0 )
        , donutFromHooterIsHere( false )
        , labelOfBubbles( "bubbles" )
        , labelOfFiredDoughnut( "bubbles" )
        , speedTimer( new Timer () )
        , fallTimer( new Timer () )
        , glideTimer( new Timer () )
        , timerForBlinking( new Timer () )
{

}

UserControlled::~UserControlled()
{
        jumpVector.clear();
        highJumpVector.clear();
}

void UserControlled::wait( PlayerItem & player )
{
        player.wait();

        if ( FallKindOfActivity::getInstance().fall( this ) )
        {
                speedTimer->reset();
                activity = Activity::Fall;

                if ( player.getLabel() == "head" && player.getHighSpeed() > 0 && this->highSpeedSteps < 4 )
                {
                        // reset double speed counter
                        this->highSpeedSteps = 0;
                }
        }
}

void UserControlled::move( PlayerItem & player )
{
        // move when character isn’t frozen
        if ( ! player.isFrozen() )
        {
                // apply effect of bunny of high speed
                float speed = player.getSpeed() / ( player.getHighSpeed() > 0 ? 2 : 1 );

                // is it time to move
                if ( speedTimer->getValue() > speed )
                {
                        // move it
                        bool moved = MoveKindOfActivity::getInstance().move( this, &activity, true );

                        // decrement high speed
                        if ( player.getHighSpeed() > 0 && moved && activity != Activity::Fall )
                        {
                                this->highSpeedSteps++;

                                if ( this->highSpeedSteps == 8 )
                                {
                                        player.decreaseHighSpeed();
                                        this->highSpeedSteps = 4;
                                }
                        }

                        speedTimer->reset();

                        player.animate();
                }
        }
}

void UserControlled::autoMove( PlayerItem & player )
{
        // apply effect of bunny of high speed
        float speed = player.getSpeed() / ( player.getHighSpeed() > 0 ? 2 : 1 );

        // is it time to move
        if ( speedTimer->getValue() > speed )
        {
                // move it
                MoveKindOfActivity::getInstance().move( this, &activity, true );

                speedTimer->reset();

                player.animate();

                if ( automaticStepsThruDoor > 0 )
                {
                        -- automaticStepsThruDoor ;
                }
                else
                {
                        // done auto~moving
                        automaticStepsThruDoor = automaticSteps ;
                        activity = Activity::Wait;
                }
        }

        // when done then stop playing sound of auto movement
        if ( activity == Activity::Wait )
        {
                SoundManager::getInstance().stop( player.getLabel(), Activity::AutoMove );
        }
}

void UserControlled::displace( PlayerItem & player )
{
        // this item is moved by another one
        // when displacement couldn’t be performed due to collision then activity propagates to collided items
        if ( speedTimer->getValue() > player.getSpeed() )
        {
                DisplaceKindOfActivity::getInstance().displace( this, &activity, true );

                activity = Activity::Wait;

                speedTimer->reset();
        }
}

void UserControlled::cancelDisplace( PlayerItem & player )
{
        if ( ! player.isFrozen() )
        {
                if ( speedTimer->getValue() > player.getSpeed() )
                {
                        // move it
                        MoveKindOfActivity::getInstance().move( this, &activity, false );

                        speedTimer->reset();

                        player.animate();
                }
        }
}

void UserControlled::fall( PlayerItem & player )
{
        // is it time to lower by one unit
        if ( fallTimer->getValue() > player.getWeight() )
        {
                if ( FallKindOfActivity::getInstance().fall( this ) )
                {
                        // change character’s image to frame of falling when there’s no collision yet
                        if ( player.canAdvanceTo( 0, 0, -1 ) )
                        {
                                player.changeFrame( fallFrames[ player.getOrientation() ] );
                        }
                }
                else if ( activity != Activity::MeetMortalItem || player.hasShield() )
                {
                        activity = Activity::Wait;
                }

                fallTimer->reset();
        }

        if ( activity != Activity::Fall )
        {
                SoundManager::getInstance().stop( player.getLabel(), Activity::Fall );
        }
}

void UserControlled::jump( PlayerItem & player )
{
        switch ( activity )
        {
                case Activity::Jump:
                        // look for item below
                        player.canAdvanceTo( 0, 0, -1 );
                        // when on trampoline or with rabbit of high jumps, player makes big leap
                        activity = ( player.getMediator()->collisionWithByBehavior( "behavior of spring leap" ) != nilPointer ||
                                        ( player.getHighJumps() > 0 && player.getLabel() == "heels" )
                                ? Activity::HighJump
                                : Activity::RegularJump );

                        if ( activity == Activity::HighJump )
                        {
                                if ( player.getLabel() == "heels" )
                                {
                                        player.decreaseHighJumps();
                                }
                                SoundManager::getInstance().play( player.getOriginalLabel(), Activity::Rebound );
                        }
                        break;

                case Activity::RegularJump:
                case Activity::HighJump:
                        // is it time to jump
                        if ( speedTimer->getValue() > player.getSpeed() )
                        {
                                if ( activity == Activity::RegularJump )
                                        JumpKindOfActivity::getInstance().jump( this, &activity, jumpPhase, jumpVector );
                                else if ( activity == Activity::HighJump )
                                        JumpKindOfActivity::getInstance().jump( this, &activity, jumpPhase, highJumpVector );

                                // to next phase of jump
                                ++ jumpPhase ;
                                if ( activity == Activity::Fall ) jumpPhase = 0;

                                speedTimer->reset();

                                player.animate();
                        }
                        break;

                default:
                        ;
        }

        // when jump ends, stop sound of jumping
        if ( activity != Activity::Jump && activity != Activity::RegularJump && activity != Activity::HighJump )
        {
                SoundManager::getInstance().stop( player.getLabel(), Activity::Jump );
        }

        // when player is active and is at maximum height of room it may go to room above
        if ( player.isActiveCharacter() && player.getZ() >= MaxLayers * LayerHeight )
        {
                player.setWayOfExit( "above" );
        }
}

void UserControlled::glide( PlayerItem & player )
{
        if ( glideTimer->getValue() > player.getWeight() /* player.getSpeed() / 2.0 */ )
        {
                // when there’s a collision then stop falling and return to waiting
                if ( ! FallKindOfActivity::getInstance().fall( this ) &&
                        ( activity != Activity::MeetMortalItem || player.hasShield() ) )
                {
                        activity = Activity::Wait;
                }

                glideTimer->reset();
        }

        if ( speedTimer->getValue() > player.getSpeed() * ( player.getLabel() == "headoverheels" ? 2 : 1 ) )
        {
                ActivityOfItem subactivity( Activity::Wait );

                switch ( Way( player.getOrientation() ).getIntegerOfWay () )
                {
                        case Way::North:
                                subactivity = Activity::MoveNorth;
                                break;

                        case Way::South:
                                subactivity = Activity::MoveSouth;
                                break;

                        case Way::East:
                                subactivity = Activity::MoveEast;
                                break;

                        case Way::West:
                                subactivity = Activity::MoveWest;
                                break;

                        default:
                                ;
                }

                MoveKindOfActivity::getInstance().move( this, &subactivity, false );

                // pick picture of falling
                player.changeFrame( fallFrames[ player.getOrientation() ] );

                speedTimer->reset();
        }

        if ( activity != Activity::Glide )
        {
                // not gliding yet, so stop its sound
                SoundManager::getInstance().stop( player.getLabel(), Activity::Glide );
        }
}

void UserControlled::wayInTeletransport( PlayerItem & player )
{
        switch ( activity )
        {
                case Activity::BeginWayInTeletransport:
                        // change to bubbles
                        player.metamorphInto( labelOfBubbles, "begin way in teletransport" );

                        // reverse animation of bubbles
                        player.doBackwardsMotion();

                        activity = Activity::WayInTeletransport;
                        break;

                case Activity::WayInTeletransport:
                        // animate item, player appears in room when animation finishes
                        player.animate() ;

                        if ( player.animationFinished() )
                        {
                                // back to original appearance of player
                                player.metamorphInto( player.getOriginalLabel(), "way in teletransport" );

                                activity = Activity::Wait;
                        }
                        break;

                default:
                        ;
        }
}

void UserControlled::wayOutTeletransport( PlayerItem & player )
{
        switch ( activity )
        {
                case Activity::BeginWayOutTeletransport:
                        // change to bubbles
                        player.metamorphInto( labelOfBubbles, "begin way out teletransport" );

                        activity = Activity::WayOutTeletransport;
                        break;

                case Activity::WayOutTeletransport:
                        // animate item, change room when animation finishes
                        player.animate() ;

                        if ( player.animationFinished() )
                        {
                                player.addToZ( -1 );
                                player.setWayOfExit( player.getMediator()->collisionWithByLabel( "teleport" ) != nilPointer ?
                                        "via teleport" : "via second teleport" );
                        }
                        break;

                default:
                        ;
        }
}

void UserControlled::collideWithMortalItem( PlayerItem & player )
{
        switch ( activity )
        {
                // player just met mortal guy
                case Activity::MeetMortalItem:
                        // do you have immunity
                        if ( ! player.hasShield() )
                        {
                                // change to bubbles retaining player’s label
                                player.metamorphInto( labelOfBubbles, "collide with mortal item" );

                                activity = Activity::Vanish;
                        }
                        else
                        {
                                activity = Activity::Wait;
                        }
                        break;

                case Activity::Vanish:
                        if ( ! player.isActiveCharacter() )
                        {
                                std::cout << "inactive " << player.getUniqueName() << " is going to vanish, activate it" << std::endl ;
                                player.getMediator()->pickNextCharacter();
                        }

                        if ( player.isActiveCharacter() )
                        {
                                player.animate ();
                                if ( player.animationFinished() ) player.loseLife() ;
                        }
                        break;

                default:
                        ;
        }
}

void UserControlled::useHooter( PlayerItem & player )
{
        if ( player.hasTool( "horn" ) && player.getDoughnuts() > 0 )
        {
                this->donutFromHooterIsHere = true;

                const DescriptionOfItem* hooterDoughnut = player.getDescriptionOfItem()->getItemDescriptions()->getDescriptionByLabel( labelOfFiredDoughnut );

                if ( hooterDoughnut != nilPointer )
                {
                        SoundManager::getInstance().stop( player.getLabel(), Activity::FireDoughnut );

                        // create item at the same position as player
                        int z = player.getZ() + player.getHeight() - hooterDoughnut->getHeight();
                        FreeItemPtr donut( new FreeItem (
                                hooterDoughnut,
                                player.getX(), player.getY(),
                                z < 0 ? 0 : z,
                                "none" ) );

                        donut->setBehaviorOf( "behavior of freezing doughnut" );

                        Doughnut * behaviorOfDonut = dynamic_cast< Doughnut * >( donut->getBehavior() );
                        behaviorOfDonut->setPlayerItem( PlayerItemPtr( &player ) );

                        // at first it shares the same position as player so it does not detect collisions
                        donut->setCollisionDetector( false );

                        player.getMediator()->getRoom()->addFreeItem( donut );

                        player.useDoughnut();

                        SoundManager::getInstance().play( player.getOriginalLabel(), Activity::FireDoughnut );
                }
        }
}

void UserControlled::takeItem( PlayerItem & player )
{
        if ( player.hasTool( "handbag" ) )
        {
                Mediator* mediator = player.getMediator();
                ItemPtr takenItem ;

                // look for item below player
                if ( ! player.canAdvanceTo( 0, 0, -1 ) )
                {
                        int coordinates = 0;

                        while ( ! mediator->isStackOfCollisionsEmpty() )
                        {
                                ItemPtr bottomItem = mediator->findCollisionPop( );

                                // choose free pushable item less than or equal to 3/4 of size of one tile
                                if ( bottomItem != nilPointer && bottomItem->getBehavior() != nilPointer
                                        && ( bottomItem->getBehavior()->getNameOfBehavior() == "behavior of thing able to move by pushing" ||
                                                bottomItem->getBehavior()->getNameOfBehavior() == "behavior of big leap for character" )
                                        && bottomItem->getWidthX() <= ( mediator->getRoom()->getSizeOfOneTile() * 3 ) >> 2
                                        && bottomItem->getWidthY() <= ( mediator->getRoom()->getSizeOfOneTile() * 3 ) >> 2 )
                                {
                                        if ( bottomItem->getX() + bottomItem->getY() > coordinates )
                                        {
                                                coordinates = bottomItem->getX() + bottomItem->getY();
                                                takenItem = bottomItem;
                                        }
                                }
                        }

                        // take that item
                        if ( takenItem != nilPointer )
                        {
                                PicturePtr takenItemImage( new Picture( takenItem->getRawImage() ) );

                                GameManager::getInstance().setImageOfItemInBag( takenItemImage );

                                player.placeItemInBag( takenItem->getLabel(), takenItem->getBehavior()->getNameOfBehavior () );

                                takenItem->getBehavior()->changeActivityOfItem( Activity::Vanish );
                                activity = ( activity == Activity::TakeAndJump ? Activity::Jump : Activity::ItemTaken );

                                SoundManager::getInstance().play( player.getOriginalLabel(), Activity::TakeItem );

                                std::cout << "took item \"" << takenItem->getUniqueName() << "\"" << std::endl ;
                        }
                }
        }

        if ( activity != Activity::ItemTaken && activity != Activity::Jump )
        {
                activity = Activity::Wait;
        }
}

void UserControlled::dropItem( PlayerItem & player )
{
        if ( player.getDescriptionOfTakenItem() != nilPointer )
        {
                std::cout << "drop item \"" << player.getDescriptionOfTakenItem()->getLabel() << "\"" << std::endl ;

                // place dropped item just below player
                if ( player.addToZ( LayerHeight ) )
                {
                        FreeItemPtr freeItem( new FreeItem( player.getDescriptionOfTakenItem(),
                                                            player.getX(), player.getY(),
                                                            player.getZ() - LayerHeight,
                                                            "none" ) );

                        freeItem->setBehaviorOf( player.getBehaviorOfTakenItem() );

                        player.getMediator()->getRoom()->addFreeItem( freeItem );

                        GameManager::getInstance().emptyHandbag();

                        player.placeItemInBag( nilPointer, "still" );

                        // update activity
                        activity = ( activity == Activity::DropAndJump ? Activity::Jump : Activity::Wait );

                        SoundManager::getInstance().stop( player.getOriginalLabel(), Activity::Fall );
                        SoundManager::getInstance().play( player.getOriginalLabel(), Activity::DropItem );
                }
                else
                {
                        // emit sound of o~ ou
                        SoundManager::getInstance().play( player.getOriginalLabel(), Activity::Mistake );
                }
        }

        if ( activity != Activity::Jump )
        {
                activity = Activity::Wait;
        }
}

}

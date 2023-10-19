
#include "PlayerControlled.hpp"

#include "AvatarItem.hpp"
#include "Isomot.hpp"
#include "DescriptionOfItem.hpp"
#include "ItemDescriptions.hpp"
#include "Room.hpp"
#include "Mediator.hpp"
#include "MoveKindOfActivity.hpp"
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "JumpKindOfActivity.hpp"
#include "Doughnut.hpp"
#include "GameManager.hpp"
#include "SoundManager.hpp"

using behaviors::PlayerControlled ;


PlayerControlled::PlayerControlled( const ItemPtr & item, const std::string & behavior )
        : Behavior( item, behavior )
        , isLosingLife( false )
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

PlayerControlled::~PlayerControlled()
{
        jumpVector.clear();
        highJumpVector.clear();
}

void PlayerControlled::wait( ::AvatarItem & character )
{
        character.wait();

        if ( activities::FallKindOfActivity::getInstance().fall( this ) )
        {
                speedTimer->reset();
                activity = activities::Activity::Fall;

                if ( character.isHead ()
                                && character.getQuickSteps() > 0
                                        && this->highSpeedSteps < 4 )
                {
                        // reset the double speed steps counter
                        this->highSpeedSteps = 0;
                }
        }
}

void PlayerControlled::move( ::AvatarItem & character )
{
        // move when character isn’t frozen
        if ( ! character.isFrozen() )
        {
                // apply the effect of quick steps bonus bunny
                double speed = character.getSpeed() / ( character.getQuickSteps() > 0 ? 2 : 1 );

                // is it time to move
                if ( speedTimer->getValue() > speed )
                {
                        // move it
                        bool moved = activities::MoveKindOfActivity::getInstance().move( this, &activity, true );

                        // decrement the quick steps
                        if ( character.getQuickSteps() > 0
                                        && moved && activity != activities::Activity::Fall )
                        {
                                this->highSpeedSteps ++ ;

                                if ( this->highSpeedSteps == 8 )
                                {
                                        character.decrementBonusQuickSteps () ;
                                        this->highSpeedSteps = 4 ;
                                }
                        }

                        speedTimer->reset();

                        character.animate();
                }
        }
}

void PlayerControlled::autoMove( ::AvatarItem & character )
{
        // apply the effect of quick steps bonus bunny
        double speed = character.getSpeed() / ( character.getQuickSteps() > 0 ? 2 : 1 );

        // is it time to move
        if ( speedTimer->getValue() > speed )
        {
                // move it
                activities::MoveKindOfActivity::getInstance().move( this, &activity, true );

                speedTimer->reset();

                character.animate();

                if ( automaticStepsThruDoor > 0 )
                {
                        -- automaticStepsThruDoor ;
                }
                else
                {
                        // done auto~moving
                        automaticStepsThruDoor = automaticSteps ;
                        activity = activities::Activity::Wait;
                }
        }

        if ( activity == activities::Activity::Wait )
        {       // stop playing the sound of auto-moving
                SoundManager::getInstance().stop( character.getOriginalLabel(), activities::Activity::AutoMove );
        }
}

void PlayerControlled::displace( ::AvatarItem & character )
{
        // this item is moved by another one
        // when the displacement couldn’t be performed due to a collision then the activity propagates to the collided items
        if ( speedTimer->getValue() > character.getSpeed() )
        {
                activities::DisplaceKindOfActivity::getInstance().displace( this, &activity, true );

                activity = activities::Activity::Wait;

                speedTimer->reset();
        }
}

void PlayerControlled::cancelDisplace( ::AvatarItem & character )
{
        if ( ! character.isFrozen() )
        {
                if ( speedTimer->getValue() > character.getSpeed() )
                {
                        // move it
                        activities::MoveKindOfActivity::getInstance().move( this, &activity, false );

                        speedTimer->reset();

                        character.animate();
                }
        }
}

void PlayerControlled::fall( ::AvatarItem & character )
{
        // is it time to lower by one unit
        if ( fallTimer->getValue() > character.getWeight() )
        {
                if ( activities::FallKindOfActivity::getInstance().fall( this ) )
                {
                        // as long as there's no collision below
                        if ( character.canAdvanceTo( 0, 0, -1 ) )
                        {       // show images of falling character
                                character.changeFrame( fallFrames[ character.getOrientation() ] );
                        }
                }
                else if ( activity != activities::Activity::MeetMortalItem || character.hasShield() )
                {
                        activity = activities::Activity::Wait;
                }

                fallTimer->reset();
        }

        if ( activity != activities::Activity::Fall )
        {
                SoundManager::getInstance().stop( character.getOriginalLabel(), activities::Activity::Fall );
        }
}

void PlayerControlled::jump( ::AvatarItem & character )
{
        switch ( activity )
        {
                case activities::Activity::Jump:
                {
                        // look for an item below
                        character.canAdvanceTo( 0, 0, -1 );

                        bool onASpringStool = ( character.getMediator()->collisionWithByBehavior( "behavior of spring leap" ) != nilPointer );
                        activity = ( onASpringStool || ( character.isHeels() && character.getHighJumps() > 0 )
                                        ? activities::Activity::HighJump // when on a trampoline or has a bonus high jumps
                                        : activities::Activity::RegularJump );

                        if ( activity == activities::Activity::HighJump )
                        {
                                if ( character.isHeels () )
                                {
                                        character.decrementBonusHighJumps () ;
                                }
                                SoundManager::getInstance().play( character.getOriginalLabel(), activities::Activity::Rebound );
                        }
                }
                        break;

                case activities::Activity::RegularJump:
                case activities::Activity::HighJump:
                {
                        // is it time to jump
                        if ( speedTimer->getValue() > character.getSpeed() )
                        {
                                if ( activity == activities::Activity::RegularJump )
                                        activities::JumpKindOfActivity::getInstance().jump( this, &activity, jumpPhase, jumpVector );
                                else if ( activity == activities::Activity::HighJump )
                                        activities::JumpKindOfActivity::getInstance().jump( this, &activity, jumpPhase, highJumpVector );

                                // to the next phase of jump
                                ++ jumpPhase ;
                                if ( activity == activities::Activity::Fall ) jumpPhase = 0;

                                speedTimer->reset();

                                character.animate();
                        }
                }
                        break;

                default:
                        ;
        }

        // when jump ends, stop sound of jumping
        if ( activity != activities::Activity::Jump && activity != activities::Activity::RegularJump && activity != activities::Activity::HighJump )
        {
                SoundManager::getInstance().stop( character.getOriginalLabel(), activities::Activity::Jump );
        }

        // when character is active and is at maximum height of room it may go to room above
        if ( character.isActiveCharacter() && character.getZ() >= Isomot::MaxLayers * Isomot::LayerHeight )
        {
                character.setWayOfExit( "above" );
        }
}

void PlayerControlled::glide( ::AvatarItem & character )
{
        if ( glideTimer->getValue() > character.getWeight() /* character.getSpeed() / 2.0 */ )
        {
                // when there’s a collision then stop falling and return to waiting
                if ( ! activities::FallKindOfActivity::getInstance().fall( this ) &&
                        ( activity != activities::Activity::MeetMortalItem || character.hasShield() ) )
                {
                        activity = activities::Activity::Wait;
                }

                glideTimer->reset();
        }

        if ( speedTimer->getValue() > character.getSpeed() * ( character.isHeadOverHeels() ? 2 : 1 ) )
        {
                ActivityOfItem subactivity( activities::Activity::Wait );

                switch ( Way( character.getOrientation() ).getIntegerOfWay () )
                {
                        case Way::North:
                                subactivity = activities::Activity::MoveNorth;
                                break;

                        case Way::South:
                                subactivity = activities::Activity::MoveSouth;
                                break;

                        case Way::East:
                                subactivity = activities::Activity::MoveEast;
                                break;

                        case Way::West:
                                subactivity = activities::Activity::MoveWest;
                                break;

                        default:
                                ;
                }

                activities::MoveKindOfActivity::getInstance().move( this, &subactivity, false );

                // pick picture of falling
                character.changeFrame( fallFrames[ character.getOrientation() ] );

                speedTimer->reset();
        }

        if ( activity != activities::Activity::Glide )
        {
                // not gliding yet, so stop its sound
                SoundManager::getInstance().stop( character.getOriginalLabel(), activities::Activity::Glide );
        }
}

void PlayerControlled::wayInTeletransport( ::AvatarItem & character )
{
        switch ( activity )
        {
                case activities::Activity::BeginWayInTeletransport:
                        // change to bubbles
                        character.metamorphInto( labelOfBubbles, "begin way in teletransport" );

                        // reverse animation of bubbles
                        character.doBackwardsMotion();

                        activity = activities::Activity::WayInTeletransport;
                        break;

                case activities::Activity::WayInTeletransport:
                        // animate item, character appears in room when animation finishes
                        character.animate() ;

                        if ( character.animationFinished () )
                        {
                                // back to original appearance of character
                                character.metamorphInto( character.getOriginalLabel(), "way in teletransport" );

                                activity = activities::Activity::Wait;
                        }
                        break;

                default:
                        ;
        }
}

void PlayerControlled::wayOutTeletransport( ::AvatarItem & character )
{
        switch ( activity )
        {
                case activities::Activity::BeginWayOutTeletransport:
                        // change to bubbles
                        character.metamorphInto( labelOfBubbles, "begin way out teletransport" );

                        activity = activities::Activity::WayOutTeletransport;
                        break;

                case activities::Activity::WayOutTeletransport:
                        // animate item, change room when animation finishes
                        character.animate() ;

                        if ( character.animationFinished () )
                        {
                                character.addToZ( -1 );
                                character.setWayOfExit( character.getMediator()->collisionWithByLabel( "teleport" ) != nilPointer ?
                                        "via teleport" : "via second teleport" );
                        }
                        break;

                default:
                        ;
        }
}

void PlayerControlled::collideWithMortalItem( ::AvatarItem & character )
{
        switch ( activity )
        {
                // character just met mortal guy
                case activities::Activity::MeetMortalItem:
                        // do you have immunity
                        if ( ! character.hasShield() )
                        {
                                // change to bubbles retaining character’s label
                                character.metamorphInto( labelOfBubbles, "collide with mortal item" );

                                this->isLosingLife = true ;
                                activity = activities::Activity::Vanish;
                        }
                        else
                        {
                                activity = activities::Activity::Wait;
                        }
                        break;

                case activities::Activity::Vanish:
                        if ( ! character.isActiveCharacter() )
                        {
                                std::cout << "inactive " << character.getUniqueName() << " is going to vanish, activate it" << std::endl ;
                                character.getMediator()->pickNextCharacter();
                        }

                        if ( character.isActiveCharacter() )
                        {
                                character.animate ();
                                if ( character.animationFinished () && this->isLosingLife ) {
                                        character.loseLife() ;
                                        this->isLosingLife = false ;
                                }
                        }
                        break;

                default:
                        ;
        }
}

void PlayerControlled::useHooter( ::AvatarItem & character )
{
        if ( character.hasTool( "horn" ) && character.getDonuts() > 0 )
        {
                this->donutFromHooterIsHere = true;

                const DescriptionOfItem * hooterDoughnut = ItemDescriptions::descriptions().getDescriptionByLabel( labelOfFiredDoughnut );
                if ( hooterDoughnut != nilPointer )
                {
                        SoundManager::getInstance().stop( character.getOriginalLabel(), activities::Activity::FireDoughnut );

                        // create item at the same position as the character
                        int z = character.getZ() + character.getHeight() - hooterDoughnut->getHeight();
                        FreeItemPtr donut( new FreeItem (
                                hooterDoughnut,
                                character.getX(), character.getY(),
                                z < 0 ? 0 : z,
                                character.getOrientation () ) );

                        donut->setBehaviorOf( "behavior of freezing doughnut" );

                        Doughnut * behaviorOfDonut = dynamic_cast< Doughnut * >( donut->getBehavior().get () );
                        behaviorOfDonut->setCharacter( AvatarItemPtr( &character ) );

                        // initially the doughnut shares the same position with the character, therefore ignore collisions
                        // COMMENT THIS AND THE GAME CRASHES WHAHA ///////////
                        donut->setIgnoreCollisions( true );

                        character.getMediator()->getRoom()->addFreeItem( donut );

                        character.useDoughnutHorn () ;

                        SoundManager::getInstance().play( character.getOriginalLabel(), activities::Activity::FireDoughnut );
                }
        }
}

void PlayerControlled::takeItem( ::AvatarItem & character )
{
        if ( character.hasTool( "handbag" ) )
        {
                Mediator* mediator = character.getMediator();
                ItemPtr takenItem ;

                // look for an item below the character
                if ( ! character.canAdvanceTo( 0, 0, -1 ) )
                {
                        int coordinates = 0;

                        while ( ! mediator->isStackOfCollisionsEmpty() )
                        {
                                ItemPtr bottomItem = mediator->findCollisionPop( );

                                // choose free pushable item less than or equal to 3/4 of size of one tile
                                if ( bottomItem != nilPointer && bottomItem->getBehavior() != nilPointer
                                        && ( bottomItem->getBehavior()->getNameOfBehavior() == "behavior of thing able to move by pushing" ||
                                                bottomItem->getBehavior()->getNameOfBehavior() == "behavior of spring leap" )
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

                                character.placeItemInBag( takenItem->getLabel(), takenItem->getBehavior()->getNameOfBehavior () );

                                takenItem->getBehavior()->changeActivityOfItem( activities::Activity::Vanish );
                                activity = ( activity == activities::Activity::TakeAndJump ? activities::Activity::Jump : activities::Activity::ItemTaken );

                                SoundManager::getInstance().play( character.getOriginalLabel(), activities::Activity::TakeItem );

                                std::cout << "took item \"" << takenItem->getUniqueName() << "\"" << std::endl ;
                        }
                }
        }

        if ( activity != activities::Activity::ItemTaken && activity != activities::Activity::Jump )
        {
                activity = activities::Activity::Wait;
        }
}

void PlayerControlled::dropItem( ::AvatarItem & character )
{
        if ( character.getDescriptionOfTakenItem() != nilPointer )
        {
                std::cout << "drop item \"" << character.getDescriptionOfTakenItem()->getLabel() << "\"" << std::endl ;

                // place a dropped item just below the character
                if ( character.addToZ( Isomot::LayerHeight ) )
                {
                        FreeItemPtr freeItem( new FreeItem( character.getDescriptionOfTakenItem(),
                                                            character.getX(), character.getY(),
                                                            character.getZ() - Isomot::LayerHeight,
                                                            "none" ) );

                        freeItem->setBehaviorOf( character.getBehaviorOfTakenItem() );

                        character.getMediator()->getRoom()->addFreeItem( freeItem );

                        GameManager::getInstance().emptyHandbag () ;

                        character.emptyBag();

                        // update activity
                        activity = ( activity == activities::Activity::DropAndJump ? activities::Activity::Jump : activities::Activity::Wait );

                        SoundManager::getInstance().stop( character.getOriginalLabel(), activities::Activity::Fall );
                        SoundManager::getInstance().play( character.getOriginalLabel(), activities::Activity::DropItem );
                }
                else
                {
                        // emit sound of o~ ou
                        SoundManager::getInstance().play( character.getOriginalLabel(), activities::Activity::Mistake );
                }
        }

        if ( activity != activities::Activity::Jump )
        {
                activity = activities::Activity::Wait;
        }
}

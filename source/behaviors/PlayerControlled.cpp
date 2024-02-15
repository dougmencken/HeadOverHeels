
#include "PlayerControlled.hpp"

#include "AvatarItem.hpp"
#include "DescriptionOfItem.hpp"
#include "ItemDescriptions.hpp"
#include "Room.hpp"
#include "Mediator.hpp"
#include "Moving.hpp"
#include "Displacing.hpp"
#include "Falling.hpp"
#include "Jumping.hpp"
#include "Doughnut.hpp"
#include "GameManager.hpp"
#include "SoundManager.hpp"

using behaviors::PlayerControlled ;


PlayerControlled::PlayerControlled( const ItemPtr & item, const std::string & behavior )
        : Behavior( item, behavior )
        , isLosingLife( false )
        , jumpPhase( -1 )
        , highJump( false )
        , automaticSteps( 16 /* pasos automáticos */ )
        , automaticStepsThruDoor( automaticSteps )
        , highSpeedSteps( 0 )
        , shieldSteps( 0 )
        , kindOfBubbles( "bubbles" )
        , kindOfFiredDoughnut( "bubbles" )
        , speedTimer( new Timer () )
        , fallTimer( new Timer () )
        , glideTimer( new Timer () )
        , timerForBlinking( new Timer () )
        , donutFromHooterInRoom( false )
{

}

PlayerControlled::~PlayerControlled()
{
        jumpVector.clear();
        highJumpVector.clear();
}

bool PlayerControlled::isInvulnerableToLethalItems () const
{
        return ( dynamic_cast< const ::AvatarItem & >( * this->item ) ).hasShield ()
                        || GameManager::getInstance().isImmuneToCollisionsWithMortalItems () ;
}

void PlayerControlled::setActivityOfItem ( const ActivityOfItem & newActivity )
{
        if ( this->affectedBy != nilPointer ) this->affectedBy = ItemPtr () ;

        if ( newActivity == activities::Activity::MeetMortalItem && isInvulnerableToLethalItems () ) return ;

        this->activity = newActivity ;
}

void PlayerControlled::changeActivityOfItemDueTo ( const ActivityOfItem & newActivity, const ItemPtr & dueTo )
{
        if ( newActivity == activities::Activity::MeetMortalItem && isInvulnerableToLethalItems () ) return ;

        this->activity = newActivity ;
        this->affectedBy = dueTo ;
}

void PlayerControlled::wait( ::AvatarItem & character )
{
        character.wait();

        if ( activities::Falling::getInstance().fall( this ) )
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
                        bool moved = activities::Moving::getInstance().move( this, &activity, true );

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
                activities::Moving::getInstance().move( this, &activity, true );

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
        {       // stop playing the sound of moving
                SoundManager::getInstance().stop( character.getOriginalKind(), "move" );
        }
}

void PlayerControlled::displace( ::AvatarItem & character )
{
        // the character is moved by another item
        // when the displacement couldn’t be performed due to a collision then the activity propagates to the collided items
        if ( speedTimer->getValue() > character.getSpeed() )
        {
                activities::Displacing::getInstance().displace( this, &activity, true );

                activity = activities::Activity::Wait;

                speedTimer->reset();
        }
}

void PlayerControlled::cancelDragging( ::AvatarItem & character )
{
        if ( ! character.isFrozen() )
        {
                if ( speedTimer->getValue() > character.getSpeed() )
                {
                        // move it
                        activities::Moving::getInstance().move( this, &activity, false );

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
                if ( activities::Falling::getInstance().fall( this ) )
                {
                        // as long as there's no collision below
                        if ( character.canAdvanceTo( 0, 0, -1 ) )
                        {       // show images of falling character
                                character.changeFrame( fallFrames[ character.getOrientation() ] );
                        }
                }
                else if ( activity != activities::Activity::MeetMortalItem || character.hasShield() )
                {
                        activity = activities::Activity::Wait ;
                }

                fallTimer->reset();
        }

        if ( activity != activities::Activity::Fall )
        {
                SoundManager::getInstance().stop( character.getOriginalKind(), "fall" );
        }
}

void PlayerControlled::jump( ::AvatarItem & character )
{
        if ( activity == activities::Activity::Jump )
        {
                if ( jumpPhase < 0 )
                {
                        this->highJump = false ;

                        // look for an item below
                        character.canAdvanceTo( 0, 0, -1 );

                        bool onASpringStool = ( character.getMediator()->collisionWithBehavingAs( "behavior of spring leap" ) != nilPointer );
                        this->highJump = onASpringStool || ( character.isHeels() && character.getHighJumps() > 0 ) ;

                        if ( this->highJump ) {
                                if ( character.isHeels () )
                                        character.decrementBonusHighJumps () ;

                                SoundManager::getInstance().play( character.getOriginalKind(), "rebound" );
                        }

                        jumpPhase = 0 ;
                }

                // is it time to jump
                if ( speedTimer->getValue() > character.getSpeed() )
                {
                        activities::Jumping::getInstance().jump( this, &activity, jumpPhase, this->highJump ? highJumpVector : jumpVector );

                        // to the next phase of jump
                        ++ jumpPhase ;

                        if ( activity == activities::Activity::Fall ) jumpPhase = -1 ; // end of jump

                        speedTimer->reset();

                        character.animate();
                }
        }

        if ( activity != activities::Activity::Jump )
        {       // not jumping, then stop the sound
                SoundManager::getInstance().stop( character.getOriginalKind(), "jump" );
        }

        // when the active character is at the maximum height of room, it moves to the room above
        if ( character.isActiveCharacter() && character.getZ() >= Room::MaxLayers * Room::LayerHeight )
        {
                character.setWayOfExit( "above" );
        }
}

void PlayerControlled::glide( ::AvatarItem & character )
{
        if ( glideTimer->getValue() > character.getWeight() /* character.getSpeed() / 2.0 */ )
        {
                // when there’s a collision then stop falling and return to waiting
                if ( ! activities::Falling::getInstance().fall( this ) &&
                        ( activity != activities::Activity::MeetMortalItem || character.hasShield() ) )
                {
                        activity = activities::Activity::Wait ;
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

                activities::Moving::getInstance().move( this, &subactivity, false );

                // pick picture of falling
                character.changeFrame( fallFrames[ character.getOrientation() ] );

                speedTimer->reset();
        }

        if ( activity != activities::Activity::Glide )
        {       // not gliding yet, so stop the sound
                SoundManager::getInstance().stop( character.getOriginalKind(), "fall" );
        }
}

void PlayerControlled::enterTeletransport( ::AvatarItem & character )
{
	if ( activity != activities::Activity::BeginTeletransportation ) return ;

        // morph into bubbles
        if ( character.getKind() != kindOfBubbles )
                character.metamorphInto( kindOfBubbles, "begin teletransportation" );

        // animate item, change room when the animation finishes
        character.animate() ;
        if ( ! character.animationFinished () ) return ;

        character.addToZ( -1 );
        character.setWayOfExit( character.getMediator()->collisionWithSomeKindOf( "teleport" ) != nilPointer
                                        ? "via teleport" : "via second teleport" );
}

void PlayerControlled::exitTeletransport( ::AvatarItem & character )
{
	if ( activity != activities::Activity::EndTeletransportation ) return ;

        // morph back from bubbles
        if ( character.getKind() != kindOfBubbles ) {
                character.metamorphInto( kindOfBubbles, "backwards motion" );
                character.doBackwardsMotion(); // reverse the animation of bubbles
        }

        // animate item, the character appears in the room when the animation finishes
        character.animate() ;
        if ( ! character.animationFinished () ) return ;

        // back to the original appearance of character
        character.metamorphInto( character.getOriginalKind(), "end teletransportation" );

        activity = activities::Activity::Wait ;
}

void PlayerControlled::collideWithMortalItem( ::AvatarItem & character )
{
        switch ( activity )
        {
                // the character just met something mortal
                case activities::Activity::MeetMortalItem:
                        // do you have immunity
                        if ( ! character.hasShield() )
                        {
                                // change to bubbles
                                character.metamorphInto( kindOfBubbles, "hit something mortal" );

                                this->isLosingLife = true ;
                                activity = activities::Activity::Vanish ;
                        }
                        else
                        {
                                activity = activities::Activity::Wait ;
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
        if ( character.hasTool( "horn" ) && character.getDonuts() > 0 && ! this->donutFromHooterInRoom )
        {
                this->donutFromHooterInRoom = true ;

                const DescriptionOfItem * whatIsDonut = ItemDescriptions::descriptions().getDescriptionByKind( kindOfFiredDoughnut );
                if ( whatIsDonut != nilPointer )
                {
                        SoundManager::getInstance().stop( character.getOriginalKind(), "donut" );

                        // create item at the same position as the character
                        int z = character.getZ() + character.getHeight() - whatIsDonut->getHeight() ;
                        FreeItemPtr donut( new FreeItem (
                                whatIsDonut,
                                character.getX(), character.getY(),
                                z < 0 ? 0 : z,
                                character.getOrientation () ) );

                        donut->setBehaviorOf( "behavior of freezing doughnut" );

                        Doughnut * behaviorOfDonut = dynamic_cast< Doughnut * >( donut->getBehavior().get () );
                        behaviorOfDonut->setCharacter( AvatarItemPtr( &character ) );

                        // initially the doughnut shares the same position as the character, therefore ignore collisions
                        // COMMENT THIS AND THE GAME CRASHES WHAHA ///////////
                        donut->setIgnoreCollisions( true );

                        character.getMediator()->getRoom()->addFreeItem( donut );

                        character.useDoughnutHorn () ;

                        SoundManager::getInstance().play( character.getOriginalKind(), "donut" );
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
                        int whereIsItemToPick = 0 ;

                        while ( ! mediator->isStackOfCollisionsEmpty() )
                        {
                                ItemPtr bottomItem = mediator->findCollisionPop( );

                                // pick a free pushable item less than or equal to 3/4 of the size of one tile
                                if ( bottomItem != nilPointer && bottomItem->getBehavior() != nilPointer
                                        && ( bottomItem->getBehavior()->getNameOfBehavior() == "behavior of thing able to move by pushing" ||
                                                bottomItem->getBehavior()->getNameOfBehavior() == "behavior of spring leap" )
                                        && bottomItem->getUnsignedWidthX() <= ( mediator->getRoom()->getSizeOfOneTile() * 3 ) >> 2
                                        && bottomItem->getUnsignedWidthY() <= ( mediator->getRoom()->getSizeOfOneTile() * 3 ) >> 2 )
                                {
                                        if ( bottomItem->getX() + bottomItem->getY() > whereIsItemToPick )
                                        {
                                                whereIsItemToPick = bottomItem->getX () + bottomItem->getY ();
                                                takenItem = bottomItem ;
                                        }
                                }
                        }

                        // take that item
                        if ( takenItem != nilPointer )
                        {
                                PicturePtr takenItemImage( new Picture( takenItem->getRawImage() ) );

                                GameManager::getInstance().setImageOfItemInBag( takenItemImage );

                                character.placeItemInBag( takenItem->getKind (), takenItem->getBehavior()->getNameOfBehavior () );

                                takenItem->getBehavior()->setActivityOfItem( activities::Activity::Vanish );
                                activity = ( activity == activities::Activity::TakeAndJump )
                                                ? activities::Activity::Jump : activities::Activity::ItemTaken ;

                                SoundManager::getInstance().play( character.getOriginalKind(), "take" );

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
                std::cout << "drop item \"" << character.getDescriptionOfTakenItem()->getKind () << "\"" << std::endl ;

                // place a dropped item just below the character
                if ( character.addToZ( Room::LayerHeight ) )
                {
                        FreeItemPtr freeItem( new FreeItem( character.getDescriptionOfTakenItem(),
                                                            character.getX(), character.getY(),
                                                            character.getZ() - Room::LayerHeight,
                                                            "none" ) );

                        freeItem->setBehaviorOf( character.getBehaviorOfTakenItem() );

                        character.getMediator()->getRoom()->addFreeItem( freeItem );

                        GameManager::getInstance().emptyHandbag () ;

                        character.emptyBag();

                        // update activity
                        activity = ( activity == activities::Activity::DropAndJump ? activities::Activity::Jump : activities::Activity::Wait );

                        SoundManager::getInstance().stop( character.getOriginalKind(), "fall" );
                        SoundManager::getInstance().play( character.getOriginalKind(), "drop" );
                }
                else
                {
                        // emit the sound of o~ ou
                        SoundManager::getInstance().play( character.getOriginalKind(), "mistake" );
                }
        }

        if ( activity != activities::Activity::Jump )
        {
                activity = activities::Activity::Wait;
        }
}

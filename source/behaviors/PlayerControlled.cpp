
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
#include "InputManager.hpp"

using behaviors::PlayerControlled ;


PlayerControlled::PlayerControlled( AvatarItem & item, const std::string & behavior )
        : Behavior( item, behavior )
        , jumpPhase( -1 )
        , bigJump( false )
        , automoveStepsRemained( automatic_steps )
        , quickSteps( 0 )
        , speedTimer( new Timer () )
        , fallTimer( new Timer () )
        , glideTimer( new Timer () )
        , timerForBlinking( new Timer () )
        , isLosingLife( false )
{

}

PlayerControlled::~PlayerControlled( )
{
        this->jumpVector.clear() ;
        this->bigJumpVector.clear() ;
}

bool PlayerControlled::update ()
{
        const Activity & activity = getCurrentActivity() ;

        if ( activity == activities::Activity::BeginTeletransportation )
                enterTeletransport ();
        else
        if ( activity == activities::Activity::EndTeletransportation )
                exitTeletransport ();
        else
        if ( activity == activities::Activity::MetLethalItem || activity == activities::Activity::Vanishing )
                collideWithALethalItem ();

        // play sound for the current activity
        SoundManager::getInstance().play (
                        dynamic_cast< const ::DescribedItem & >( getItem() ).getOriginalKind(),
                        SoundManager::activityToNameOfSound( getCurrentActivity() ) );
        return true ;
}

bool PlayerControlled::isInvulnerableToLethalItems () const
{
        return dynamic_cast< const ::AvatarItem & >( getItem () ).hasShield ()
                        || GameManager::getInstance().isImmuneToCollisionsWithMortalItems () ;
}

void PlayerControlled::setCurrentActivity ( const Activity & newActivity, const Motion2D & velocity )
{
        // nullify “changed due to” item
        Behavior::setCurrentActivity( getCurrentActivity(), velocity );

        if ( newActivity == activities::Activity::MetLethalItem && isInvulnerableToLethalItems () ) return ;

        Behavior::setCurrentActivity( newActivity, velocity );
}

void PlayerControlled::changeActivityDueTo ( const Activity & newActivity, const Motion2D & velocity, const AbstractItemPtr & dueTo )
{
        if ( newActivity == activities::Activity::MetLethalItem && isInvulnerableToLethalItems () ) return ;

        Behavior::changeActivityDueTo( newActivity, velocity, dueTo );
}

void PlayerControlled::wait ()
{
        ::AvatarItem & character = dynamic_cast< ::AvatarItem & >( getItem () );

        character.wait ();

        if ( activities::Falling::getInstance().fall( * this ) )
        {
                speedTimer->go() ;

                setCurrentActivity( activities::Activity::Falling, Motion2D::rest() );

                if ( character.isHead ()
                                && character.howManyBonusQuickSteps() > 0
                                        && this->quickSteps < 4 )
                        this->quickSteps = 0 ; // reset the quick steps counter
        }
}

void PlayerControlled::move ()
{
        ::AvatarItem & character = dynamic_cast< ::AvatarItem & >( getItem () );

        if ( character.isFrozen() ) return ;

        // apply the effect of quick steps bonus bunny
        double speed = character.getSpeed() / ( character.howManyBonusQuickSteps() > 0 ? 2 : 1 );

        // is it time to move
        if ( speedTimer->get() > speed )
        {
                bool moved = activities::Moving::getInstance().move( *this, true );

                // decrement the quick steps
                if ( character.howManyBonusQuickSteps() > 0
                                && moved && getCurrentActivity() != activities::Activity::Falling )
                {
                        this->quickSteps ++ ;

                        if ( this->quickSteps == 8 ) {
                                character.decrementBonusQuickSteps () ;
                                this->quickSteps = 4 ;
                        }
                }

                speedTimer->go() ;

                character.animate();
        }
}

void PlayerControlled::automove ()
{
        ::AvatarItem & character = dynamic_cast< ::AvatarItem & >( getItem () );

        // apply the effect of quick steps bonus bunny
        double speed = character.getSpeed() / ( character.howManyBonusQuickSteps() > 0 ? 2 : 1 );

        // is it time to move
        if ( speedTimer->get() > speed )
        {
                // move it
                activities::Moving::getInstance().move( *this, true );

                speedTimer->go() ;

                character.animate() ;

                if ( this->automoveStepsRemained > 0 )
                {
                        -- this->automoveStepsRemained ;
                }
                else {  // done auto~moving
                        this->automoveStepsRemained = automatic_steps ; // reset remaining steps
                        beWaiting() ;
                }
        }

        if ( getCurrentActivity() == activities::Activity::Waiting )
        {       // stop playing the sound of moving
                SoundManager::getInstance().stop( character.getOriginalKind(), "move" );
        }
}

bool PlayerControlled::moveKeySetsActivity ()
{
        bool anyMoveTyped = false ;
        const InputManager & input = InputManager::getInstance ();

        if ( input.movenorthTyped() ) {
                anyMoveTyped = true ;
                setCurrentActivity( activities::Activity::Moving, Motion2D::movingNorth() );
        }
        else if ( input.movesouthTyped() ) {
                anyMoveTyped = true ;
                setCurrentActivity( activities::Activity::Moving, Motion2D::movingSouth() );
        }
        else if ( input.moveeastTyped() ) {
                anyMoveTyped = true ;
                setCurrentActivity( activities::Activity::Moving, Motion2D::movingEast() );
        }
        else if ( input.movewestTyped() ) {
                anyMoveTyped = true ;
                setCurrentActivity( activities::Activity::Moving, Motion2D::movingWest() );
        }

        return anyMoveTyped ;
}

bool PlayerControlled::moveKeyChangesHeading ()
{
        bool anyMoveTyped = false ;
        const InputManager & input = InputManager::getInstance ();
        ::AvatarItem & avatar = dynamic_cast< ::AvatarItem & >( getItem () );

        std::string headingBefore = avatar.getHeading() ;
        std::string headingAfter = headingBefore ;

        if ( input.movenorthTyped() ) {
                anyMoveTyped = true ;
                headingAfter = "north" ;
        }
        else if ( input.movesouthTyped() ) {
                anyMoveTyped = true ;
                headingAfter = "south" ;
        }
        else if ( input.moveeastTyped() ) {
                anyMoveTyped = true ;
                headingAfter = "east" ;
        }
        else if ( input.movewestTyped() ) {
                anyMoveTyped = true ;
                headingAfter = "west" ;
        }

        if ( anyMoveTyped && headingBefore != headingAfter )
                avatar.changeHeading( headingAfter );

#if defined( DEBUG_ACTIVITIES ) && DEBUG_ACTIVITIES
        if ( anyMoveTyped && headingBefore != avatar.getHeading() )
                std::cout << "in PlayerControlled::moveKeyChangesHeading() "
                                << avatar.getOriginalKind() << "’s heading changed"
                                << " from \"" << headingBefore << "\" to \"" << avatar.getHeading() << "\""
                                << std::endl ;
#endif

        return anyMoveTyped ;
}

void PlayerControlled::displace ()
{
        if ( speedTimer->get() > dynamic_cast< const ::DescribedItem & >( getItem() ).getSpeed() )
        {
                activities::Displacing::getInstance().displace( *this, true );
                // when the displacement couldn’t be performed due to a collision
                // then the activity is propagated to the collided items

                beWaiting() ;

                speedTimer->go() ;
        }
}

void PlayerControlled::handleMoveKeyWhenDragged ()
{
        ::AvatarItem & avatar = dynamic_cast< ::AvatarItem & >( getItem () );

        Activity whatDoing = getCurrentActivity() ;
        Motion2D whereMoving = get2DVelocityVector() ;

        const InputManager & input = InputManager::getInstance ();

        if ( input.movenorthTyped() ) {
                avatar.changeHeading( "north" );

                setCurrentActivity(
                        activities::Activity::Moving,
                        ( whatDoing == activities::Activity::Dragged && whereMoving.isMovingOnlySouth() )
                                ? /* cancel draggin’ */ Motion2D::rest()
                                : whereMoving.add( Motion2D::movingNorth() ) );
        }
        else if ( input.movesouthTyped() ) {
                avatar.changeHeading( "south" );

                setCurrentActivity(
                        activities::Activity::Moving,
                        ( whatDoing == activities::Activity::Dragged && whereMoving.isMovingOnlyNorth() )
                                ? /* cancel draggin’ */ Motion2D::rest()
                                : whereMoving.add( Motion2D::movingSouth() ) );
        }
        else if ( input.moveeastTyped() ) {
                avatar.changeHeading( "east" );

                setCurrentActivity(
                        activities::Activity::Moving,
                        ( whatDoing == activities::Activity::Dragged && whereMoving.isMovingOnlyWest() )
                                ? /* cancel draggin’ */ Motion2D::rest()
                                : whereMoving.add( Motion2D::movingEast() ) );
        }
        else if ( input.movewestTyped() ) {
                avatar.changeHeading( "west" );

                setCurrentActivity(
                        activities::Activity::Moving,
                        ( whatDoing == activities::Activity::Dragged && whereMoving.isMovingOnlyEast() )
                                ? /* cancel draggin’ */ Motion2D::rest()
                                : whereMoving.add( Motion2D::movingWest() ) );
        }
}

void PlayerControlled::fall ()
{
        ::AvatarItem & character = dynamic_cast< ::AvatarItem & >( getItem () );

        if ( fallTimer->get() > character.getWeight() )
        {
                if ( activities::Falling::getInstance().fall( *this ) ) {
                        if ( character.canAdvanceTo( 0, 0, -1 ) /* there’s nothing below the character */ )
                                character.setSequenceAndFrame( fallFrames[ character.getHeading() ].first,
                                                                fallFrames[ character.getHeading() ].second );
                }
                else if ( getCurrentActivity() != activities::Activity::MetLethalItem || isInvulnerableToLethalItems() )
                        beWaiting() ;

                fallTimer->go() ;
        }

        if ( getCurrentActivity() != activities::Activity::Falling )
                SoundManager::getInstance().stop( character.getOriginalKind(), "fall" );
}

void PlayerControlled::glide ()
{
        ::AvatarItem & character = dynamic_cast< ::AvatarItem & >( getItem () );

        if ( glideTimer->get() > character.getWeight() )
        {
                if ( ! activities::Falling::getInstance().fall( * this ) &&
                        ( getCurrentActivity() != activities::Activity::MetLethalItem || isInvulnerableToLethalItems() ) )
                {
                        // not falling, back to waiting
                        beWaiting() ;
                }

                glideTimer->go() ;
        }

        if ( speedTimer->get() > character.getSpeed() * ( character.isHeadOverHeels() ? 2 : 1 ) )
        {
                const Activity & priorActivity = getCurrentActivity() ;
                const Motion2D & priorMotion = get2DVelocityVector() ;

                setCurrentActivity( activities::Activity::Moving, character.getHeading() );
                activities::Moving::getInstance().move( *this, false );

                setCurrentActivity( priorActivity, priorMotion );

                // may turn while gliding so update the frame of falling
                character.setSequenceAndFrame( fallFrames[ character.getHeading() ].first,
                                                fallFrames[ character.getHeading() ].second );

                speedTimer->go() ;
        }

        // unlike falling, gliding doesn’t accelerate over time
        resetHowLongFalls() ;

        if ( getCurrentActivity() != activities::Activity::Gliding )
                SoundManager::getInstance().stop( character.getOriginalKind(), "fall" );
}

void PlayerControlled::toJumpOrTeleport ()
{
        ::AvatarItem & character = dynamic_cast< ::AvatarItem & >( getItem () );

        // is there a device underneath the character
        character.canAdvanceTo( 0, 0, -1 );
        Activity jumpOrTeleport =
                        ( character.getMediator()->collisionWithBehavingAs( "behavior of teletransport" ) != nilPointer )
                                        ? activities::Activity::BeginTeletransportation
                                        : activities::Activity::Jumping ;

        setCurrentActivity( jumpOrTeleport, Motion2D::rest() );
}

void PlayerControlled::jump ()
{
        ::AvatarItem & character = dynamic_cast< ::AvatarItem & >( getItem () );

        if ( getCurrentActivity() == activities::Activity::Jumping )
        {
                if ( this->jumpPhase < 0 )
                {
                        // look for an item below
                        character.canAdvanceTo( 0, 0, -1 );

                        bool onASpringStool = ( character.getMediator()->collisionWithBehavingAs( "behavior of spring stool" ) != nilPointer );
                        bool willJumpHigher = ( character.isHeels() && character.howManyBonusBigJumps() > 0 ) ;

                        if ( onASpringStool )
                                SoundManager::getInstance().play( "spring-stool", "bounce" );
                        else
                        if ( willJumpHigher ) {
                                SoundManager::getInstance().play( "heels", "bigjump" );
                                character.decrementBonusBigJumps () ;
                        }

                        this->jumpPhase = 0 ;
                        this->bigJump = onASpringStool || willJumpHigher ;
                }

                // is it time to jump
                if ( speedTimer->get() > character.getSpeed() )
                {
                        activities::Jumping::getInstance().jump( *this, this->jumpPhase, this->bigJump ? this->bigJumpVector : this->jumpVector );

                        // to the next phase of jump
                        ++ this->jumpPhase ;

                        if ( getCurrentActivity() == activities::Activity::Falling ) this->jumpPhase = -1 ; // end of jump

                        speedTimer->go() ;

                        character.animate() ;
                }
        }

        if ( getCurrentActivity() != activities::Activity::Jumping )
                // not jumping yet stop the sound
                SoundManager::getInstance().stop( character.getOriginalKind(), "jump" );

        // when active character reaches the room’s maximum height
        if ( character.isActiveCharacter() && character.getZ() >= Room::MaxLayers * Room::LayerHeight )
                // go to the room above
                character.setWayOfExit( "above" );
}

void PlayerControlled::enterTeletransport ()
{
        if ( getCurrentActivity() != activities::Activity::BeginTeletransportation ) return ;

        ::AvatarItem & character = dynamic_cast< ::AvatarItem & >( getItem () );

        // morph into bubbles
        if ( ! character.isMetamorphed () )
                character.metamorphInto( "bubbles", "begin teletransportation" );

        // animate item, change room when the animation finishes
        character.animate() ;
        if ( ! character.isAnimationFinished () ) return ;

        character.addToZ( -1 ); // which teleport is below
        character.setWayOfExit( character.getMediator()->collisionWithSomeKindOf( "teleport" ) != nilPointer
                                        ? "via teleport" : "via second teleport" );
}

void PlayerControlled::exitTeletransport ()
{
        if ( getCurrentActivity() != activities::Activity::EndTeletransportation ) return ;

        ::AvatarItem & character = dynamic_cast< ::AvatarItem & >( getItem () );

        // morph back from bubbles
        if ( ! character.isMetamorphed () ) {
                character.metamorphInto( "bubbles", "backwards motion" );
                character.doBackwardsMotion(); // reverse the animation of bubbles
        }

        // animate item, the character appears in the room when the animation finishes
        character.animate() ;
        if ( ! character.isAnimationFinished () ) return ;

        // back to the original appearance of character
        character.metamorphInto( character.getOriginalKind(), "end teletransportation" );

        beWaiting() ;
}

void PlayerControlled::collideWithALethalItem ()
{
        ::AvatarItem & character = dynamic_cast< ::AvatarItem & >( getItem () );

        switch ( getCurrentActivity () )
        {
                // the character just met something lethal
                case activities::Activity::MetLethalItem:
                        if ( ! isInvulnerableToLethalItems() ) {
                                // change to bubbles
                                character.metamorphInto( "bubbles", "met something lethal" );

                                this->isLosingLife = true ;
                                setCurrentActivity( activities::Activity::Vanishing, Motion2D::rest() );
                        } else
                                beWaiting() ;

                        break;

                case activities::Activity::Vanishing:
                        if ( ! character.isActiveCharacter() ) {
                                std::cout << "inactive " << character.getUniqueName() << " is going to vanish, activate it" << std::endl ;
                                character.getMediator()->pickNextCharacter();
                        }

                        if ( character.isActiveCharacter() ) {
                                character.animate ();
                                if ( character.isAnimationFinished () && this->isLosingLife ) {
                                        character.loseLife() ;
                                        this->isLosingLife = false ;
                                }
                        }

                        break;

                default:
                        ;
        }
}

void PlayerControlled::useHooter ()
{
        ::AvatarItem & character = dynamic_cast< ::AvatarItem & >( getItem () );

        // can only release one doughnut in the room at a time
        bool isDonutInRoom = ( character.getMediator()->findItemBehavingAs( "behavior of freezing doughnut" ) != nilPointer );

        if ( character.hasTool( "horn" ) && character.getDonuts() > 0 && ! isDonutInRoom )
        {
                const DescriptionOfItem * whatIsDonut = ItemDescriptions::descriptions().getDescriptionByKind( "bubbles" );
                if ( whatIsDonut != nilPointer )
                {
                        SoundManager::getInstance().stop( character.getOriginalKind(), "donut" );

                        // create item at the same position as the character
                        int z = character.getZ() + character.getHeight() - whatIsDonut->getHeight() ;
                        FreeItemPtr donut( new FreeItem (
                                * whatIsDonut,
                                character.getX(), character.getY(),
                                z < 0 ? 0 : z,
                                character.getHeading () ) );

                        donut->setBehaviorOf( "behavior of freezing doughnut" );

                        // initially the doughnut shares the same position as the character, therefore ignore collisions
                        // COMMENT THIS AND THE GAME CRASHES WHAHA ///////////
                        donut->setIgnoreCollisions( true );

                        character.getMediator()->getRoom().addFreeItem( donut );

                        character.useDoughnutHorn () ;

                        SoundManager::getInstance().play( character.getOriginalKind(), "donut" );
                }
        }
}

void PlayerControlled::takeItem ()
{
        ::AvatarItem & character = dynamic_cast< ::AvatarItem & >( getItem () );

        if ( character.hasTool( "handbag" ) )
        {
                Mediator* mediator = character.getMediator();
                DescribedItemPtr itemToTake ;

                // look for an item below the character
                if ( ! character.canAdvanceTo( 0, 0, -1 ) )
                {
                        int whereIsItemToPick = 0 ;

                        while ( mediator->isThereAnyCollision() )
                        {
                                DescribedItemPtr belowItem = mediator->findCollisionPop( );

                                // pick a free pushable item less than or equal to 3/4 of the size of one tile
                                if ( belowItem != nilPointer && belowItem->getBehavior() != nilPointer
                                        && ( belowItem->getBehavior()->getNameOfBehavior() == "behavior of thing able to move by pushing" ||
                                                belowItem->getBehavior()->getNameOfBehavior() == "behavior of spring stool" )
                                        && belowItem->getUnsignedWidthX() <= ( mediator->getRoom().getSizeOfOneTile() * 3 ) >> 2
                                        && belowItem->getUnsignedWidthY() <= ( mediator->getRoom().getSizeOfOneTile() * 3 ) >> 2 )
                                {
                                        if ( belowItem->getX() + belowItem->getY() > whereIsItemToPick ) {
                                                whereIsItemToPick = belowItem->getX() + belowItem->getY() ;
                                                itemToTake = belowItem ;
                                        }
                                }
                        }

                        // take that item
                        if ( itemToTake != nilPointer )
                        {
                                std::cout << "taking item \"" << itemToTake->getUniqueName() << "\"" << std::endl ;

                                character.putItemInTheBag( itemToTake->getKind (), itemToTake->getBehavior()->getNameOfBehavior () );
                                GameManager::getInstance().setImageOfItemInBag (PicturePtr( new Picture( itemToTake->getCurrentRawImage() ) ));

                                itemToTake->getBehavior()->setCurrentActivity( activities::Activity::Vanishing, Motion2D::rest() );

                                Activity jumpOrFall = ( getCurrentActivity() == activities::Activity::TakeAndJump )
                                                                ? activities::Activity::Jumping
                                                                : activities::Activity::Falling ;
                                setCurrentActivity( jumpOrFall, Motion2D::rest() );

                                SoundManager::getInstance().play( character.getOriginalKind(), "take" );
                        }
                }
        }

        if ( getCurrentActivity() == activities::Activity::TakingItem || getCurrentActivity() == activities::Activity::TakeAndJump )
                // wait if can’t take
                beWaiting() ;
                // moreover, the original game plays a yucky sound for that
}

void PlayerControlled::dropItem ()
{
        ::AvatarItem & character = dynamic_cast< ::AvatarItem & >( getItem () );

        if ( character.getDescriptionOfTakenItem() != nilPointer )
        {
                std::cout << "dropping item \"" << character.getDescriptionOfTakenItem()->getKind () << "\"" << std::endl ;

                // place a dropped item just below the character
                if ( character.addToZ( Room::LayerHeight ) )
                {
                        FreeItemPtr freeItem( new FreeItem( * character.getDescriptionOfTakenItem(),
                                                            character.getX(), character.getY(),
                                                            character.getZ() - Room::LayerHeight ) );
                        freeItem->setBehaviorOf( character.getBehaviorOfTakenItem() );

                        character.getMediator()->getRoom().addFreeItem( freeItem );

                        GameManager::getInstance().emptyHandbag () ;
                        character.emptyTheBag ();

                        Activity jumpOrWait = ( getCurrentActivity() == activities::Activity::DropAndJump )
                                                        ? activities::Activity::Jumping
                                                        : activities::Activity::Waiting ;
                        setCurrentActivity( jumpOrWait, Motion2D::rest() ) ;

                        SoundManager::getInstance().stop( character.getOriginalKind(), "fall" );
                        SoundManager::getInstance().play( character.getOriginalKind(), "drop" );
                }
                else {
                        // emit the sound of o~ ou
                        SoundManager::getInstance().play( character.getOriginalKind(), "mistake" );
                }
        }

        if ( getCurrentActivity() == activities::Activity::DroppingItem || getCurrentActivity() == activities::Activity::DropAndJump )
                // wait if can’t drop
                beWaiting() ;
}


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
        , highJump( false )
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
        jumpVector.clear() ;
        highJumpVector.clear() ;
}

bool PlayerControlled::update ()
{
        AvatarItem & avatar = dynamic_cast< AvatarItem & >( getItem() );

        if ( avatar.hasShield() ) avatar.decrementShieldOverTime () ;

        // play sound for the current activity
        SoundManager::getInstance().play( avatar.getOriginalKind(), SoundManager::activityToNameOfSound( getCurrentActivity() ) );

        return true ;
}

bool PlayerControlled::isInvulnerableToLethalItems () const
{
        return ( dynamic_cast< const ::AvatarItem & >( getItem () ) ).hasShield ()
                        || GameManager::getInstance().isImmuneToCollisionsWithMortalItems () ;
}

void PlayerControlled::setCurrentActivity ( const Activity & newActivity )
{
        // nullify “changed due to” item
        Behavior::setCurrentActivity( getCurrentActivity() );

        if ( newActivity == activities::Activity::MetLethalItem && isInvulnerableToLethalItems () ) return ;

        Behavior::setCurrentActivity( newActivity );
}

void PlayerControlled::changeActivityDueTo ( const Activity & newActivity, const ItemPtr & dueTo )
{
        if ( newActivity == activities::Activity::MetLethalItem && isInvulnerableToLethalItems () ) return ;

        Behavior::changeActivityDueTo( newActivity, dueTo );
}

void PlayerControlled::wait ()
{
        ::AvatarItem & character = dynamic_cast< ::AvatarItem & >( getItem () );

        character.wait ();

        if ( activities::Falling::getInstance().fall( * this ) )
        {
                speedTimer->go() ;

                setCurrentActivity( activities::Activity::Falling );

                if ( character.isHead ()
                                && character.getQuickSteps() > 0
                                        && this->quickSteps < 4 )
                        this->quickSteps = 0 ; // reset the quick steps counter
        }
}

void PlayerControlled::move ()
{
        ::AvatarItem & character = dynamic_cast< ::AvatarItem & >( getItem () );

        if ( character.isFrozen() ) return ;

        // apply the effect of quick steps bonus bunny
        double speed = character.getSpeed() / ( character.getQuickSteps() > 0 ? 2 : 1 );

        // is it time to move
        if ( speedTimer->getValue() > speed )
        {
                bool moved = activities::Moving::getInstance().move( *this, true );

                // decrement the quick steps
                if ( character.getQuickSteps() > 0
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
        double speed = character.getSpeed() / ( character.getQuickSteps() > 0 ? 2 : 1 );

        // is it time to move
        if ( speedTimer->getValue() > speed )
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
                        this->automoveStepsRemained = automatic_steps ; // reset remained steps
                        setCurrentActivity( activities::Activity::Waiting );
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
                setCurrentActivity( activities::Activity::MovingNorth );
        }
        else if ( input.movesouthTyped() ) {
                anyMoveTyped = true ;
                setCurrentActivity( activities::Activity::MovingSouth );
        }
        else if ( input.moveeastTyped() ) {
                anyMoveTyped = true ;
                setCurrentActivity( activities::Activity::MovingEast );
        }
        else if ( input.movewestTyped() ) {
                anyMoveTyped = true ;
                setCurrentActivity( activities::Activity::MovingWest );
        }

        return anyMoveTyped ;
}

bool PlayerControlled::moveKeyChangesHeading ()
{
        bool anyMoveTyped = false ;
        const InputManager & input = InputManager::getInstance ();
        ::AvatarItem & avatar = dynamic_cast< ::AvatarItem & >( getItem () );

        if ( input.movenorthTyped() ) {
                anyMoveTyped = true ;
                avatar.changeHeading( "north" );
        }
        else if ( input.movesouthTyped() ) {
                anyMoveTyped = true ;
                avatar.changeHeading( "south" );
        }
        else if ( input.moveeastTyped() ) {
                anyMoveTyped = true ;
                avatar.changeHeading( "east" );
        }
        else if ( input.movewestTyped() ) {
                anyMoveTyped = true ;
                avatar.changeHeading( "west" );
        }

        return anyMoveTyped ;
}

void PlayerControlled::displace ()
{
        if ( speedTimer->getValue() > getItem().getSpeed() )
        {
                activities::Displacing::getInstance().displace( *this, true );
                // when the displacement couldn’t be performed due to a collision
                // then the activity is propagated to the collided items

                setCurrentActivity( activities::Activity::Waiting );

                speedTimer->go() ;
        }
}

void PlayerControlled::handleMoveKeyWhenDragged ()
{
        const InputManager & input = InputManager::getInstance ();
        ::AvatarItem & avatar = dynamic_cast< ::AvatarItem & >( getItem () );
        Activity whatDoing = getCurrentActivity() ;

        if ( input.movenorthTyped() ) {
                avatar.changeHeading( "north" );
                setCurrentActivity( whatDoing == activities::Activity::DraggedSouth
                                        ? activities::Activity::CancelDragging
                                        : activities::Activity::MovingNorth );
        }
        else if ( input.movesouthTyped() ) {
                avatar.changeHeading( "south" );
                setCurrentActivity( whatDoing == activities::Activity::DraggedNorth
                                        ? activities::Activity::CancelDragging
                                        : activities::Activity::MovingSouth );
        }
        else if ( input.moveeastTyped() ) {
                avatar.changeHeading( "east" );
                setCurrentActivity( whatDoing == activities::Activity::DraggedWest
                                        ? activities::Activity::CancelDragging
                                        : activities::Activity::MovingEast );
        }
        else if ( input.movewestTyped() ) {
                avatar.changeHeading( "west" );
                setCurrentActivity( whatDoing == activities::Activity::DraggedEast
                                        ? activities::Activity::CancelDragging
                                        : activities::Activity::MovingWest );
        }
}

void PlayerControlled::cancelDragging ()
{
        ::AvatarItem & character = dynamic_cast< ::AvatarItem & >( getItem () );

        if ( ! character.isFrozen() )
        {
                if ( speedTimer->getValue() > character.getSpeed() ) {
                        activities::Moving::getInstance().move( *this, false );

                        speedTimer->go() ;

                        character.animate ();
                }
        }
}

void PlayerControlled::fall ()
{
        ::AvatarItem & character = dynamic_cast< ::AvatarItem & >( getItem () );

        if ( fallTimer->getValue() > character.getWeight() )
        {
                if ( activities::Falling::getInstance().fall( *this ) ) {
                        if ( character.canAdvanceTo( 0, 0, -1 ) /* there’s nothing below the character */ )
                                character.changeFrame( fallFrames[ character.getHeading () ] );
                }
                else if ( getCurrentActivity() != activities::Activity::MetLethalItem || isInvulnerableToLethalItems() )
                        setCurrentActivity( activities::Activity::Waiting );

                fallTimer->go() ;
        }

        if ( getCurrentActivity() != activities::Activity::Falling )
                SoundManager::getInstance().stop( character.getOriginalKind(), "fall" );
}

void PlayerControlled::glide ()
{
        ::AvatarItem & character = dynamic_cast< ::AvatarItem & >( getItem () );

        if ( glideTimer->getValue() > character.getWeight() )
        {
                if ( ! activities::Falling::getInstance().fall( * this ) &&
                        ( getCurrentActivity() != activities::Activity::MetLethalItem || isInvulnerableToLethalItems() ) )
                {
                        // not falling, back to waiting
                        setCurrentActivity( activities::Activity::Waiting );
                }

                glideTimer->go() ;
        }

        if ( speedTimer->getValue() > character.getSpeed() * ( character.isHeadOverHeels() ? 2 : 1 ) )
        {
                Activity priorActivity = getCurrentActivity() ;
                Activity glideActivity = activities::Activity::Waiting ;

                const std::string & heading = character.getHeading ();
                if ( heading == "north" )
                        glideActivity = activities::Activity::MovingNorth ;
                else
                if ( heading == "south" )
                        glideActivity = activities::Activity::MovingSouth ;
                else
                if ( heading == "east" )
                        glideActivity = activities::Activity::MovingEast ;
                else
                if ( heading == "west" )
                        glideActivity = activities::Activity::MovingWest ;

                setCurrentActivity( glideActivity );
                activities::Moving::getInstance().move( *this, false );

                setCurrentActivity( priorActivity );

                // may turn while gliding so update the frame of falling
                character.changeFrame( fallFrames[ character.getHeading() ] );

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

        character.canAdvanceTo( 0, 0, -1 ); // is there a device underneath the character
        setCurrentActivity( character.getMediator()->collisionWithBehavingAs( "behavior of teletransport" ) != nilPointer
                                        ? activities::Activity::BeginTeletransportation
                                        : activities::Activity::Jumping );
}

void PlayerControlled::jump ()
{
        ::AvatarItem & character = dynamic_cast< ::AvatarItem & >( getItem () );

        if ( getCurrentActivity() == activities::Activity::Jumping )
        {
                if ( this->jumpPhase < 0 )
                {
                        this->highJump = false ;

                        // look for an item below
                        character.canAdvanceTo( 0, 0, -1 );

                        bool onASpringStool = ( character.getMediator()->collisionWithBehavingAs( "behavior of spring stool" ) != nilPointer );
                        this->highJump = onASpringStool || ( character.isHeels() && character.getHighJumps() > 0 ) ;

                        if ( this->highJump ) {
                                if ( character.isHeels () )
                                        character.decrementBonusHighJumps () ;

                                SoundManager::getInstance().play( character.getOriginalKind(), "rebound" );
                        }

                        this->jumpPhase = 0 ;
                }

                // is it time to jump
                if ( speedTimer->getValue() > character.getSpeed() )
                {
                        activities::Jumping::getInstance().jump( *this, this->jumpPhase, this->highJump ? this->highJumpVector : this->jumpVector );

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
        if ( ! character.animationFinished () ) return ;

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
        if ( ! character.animationFinished () ) return ;

        // back to the original appearance of character
        character.metamorphInto( character.getOriginalKind(), "end teletransportation" );

        setCurrentActivity( activities::Activity::Waiting );
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
                                setCurrentActivity( activities::Activity::Vanishing );
                        } else
                                setCurrentActivity( activities::Activity::Waiting );

                        break;

                case activities::Activity::Vanishing:
                        if ( ! character.isActiveCharacter() ) {
                                std::cout << "inactive " << character.getUniqueName() << " is going to vanish, activate it" << std::endl ;
                                character.getMediator()->pickNextCharacter();
                        }

                        if ( character.isActiveCharacter() ) {
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

                        character.getMediator()->getRoom()->addFreeItem( donut );

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
                ItemPtr itemToTake ;

                // look for an item below the character
                if ( ! character.canAdvanceTo( 0, 0, -1 ) )
                {
                        int whereIsItemToPick = 0 ;

                        while ( mediator->isThereAnyCollision() )
                        {
                                ItemPtr belowItem = mediator->findCollisionPop( );

                                // pick a free pushable item less than or equal to 3/4 of the size of one tile
                                if ( belowItem != nilPointer && belowItem->getBehavior() != nilPointer
                                        && ( belowItem->getBehavior()->getNameOfBehavior() == "behavior of thing able to move by pushing" ||
                                                belowItem->getBehavior()->getNameOfBehavior() == "behavior of spring stool" )
                                        && belowItem->getUnsignedWidthX() <= ( mediator->getRoom()->getSizeOfOneTile() * 3 ) >> 2
                                        && belowItem->getUnsignedWidthY() <= ( mediator->getRoom()->getSizeOfOneTile() * 3 ) >> 2 )
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

                                character.placeItemInBag( itemToTake->getKind (), itemToTake->getBehavior()->getNameOfBehavior () );
                                GameManager::getInstance().setImageOfItemInBag (PicturePtr( new Picture( itemToTake->getRawImage() ) ));

                                itemToTake->getBehavior()->setCurrentActivity( activities::Activity::Vanishing );

                                setCurrentActivity ( // update activity
                                        ( getCurrentActivity() == activities::Activity::TakeAndJump )
                                                ? activities::Activity::Jumping
                                                : activities::Activity::Falling );

                                SoundManager::getInstance().play( character.getOriginalKind(), "take" );
                        }
                }
        }

        if ( getCurrentActivity() == activities::Activity::TakingItem || getCurrentActivity() == activities::Activity::TakeAndJump )
                // wait if can’t take
                setCurrentActivity( activities::Activity::Waiting );
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
                                                            character.getZ() - Room::LayerHeight,
                                                            "none" ) );
                        freeItem->setBehaviorOf( character.getBehaviorOfTakenItem() );

                        character.getMediator()->getRoom()->addFreeItem( freeItem );

                        GameManager::getInstance().emptyHandbag () ;
                        character.emptyBag ();

                        setCurrentActivity ( // update activity
                                ( getCurrentActivity() == activities::Activity::DropAndJump )
                                        ? activities::Activity::Jumping
                                        : activities::Activity::Waiting );

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
                setCurrentActivity( activities::Activity::Waiting );
}

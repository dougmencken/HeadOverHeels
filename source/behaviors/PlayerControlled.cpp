
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


PlayerControlled::PlayerControlled( const ItemPtr & item, const std::string & behavior )
        : Behavior( item, behavior )
        , jumpPhase( -1 )
        , highJump( false )
        , automoveStepsRemained( automatic_steps )
        , quickSteps( 0 )
        , speedTimer( new Timer () )
        , fallTimer( new Timer () )
        , glideTimer( new Timer () )
        , timerForBlinking( new Timer () )
        , donutFromHooterInRoom( false )
        , isLosingLife( false )
{

}

PlayerControlled::~PlayerControlled( )
{
        jumpVector.clear() ;
        highJumpVector.clear() ;
}

bool PlayerControlled::isInvulnerableToLethalItems () const
{
        return ( dynamic_cast< const ::AvatarItem & >( * getItem () ) ).hasShield ()
                        || GameManager::getInstance().isImmuneToCollisionsWithMortalItems () ;
}

void PlayerControlled::setCurrentActivity ( const Activity & newActivity )
{
        if ( this->affectedBy != nilPointer ) this->affectedBy = ItemPtr () ;

        if ( newActivity == activities::Activity::MetLethalItem && isInvulnerableToLethalItems () ) return ;

        this->activity = newActivity ;
}

void PlayerControlled::changeActivityDueTo ( const Activity & newActivity, const ItemPtr & dueTo )
{
        if ( newActivity == activities::Activity::MetLethalItem && isInvulnerableToLethalItems () ) return ;

        this->activity = newActivity ;
        this->affectedBy = dueTo ;
}

void PlayerControlled::wait( ::AvatarItem & character )
{
        character.wait ();

        if ( activities::Falling::getInstance().fall( this ) )
        {
                speedTimer->reset();
                activity = activities::Activity::Falling;

                if ( character.isHead ()
                                && character.getQuickSteps() > 0
                                        && this->quickSteps < 4 )
                {
                        // reset the quick steps counter
                        this->quickSteps = 0 ;
                }
        }
}

void PlayerControlled::move( ::AvatarItem & character )
{
        if ( character.isFrozen() ) return ;

        // apply the effect of quick steps bonus bunny
        double speed = character.getSpeed() / ( character.getQuickSteps() > 0 ? 2 : 1 );

        // is it time to move
        if ( speedTimer->getValue() > speed )
        {
                bool moved = activities::Moving::getInstance().move( this, &activity, true );

                // decrement the quick steps
                if ( character.getQuickSteps() > 0
                                && moved && activity != activities::Activity::Falling )
                {
                        this->quickSteps ++ ;

                        if ( this->quickSteps == 8 ) {
                                character.decrementBonusQuickSteps () ;
                                this->quickSteps = 4 ;
                        }
                }

                speedTimer->reset();

                character.animate();
        }
}

void PlayerControlled::automove( ::AvatarItem & character )
{
        // apply the effect of quick steps bonus bunny
        double speed = character.getSpeed() / ( character.getQuickSteps() > 0 ? 2 : 1 );

        // is it time to move
        if ( speedTimer->getValue() > speed )
        {
                // move it
                activities::Moving::getInstance().move( this, &activity, true );

                speedTimer->reset();

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

void PlayerControlled::displace( ::AvatarItem & character )
{
        if ( speedTimer->getValue() > character.getSpeed() )
        {
                activities::Displacing::getInstance().displace( this, &activity, true );
                // when the displacement couldn’t be performed due to a collision
                // then the activity is propagated to the collided items

                setCurrentActivity( activities::Activity::Waiting );

                speedTimer->reset();
        }
}

void PlayerControlled::handleMoveKeyWhenDragged ()
{
        const InputManager & input = InputManager::getInstance ();
        ::AvatarItem & avatar = dynamic_cast< ::AvatarItem & >( * getItem () );
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

void PlayerControlled::cancelDragging( ::AvatarItem & character )
{
        if ( ! character.isFrozen() )
        {
                if ( speedTimer->getValue() > character.getSpeed() )
                {
                        // move it
                        activities::Moving::getInstance().move( this, &activity, false );

                        speedTimer->reset();

                        character.animate ();
                }
        }
}

void PlayerControlled::fall( ::AvatarItem & character )
{
        // is it time to lower by one unit
        if ( fallTimer->getValue() > character.getWeight() )
        {
                if ( activities::Falling::getInstance().fall( this ) ) {
                        // as long as there's no collision below
                        if ( character.canAdvanceTo( 0, 0, -1 ) )
                        {       // show images of falling character
                                character.changeFrame( fallFrames[ character.getHeading () ] );
                        }
                }
                else if ( getCurrentActivity() != activities::Activity::MetLethalItem || isInvulnerableToLethalItems() )
                        setCurrentActivity( activities::Activity::Waiting );

                fallTimer->reset();
        }

        if ( getCurrentActivity() != activities::Activity::Falling )
                SoundManager::getInstance().stop( character.getOriginalKind(), "fall" );
}

void PlayerControlled::toJumpOrTeleport ()
{
        ::AvatarItem & character = dynamic_cast< ::AvatarItem & >( * getItem () );

        character.canAdvanceTo( 0, 0, -1 ); // is there a device underneath the character
        setCurrentActivity( character.getMediator()->collisionWithBehavingAs( "behavior of teletransport" ) != nilPointer
                                        ? activities::Activity::BeginTeletransportation
                                        : activities::Activity::Jumping );
}

void PlayerControlled::jump( ::AvatarItem & character )
{
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
                        activities::Jumping::getInstance().jump( this, &activity, this->jumpPhase, this->highJump ? this->highJumpVector : this->jumpVector );

                        // to the next phase of jump
                        ++ this->jumpPhase ;

                        if ( getCurrentActivity() == activities::Activity::Falling ) this->jumpPhase = -1 ; // end of jump

                        speedTimer->reset();

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

void PlayerControlled::glide( ::AvatarItem & character )
{
        if ( glideTimer->getValue() > character.getWeight() /* character.getSpeed() / 2.0 */ )
        {
                if ( ! activities::Falling::getInstance().fall( this ) &&
                        ( getCurrentActivity() != activities::Activity::MetLethalItem || isInvulnerableToLethalItems() ) )
                {
                        // not falling, back to waiting
                        setCurrentActivity( activities::Activity::Waiting );
                }

                glideTimer->reset();
        }

        if ( speedTimer->getValue() > character.getSpeed() * ( character.isHeadOverHeels() ? 2 : 1 ) )
        {
                Activity glideActivity = activities::Activity::Waiting ;

                switch ( Way( character.getHeading() ).getIntegerOfWay () )
                {
                        case Way::North:
                                glideActivity = activities::Activity::MovingNorth;
                                break;

                        case Way::South:
                                glideActivity = activities::Activity::MovingSouth;
                                break;

                        case Way::East:
                                glideActivity = activities::Activity::MovingEast;
                                break;

                        case Way::West:
                                glideActivity = activities::Activity::MovingWest;
                                break;

                        default:
                                ;
                }

                activities::Moving::getInstance().move( this, &glideActivity, false );

                // may turn while gliding so update the frame of falling
                character.changeFrame( fallFrames[ character.getHeading() ] );

                speedTimer->reset();
        }

        if ( getCurrentActivity() != activities::Activity::Gliding )
                SoundManager::getInstance().stop( character.getOriginalKind(), "fall" );
}

void PlayerControlled::enterTeletransport( ::AvatarItem & character )
{
	if ( getCurrentActivity() != activities::Activity::BeginTeletransportation ) return ;

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

void PlayerControlled::exitTeletransport( ::AvatarItem & character )
{
	if ( getCurrentActivity() != activities::Activity::EndTeletransportation ) return ;

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

void PlayerControlled::collideWithALethalItem( ::AvatarItem & character )
{
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

void PlayerControlled::useHooter( ::AvatarItem & character )
{
        if ( character.hasTool( "horn" ) && character.getDonuts() > 0 && ! this->donutFromHooterInRoom )
        {
                this->donutFromHooterInRoom = true ;

                const DescriptionOfItem * whatIsDonut = ItemDescriptions::descriptions().getDescriptionByKind( "bubbles" );
                if ( whatIsDonut != nilPointer )
                {
                        SoundManager::getInstance().stop( character.getOriginalKind(), "donut" );

                        // create item at the same position as the character
                        int z = character.getZ() + character.getHeight() - whatIsDonut->getHeight() ;
                        FreeItemPtr donut( new FreeItem (
                                whatIsDonut,
                                character.getX(), character.getY(),
                                z < 0 ? 0 : z,
                                character.getHeading () ) );

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
                                                bottomItem->getBehavior()->getNameOfBehavior() == "behavior of spring stool" )
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

                                takenItem->getBehavior()->setCurrentActivity( activities::Activity::Vanishing );
                                setCurrentActivity ( // update activity
                                        ( getCurrentActivity() == activities::Activity::TakeAndJump )
                                                ? activities::Activity::Jumping
                                                : activities::Activity::ItemTaken );

                                SoundManager::getInstance().play( character.getOriginalKind(), "take" );

                                std::cout << "took item \"" << takenItem->getUniqueName() << "\"" << std::endl ;
                        }
                }
        }

        if ( getCurrentActivity() != activities::Activity::ItemTaken && getCurrentActivity() != activities::Activity::Jumping )
                setCurrentActivity( activities::Activity::Waiting );
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

        if ( getCurrentActivity() != activities::Activity::Jumping )
                setCurrentActivity( activities::Activity::Waiting );
}

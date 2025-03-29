
#include "Volatile.hpp"

#include "AbstractItem.hpp"
#include "Room.hpp"
#include "Mediator.hpp"
#include "SoundManager.hpp"

#include <stack>

const float delayBeforeDisappearance = 0.04 ;
const float longDelayBeforeDisappearance = delayBeforeDisappearance * 20 ;
const float reactionTimeToHeadAppearance = 0.5 ;


namespace behaviors
{

Volatile::Volatile( DescribedItem & item, const std::string & behavior )
        : Behavior( item, behavior )
        , solid( false )
        , disappearanceTimer( new Timer() )
{
        disappearanceTimer->go() ;
}

bool Volatile::update ()
{
        DescribedItem & volatileItem = dynamic_cast< DescribedItem & >( getItem () );
        Mediator * mediator = volatileItem.getMediator() ;

        bool present = true ;

        switch ( getCurrentActivity () )
        {
                case activities::Activity::Waiting:
                case activities::Activity::WakeUp:
                        // for such activity it is always volatile
                        this->solid = false ;

                        // if it is volatile by contact and has an item above it
                        if ( ( getNameOfBehavior () == "vanishing on contact" ||
                                        getNameOfBehavior () == "vanishing when something is above" ||
                                                getNameOfBehavior () == "slowly vanishing when something is above" )
                                && ! volatileItem.canAdvanceTo( 0, 0, 1 ) )
                        {
                                bool gone = false ;

                                std::stack< DescribedItemPtr > itemsAbove ;

                                // look for every item above it
                                while ( mediator->isThereAnyCollision() )
                                {
                                        DescribedItemPtr atopItem = mediator->findCollisionPop( );

                                        // is it free item
                                        if ( atopItem != nilPointer &&
                                                ( atopItem->whichItemClass() == "free item" || atopItem->whichItemClass() == "avatar item" ) )
                                        {
                                                // look at whether an item above is volatile or bonus
                                                // because that item would disappear unless it is leaning on another one
                                                if ( atopItem->getBehavior() != nilPointer &&
                                                        atopItem->getBehavior()->getNameOfBehavior () != "vanishing when something is above" &&
                                                        atopItem->getBehavior()->getNameOfBehavior () != "slowly vanishing when something is above" &&
                                                        atopItem->getBehavior()->getNameOfBehavior () != "vanishing on contact" &&
                                                        atopItem->getBehavior()->getNameOfBehavior () != "behavior of bonus" )
                                                {
                                                        gone = true ;
                                                        itemsAbove.push( atopItem );
                                                }
                                        }
                                }

                                if ( gone )
                                {
                                        DescribedItemPtr atopItem = itemsAbove.top();
                                        itemsAbove.pop();
                                        atopItem->canAdvanceTo( 0, 0, -1 );

                                        // when an item that caused the disappearance is just above this volatile item, then it’s okay to vanish
                                        if ( mediator->howManyCollisions() > 1 ) {
                                                while ( mediator->isThereAnyCollision() )
                                                {
                                                        DescribedItemPtr belowItem = mediator->findCollisionPop( );

                                                        if ( belowItem != nilPointer ) {
                                                                // a volatile doesn’t vanish if it is leaning~
                                                                //     on an item without behavior, or
                                                                //     on a non-volatile item, or
                                                                //     on another item that is vanishing
                                                                if ( ( belowItem->getBehavior() == nilPointer )
                                                                        || ( belowItem->getBehavior() != nilPointer
                                                                                && belowItem->getBehavior()->getNameOfBehavior () != "vanishing when something is above"
                                                                                && belowItem->getBehavior()->getNameOfBehavior () != "vanishing on contact"
                                                                                && belowItem->getBehavior()->getNameOfBehavior () != "behavior of bonus" )
                                                                        || ( belowItem->getBehavior() != nilPointer
                                                                                && belowItem->getBehavior()->getCurrentActivity() == activities::Activity::Vanishing ) )
                                                                {
                                                                        gone = false ;
                                                                }
                                                        }
                                                }
                                        }
                                }

                                if ( gone ) {
                                        setCurrentActivity( activities::Activity::Vanishing );
                                        disappearanceTimer->go() ;
                                }
                        }
                        // if it's a puppy which disappears when Head or the composite character is in the room
                        else if ( getNameOfBehavior () == "vanishing as soon as Head appears" )
                        {
                                if ( mediator->findItemOfKind( "head" ) != nilPointer ||
                                        mediator->findItemOfKind( "headoverheels" ) != nilPointer )
                                {
                                        setCurrentActivity( activities::Activity::Vanishing );
                                        disappearanceTimer->go() ;
                                }
                        }
                        // is it volatile in time
                        else if ( getNameOfBehavior () == "vanishing after a while" )
                        {
                                volatileItem.animate() ;

                                if ( disappearanceTimer->getValue() > volatileItem.getDelayBetweenFrames() * volatileItem.howManyFramesInTheCurrentSequence() )
                                        present = false ;
                        }
                        break;

                case activities::Activity::Pushed :
                        if ( ! solid ) {
                                if ( getNameOfBehavior () == "vanishing on contact" )
                                        // pushing an item which is volatile on contact
                                        setCurrentActivity( activities::Activity::Vanishing );
                                else
                                if ( getNameOfBehavior () == "vanishing as soon as Head appears" ) {
                                        if ( mediator->findItemOfKind( "head" ) != nilPointer
                                                        || mediator->findItemOfKind( "headoverheels" ) != nilPointer )
                                                setCurrentActivity( activities::Activity::Vanishing );
                                } else
                                        setCurrentActivity( activities::Activity::Waiting );
                        } else
                                setCurrentActivity( activities::Activity::Freeze );

                        break ;

                case activities::Activity::Vanishing:
                        if ( ( getNameOfBehavior () != "vanishing when something is above" &&
                                        getNameOfBehavior () != "slowly vanishing when something is above" &&
                                        getNameOfBehavior () != "vanishing as soon as Head appears" ) ||
                                ( getNameOfBehavior () == "vanishing when something is above" && disappearanceTimer->getValue() > delayBeforeDisappearance ) ||
                                ( getNameOfBehavior () == "slowly vanishing when something is above" && disappearanceTimer->getValue() > longDelayBeforeDisappearance ) ||
                                ( getNameOfBehavior () == "vanishing as soon as Head appears" && disappearanceTimer->getValue() > reactionTimeToHeadAppearance ) )
                        {
                                SoundManager::getInstance().play( volatileItem.getKind (), "vanish" );

                                // morph into bubbles
                                volatileItem.setIgnoreCollisions( true );
                                volatileItem.changeHeightTo( 0 );
                                volatileItem.metamorphInto( "bubbles", "vanishing volatile item" );
                                volatileItem.setBehaviorOf( "vanishing after a while" );
                        }
                        break;

                case activities::Activity::Freeze:
                        this->solid = true ;
                        break;

                default:
                        ;
        }

        return present ;
}

}

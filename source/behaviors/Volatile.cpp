
#include "Volatile.hpp"

#include "Item.hpp"
#include "Room.hpp"
#include "Mediator.hpp"
#include "SoundManager.hpp"

#include <stack>

const float delayBeforeDisappearance = 0.04 ;
const float longDelayBeforeDisappearance = delayBeforeDisappearance * 20 ;


namespace behaviors
{

Volatile::Volatile( Item & item, const std::string & behavior )
        : Behavior( item, behavior )
        , solid( false )
        , disappearanceTimer( new Timer() )
{
        disappearanceTimer->go() ;
}

bool Volatile::update ()
{
        Item & volatileItem = getItem ();
        Mediator * mediator = volatileItem.getMediator() ;

        bool present = true ;

        switch ( getCurrentActivity () )
        {
                case activities::Activity::Waiting:
                case activities::Activity::WakeUp:
                        // for such activity it is always volatile
                        this->solid = false;

                        // if it is volatile by contact and has an item above it
                        if ( ( getNameOfBehavior () == "behavior of disappearance on touch" ||
                                        getNameOfBehavior () == "behavior of disappearance on jump into" ||
                                                getNameOfBehavior () == "behavior of slow disappearance on jump into" )
                                && ! volatileItem.canAdvanceTo( 0, 0, 1 ) )
                        {
                                bool gone = false ;

                                std::stack< ItemPtr > itemsAbove ;

                                // look for every item above it
                                while ( mediator->isThereAnyCollision() )
                                {
                                        ItemPtr atopItem = mediator->findCollisionPop( );

                                        // is it free item
                                        if ( atopItem != nilPointer &&
                                                ( atopItem->whichItemClass() == "free item" || atopItem->whichItemClass() == "avatar item" ) )
                                        {
                                                // look at whether an item above is volatile or bonus
                                                // because that item would disappear unless it is leaning on another one
                                                if ( atopItem->getBehavior() != nilPointer &&
                                                        atopItem->getBehavior()->getNameOfBehavior () != "behavior of disappearance on jump into" &&
                                                        atopItem->getBehavior()->getNameOfBehavior () != "behavior of slow disappearance on jump into" &&
                                                        atopItem->getBehavior()->getNameOfBehavior () != "behavior of disappearance on touch" &&
                                                        atopItem->getBehavior()->getNameOfBehavior () != "behavior of bonus" )
                                                {
                                                        gone = true ;
                                                        itemsAbove.push( atopItem );
                                                }
                                        }
                                }

                                if ( gone )
                                {
                                        ItemPtr atopItem = itemsAbove.top();
                                        itemsAbove.pop();
                                        atopItem->canAdvanceTo( 0, 0, -1 );

                                        // when an item that caused the disappearance is just above this volatile item, then it’s okay to vanish
                                        if ( mediator->howManyCollisions() > 1 ) {
                                                while ( mediator->isThereAnyCollision() )
                                                {
                                                        ItemPtr bottomItem = mediator->findCollisionPop( );

                                                        if ( bottomItem != nilPointer ) {
                                                                // a volatile doesn’t vanish if it is leaning~
                                                                //     on an item without behavior, or
                                                                //     on a non-volatile item, or
                                                                //     on another item that is vanishing
                                                                if ( ( bottomItem->getBehavior() == nilPointer )
                                                                        || ( bottomItem->getBehavior() != nilPointer
                                                                                && bottomItem->getBehavior()->getNameOfBehavior () != "behavior of disappearance on jump into"
                                                                                && bottomItem->getBehavior()->getNameOfBehavior () != "behavior of disappearance on touch"
                                                                                && bottomItem->getBehavior()->getNameOfBehavior () != "behavior of bonus" )
                                                                        || ( bottomItem->getBehavior() != nilPointer
                                                                                && bottomItem->getBehavior()->getCurrentActivity() == activities::Activity::Vanishing ) )
                                                                {
                                                                        gone = false ;
                                                                }
                                                        }
                                                }
                                        }
                                }

                                if ( gone ) {
                                        setCurrentActivity( activities::Activity::Vanishing );
                                        disappearanceTimer->reset();
                                }
                        }
                        // if it's a puppy which disappears when Head or the composite character is in the room
                        else if ( getNameOfBehavior () == "behavior of disappearance as soon as Head appears" )
                        {
                                if ( mediator->findItemOfKind( "head" ) != nilPointer ||
                                        mediator->findItemOfKind( "headoverheels" ) != nilPointer )
                                {
                                        setCurrentActivity( activities::Activity::Vanishing );
                                        disappearanceTimer->reset();
                                }
                        }
                        // is it volatile in time
                        else if ( getNameOfBehavior () == "behavior of disappearance in time" )
                        {
                                volatileItem.animate() ;

                                if ( disappearanceTimer->getValue() > volatileItem.getDelayBetweenFrames() * volatileItem.howManyFrames() )
                                        present = false ;
                        }
                        break;

                case activities::Activity::PushedNorth:
                case activities::Activity::PushedSouth:
                case activities::Activity::PushedEast:
                case activities::Activity::PushedWest:
                case activities::Activity::PushedNortheast:
                case activities::Activity::PushedSoutheast:
                case activities::Activity::PushedSouthwest:
                case activities::Activity::PushedNorthwest:
                        if ( ! solid ) {
                                if ( getNameOfBehavior () == "behavior of disappearance on touch" )
                                {
                                        // pushing item which is volatile on contact
                                        setCurrentActivity( activities::Activity::Vanishing );
                                }
                                else if ( getNameOfBehavior () == "behavior of disappearance as soon as Head appears" )
                                {
                                        if ( mediator->findItemOfKind( "head" ) != nilPointer
                                                        || mediator->findItemOfKind( "headoverheels" ) != nilPointer )
                                                setCurrentActivity( activities::Activity::Vanishing );
                                } else
                                        setCurrentActivity( activities::Activity::Waiting );
                        } else
                                setCurrentActivity( activities::Activity::Freeze );

                        break;

                case activities::Activity::Vanishing:
                        if ( ( getNameOfBehavior () != "behavior of disappearance on jump into" &&
                                        getNameOfBehavior () != "behavior of slow disappearance on jump into" &&
                                        getNameOfBehavior () != "behavior of disappearance as soon as Head appears" ) ||
                                ( getNameOfBehavior () == "behavior of disappearance on jump into" && disappearanceTimer->getValue() > delayBeforeDisappearance ) ||
                                ( getNameOfBehavior () == "behavior of slow disappearance on jump into" && disappearanceTimer->getValue() > longDelayBeforeDisappearance ) ||
                                ( getNameOfBehavior () == "behavior of disappearance as soon as Head appears" && disappearanceTimer->getValue() > 0.5 ) )
                        {
                                SoundManager::getInstance().play( volatileItem.getKind (), "vanish" );

                                // morph into bubbles

                                volatileItem.setIgnoreCollisions( true );

                                volatileItem.changeHeightTo( 0 );
                                volatileItem.metamorphInto( "bubbles", "vanishing volatile item" );
                                volatileItem.setBehaviorOf( "behavior of disappearance in time" );
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

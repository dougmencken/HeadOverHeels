
#include "Volatile.hpp"
#include "Item.hpp"
#include "ItemData.hpp"
#include "FreeItem.hpp"
#include "Room.hpp"
#include "Mediator.hpp"
#include "SoundManager.hpp"

#include <stack>

const float delayBeforeDisappearance = 0.04;
const float longDelayBeforeDisappearance = delayBeforeDisappearance * 20;


namespace iso
{

Volatile::Volatile( const ItemPtr & item, const std::string & behavior )
        : Behavior( item, behavior )
        , solid( false )
        , disappearanceTimer( new Timer() )
{
        disappearanceTimer->go();
}

Volatile::~Volatile()
{
}

bool Volatile::update ()
{
        bool vanish = false ;
        Mediator* mediator = item->getMediator();

        switch ( activity )
        {
                case Activity::Wait:
                case Activity::WakeUp:
                        // for such activity it is always volatile
                        this->solid = false;

                        // if it is volatile by contact and has an item above it
                        if ( ( getNameOfBehavior () == "behavior of disappearance on touch" ||
                                        getNameOfBehavior () == "behavior of disappearance on jump into" ||
                                                getNameOfBehavior () == "behavior of slow disappearance on jump into" )
                                && ! item->canAdvanceTo( 0, 0, 1 ) )
                        {
                                bool gone = false;

                                std::stack< ItemPtr > topItems;

                                // look for every item above it
                                while ( ! mediator->isStackOfCollisionsEmpty() )
                                {
                                        ItemPtr topItem = mediator->findCollisionPop( );

                                        // is it free item
                                        if ( topItem != nilPointer &&
                                                ( topItem->whichKindOfItem() == "free item" || topItem->whichKindOfItem() == "player item" ) )
                                        {
                                                // look at whether above item is volatile or special
                                                // because that item would disappear unless it is leaning on another one
                                                if ( topItem->getBehavior() != nilPointer &&
                                                        topItem->getBehavior()->getNameOfBehavior () != "behavior of disappearance on jump into" &&
                                                        topItem->getBehavior()->getNameOfBehavior () != "behavior of slow disappearance on jump into" &&
                                                        topItem->getBehavior()->getNameOfBehavior () != "behavior of disappearance on touch" &&
                                                        topItem->getBehavior()->getNameOfBehavior () != "behavior of something special" )
                                                {
                                                        gone = true;
                                                        topItems.push( topItem );
                                                }
                                        }
                                }

                                if ( gone )
                                {
                                        ItemPtr topItem = topItems.top();
                                        topItems.pop();
                                        topItem->canAdvanceTo( 0, 0, -1 );

                                        // when item that triggers disappearance is just on this volatile then it’s okay to vanish
                                        if ( mediator->depthOfStackOfCollisions() > 1 )
                                        {
                                                while ( ! mediator->isStackOfCollisionsEmpty() )
                                                {
                                                        ItemPtr bottomItem = mediator->findCollisionPop( );

                                                        if ( bottomItem != nilPointer )
                                                        {
                                                                // volatile doesn’t vanish if it is leaning~
                                                                //   on item without behavior, or
                                                                //   on item that is not volatile, or
                                                                //   on item that yet disappears
                                                                if ( ( bottomItem->getBehavior() == nilPointer ) ||
                                                                        ( bottomItem->getBehavior() != nilPointer
                                                                                && bottomItem->getBehavior()->getNameOfBehavior () != "behavior of disappearance on jump into"
                                                                                && bottomItem->getBehavior()->getNameOfBehavior () != "behavior of disappearance on touch"
                                                                                && bottomItem->getBehavior()->getNameOfBehavior () != "behavior of something special" ) ||
                                                                        ( bottomItem->getBehavior() != nilPointer
                                                                                && bottomItem->getBehavior()->getActivityOfItem() == Activity::Vanish ) )
                                                                {
                                                                        gone = false;
                                                                }
                                                        }
                                                }
                                        }
                                }

                                if ( gone )
                                {
                                        activity = Activity::Vanish;
                                        disappearanceTimer->reset();
                                }
                        }
                        // if it is puppy which disappears when Head or composite player is in room
                        else if ( getNameOfBehavior () == "behavior of disappearance as soon as Head appears" )
                        {
                                if ( mediator->findItemByLabel( "head" ) != nilPointer ||
                                        mediator->findItemByLabel( "headoverheels" ) != nilPointer )
                                {
                                        activity = Activity::Vanish;
                                        disappearanceTimer->reset();
                                }
                        }
                        // is it volatile in time
                        else if ( getNameOfBehavior () == "behavior of disappearance in time" )
                        {
                                item->animate();
                                vanish = ( disappearanceTimer->getValue() > item->getDelayBetweenFrames() * item->howManyMotions() );
                        }
                        break;

                case Activity::DisplaceNorth:
                case Activity::DisplaceSouth:
                case Activity::DisplaceEast:
                case Activity::DisplaceWest:
                case Activity::DisplaceNortheast:
                case Activity::DisplaceSoutheast:
                case Activity::DisplaceSouthwest:
                case Activity::DisplaceNorthwest:
                        // if displacing item which is volatile on contact then bin it
                        if ( ! solid )
                        {
                                if ( getNameOfBehavior () == "behavior of disappearance on touch" )
                                {
                                                activity = Activity::Vanish;
                                }
                                else if ( getNameOfBehavior () == "behavior of disappearance as soon as Head appears" )
                                {
                                        if ( mediator->findItemByLabel( "head" ) != nilPointer ||
                                                mediator->findItemByLabel( "headoverheels" ) != nilPointer )
                                        {
                                                activity = Activity::Vanish;
                                        }
                                }
                                else
                                {
                                        activity = Activity::Wait;
                                }
                        }
                        else
                        {
                                activity = Activity::Freeze;
                        }
                        break;

                case Activity::Vanish:
                        if ( ( getNameOfBehavior () != "behavior of disappearance on jump into" &&
                                        getNameOfBehavior () != "behavior of slow disappearance on jump into" &&
                                        getNameOfBehavior () != "behavior of disappearance as soon as Head appears" ) ||
                                ( getNameOfBehavior () == "behavior of disappearance on jump into" && disappearanceTimer->getValue() > delayBeforeDisappearance ) ||
                                ( getNameOfBehavior () == "behavior of slow disappearance on jump into" && disappearanceTimer->getValue() > longDelayBeforeDisappearance ) ||
                                ( getNameOfBehavior () == "behavior of disappearance as soon as Head appears" && disappearanceTimer->getValue() > 0.5 ) )
                        {
                                SoundManager::getInstance().play( item->getLabel(), activity );

                                // morph into bubbles

                                item->setCollisionDetector( false );

                                item->metamorphInto( "bubbles", "vanishing volatile item" );
                                item->setBehaviorOf( "behavior of disappearance in time" );
                                item->setHeight( 0 );
                        }
                        break;

                case Activity::Freeze:
                        this->solid = true;
                        break;

                default:
                        ;
        }

        return vanish ;
}

}

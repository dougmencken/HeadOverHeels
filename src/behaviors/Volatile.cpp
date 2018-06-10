
#include "Volatile.hpp"
#include "Item.hpp"
#include "ItemData.hpp"
#include "FreeItem.hpp"
#include "Room.hpp"
#include "Mediator.hpp"
#include "SoundManager.hpp"

const double delayBeforeDisappearance = 0.04;
const double longDelayBeforeDisappearance = delayBeforeDisappearance * 20;


namespace isomot
{

Volatile::Volatile( Item * item, const std::string & behavior )
        : Behavior( item, behavior )
        , solid( false )
{
        disappearanceTimer = new Timer();
        disappearanceTimer->go();
}

Volatile::~Volatile()
{
        delete disappearanceTimer;
}

bool Volatile::update ()
{
        bool vanish = false;
        Mediator* mediator = item->getMediator();

        switch ( activity )
        {
                case Wait:
                case WakeUp:
                        // for such activity it is always volatile
                        this->solid = false;

                        // if it is volatile by contact and has an item above it
                        if ( ( getNameOfBehavior () == "behavior of disappearance on touch" ||
                                        getNameOfBehavior () == "behavior of disappearance on jump into" ||
                                                getNameOfBehavior () == "behavior of slow disappearance on jump into" )
                                && ! item->canAdvanceTo( 0, 0, 1 ) )
                        {
                                bool isGone = false;

                                std::stack< Item * > topItems;

                                // look for every item above it
                                while ( ! mediator->isStackOfCollisionsEmpty() )
                                {
                                        Item* topItem = mediator->findCollisionPop( );

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
                                                        isGone = true;
                                                        topItems.push( topItem );
                                                }
                                        }
                                }

                                if ( isGone )
                                {
                                        Item* topItem = topItems.top();
                                        topItems.pop();
                                        topItem->canAdvanceTo( 0, 0, -1 );

                                        // when item that triggers disappearance is just on this volatile then it’s okay to vanish
                                        if ( mediator->depthOfStackOfCollisions() > 1 )
                                        {
                                                while ( ! mediator->isStackOfCollisionsEmpty() )
                                                {
                                                        Item* bottomItem = mediator->findCollisionPop( );

                                                        if ( bottomItem != nilPointer )
                                                        {
                                                                // volatile doesn’t vanish if it is leaning~
                                                                //   on item without behavior, or
                                                                //   on item that is not volatile, or
                                                                //   on item that disappears too
                                                                if ( ( bottomItem->getBehavior() == nilPointer ) ||
                                                                        ( bottomItem->getBehavior() != nilPointer
                                                                                && bottomItem->getBehavior()->getNameOfBehavior () != "behavior of disappearance on jump into"
                                                                                && bottomItem->getBehavior()->getNameOfBehavior () != "behavior of disappearance on touch"
                                                                                && bottomItem->getBehavior()->getNameOfBehavior () != "behavior of something special" ) ||
                                                                        ( bottomItem->getBehavior() != nilPointer
                                                                                && bottomItem->getBehavior()->getActivityOfItem() == Vanish ) )
                                                                {
                                                                        isGone = false;
                                                                }
                                                        }
                                                }
                                        }
                                }

                                if ( isGone )
                                {
                                        activity = Vanish;
                                        disappearanceTimer->reset();
                                }
                        }
                        // if it is puppy which disappears when Head or composite player is in room
                        else if ( getNameOfBehavior () == "behavior of disappearance as soon as Head appears" )
                        {
                                if ( mediator->findItemByLabel( "head" ) != nilPointer ||
                                        mediator->findItemByLabel( "headoverheels" ) != nilPointer )
                                {
                                        activity = Vanish;
                                        disappearanceTimer->reset();
                                }
                        }
                        // volatile in time item is created when some other volatile disappears
                        else if ( getNameOfBehavior () == "behavior of disappearance in time" )
                        {
                                vanish = ( disappearanceTimer->getValue() > item->getDelayBetweenFrames() * item->countFrames() );
                                item->animate();
                        }
                        break;

                case DisplaceNorth:
                case DisplaceSouth:
                case DisplaceEast:
                case DisplaceWest:
                case DisplaceNortheast:
                case DisplaceSoutheast:
                case DisplaceSouthwest:
                case DisplaceNorthwest:
                        // if displacing item which is volatile on contact then bin it
                        if ( ! solid )
                        {
                                if ( getNameOfBehavior () == "behavior of disappearance on touch" )
                                {
                                                activity = Vanish;
                                }
                                else if ( getNameOfBehavior () == "behavior of disappearance as soon as Head appears" )
                                {
                                        if ( mediator->findItemByLabel( "head" ) != nilPointer ||
                                                mediator->findItemByLabel( "headoverheels" ) != nilPointer )
                                        {
                                                activity = Vanish;
                                        }
                                }
                                else
                                {
                                        activity = Wait;
                                }
                        }
                        else
                        {
                                activity = Freeze;
                        }
                        break;

                case Vanish:
                        if ( ( getNameOfBehavior () != "behavior of disappearance on jump into" &&
                                        getNameOfBehavior () != "behavior of slow disappearance on jump into" &&
                                        getNameOfBehavior () != "behavior of disappearance as soon as Head appears" ) ||
                                ( getNameOfBehavior () == "behavior of disappearance on jump into" && disappearanceTimer->getValue() > delayBeforeDisappearance ) ||
                                ( getNameOfBehavior () == "behavior of slow disappearance on jump into" && disappearanceTimer->getValue() > longDelayBeforeDisappearance ) ||
                                ( getNameOfBehavior () == "behavior of disappearance as soon as Head appears" && disappearanceTimer->getValue() > 0.5 ) )
                        {
                                vanish = true;

                                SoundManager::getInstance()->play( item->getLabel(), activity );

                                // create bubbles at the same position as volatile
                                FreeItem* freeItem = new FreeItem(
                                        bubblesData,
                                        item->getX(), item->getY(), item->getZ(),
                                        Nowhere );

                                freeItem->assignBehavior( "behavior of disappearance in time", nilPointer );
                                freeItem->setCollisionDetector( false );

                                mediator->getRoom()->addFreeItem( freeItem );
                        }
                        break;

                case Freeze:
                        this->solid = true;
                        break;

                default:
                        ;
        }

        return vanish;
}

void Volatile::setMoreData( void* data )
{
        this->bubblesData = reinterpret_cast< ItemData * >( data );
}

}


#include "Volatile.hpp"
#include "Item.hpp"
#include "ItemData.hpp"
#include "FreeItem.hpp"
#include "Room.hpp"
#include "Mediator.hpp"
#include "SoundManager.hpp"


namespace isomot
{

Volatile::Volatile( Item * item, const std::string & behavior )
        : Behavior( item, behavior )
        , solid( false )
{
        disappearanceTimer = new HPC();
        disappearanceTimer->start();
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
                                && ! item->checkPosition( 0, 0, 1, Add ) )
                        {
                                bool isGone = false;

                                std::stack< Item * > topItems;

                                // look for every item above it
                                while ( ! mediator->isStackOfCollisionsEmpty() )
                                {
                                        int id = mediator->popCollision();

                                        // is it free item
                                        if ( id >= FirstFreeId && ( id & 1 ) )
                                        {
                                                Item* item = mediator->findItemById( id );

                                                // when item exists, look at whether it is volatile or special
                                                // because that item would disappear unless it is leaning on another one
                                                if ( item != 0 && item->getBehavior() != 0 &&
                                                        item->getBehavior()->getNameOfBehavior () != "behavior of disappearance on jump into" &&
                                                        item->getBehavior()->getNameOfBehavior () != "behavior of slow disappearance on jump into" &&
                                                        item->getBehavior()->getNameOfBehavior () != "behavior of disappearance on touch" &&
                                                        item->getBehavior()->getNameOfBehavior () != "behavior of something special" )
                                                {
                                                        isGone = true;
                                                        topItems.push( item );
                                                }
                                        }
                                }

                                if ( isGone )
                                {
                                        Item* topItem = topItems.top();
                                        topItems.pop();
                                        topItem->checkPosition( 0, 0, -1, Add );

                                        // when item that triggers disappearance is only on this volatile then it’s okay to vanish
                                        if ( mediator->depthOfStackOfCollisions() > 1 )
                                        {
                                                while ( ! mediator->isStackOfCollisionsEmpty() )
                                                {
                                                        Item* bottomItem = mediator->findCollisionPop( );

                                                        if ( bottomItem != 0 )
                                                        {
                                                                // volatile doesn’t vanish if it is leaning~
                                                                //   on item without behavior, or
                                                                //   on item that is not volatile, or
                                                                //   on item that disappears
                                                                if ( ( bottomItem->getBehavior() == 0 ) ||
                                                                        ( bottomItem->getBehavior() != 0
                                                                                && bottomItem->getBehavior()->getNameOfBehavior () != "behavior of disappearance on jump into"
                                                                                && bottomItem->getBehavior()->getNameOfBehavior () != "behavior of disappearance on touch"
                                                                                && bottomItem->getBehavior()->getNameOfBehavior () != "behavior of something special" ) ||
                                                                        ( bottomItem->getBehavior() != 0
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
                                if ( mediator->findItemByLabel( "head" ) != 0 || mediator->findItemByLabel( "headoverheels" ) != 0 )
                                {
                                        activity = Vanish;
                                        disappearanceTimer->reset();
                                }
                        }
                        // volatile in time item is created when some other volatile disappears
                        else if ( getNameOfBehavior () == "behavior of disappearance in time" )
                        {
                                vanish = ( disappearanceTimer->getValue() > item->getDelayBetweenFrames() * double( item->countFrames() ) );
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
                                        if ( mediator->findItemByLabel( "head" ) != 0 || mediator->findItemByLabel( "headoverheels" ) != 0 )
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
                                ( getNameOfBehavior () == "behavior of disappearance on jump into" && disappearanceTimer->getValue() > 0.030 ) ||
                                ( getNameOfBehavior () == "behavior of slow disappearance on jump into" && disappearanceTimer->getValue() > 0.600 ) ||
                                ( getNameOfBehavior () == "behavior of disappearance as soon as Head appears" && disappearanceTimer->getValue() > 0.500 ) )
                        {
                                vanish = true;

                                SoundManager::getInstance()->play( item->getLabel(), activity );

                                // create bubbles at the same position as volatile
                                FreeItem* freeItem = new FreeItem(
                                        bubblesData,
                                        item->getX(), item->getY(), item->getZ(),
                                        Nowhere );

                                freeItem->assignBehavior( "behavior of disappearance in time", 0 );
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

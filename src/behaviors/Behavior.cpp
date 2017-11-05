
#include "Behavior.hpp"
#include "Item.hpp"
#include "FreeItem.hpp"
#include "Mediator.hpp"
#include "CannonBall.hpp"
#include "ConveyorBelt.hpp"
#include "Detector.hpp"
#include "Drive.hpp"
#include "Elevator.hpp"
#include "Hunter.hpp"
#include "Impel.hpp"
#include "Mobile.hpp"
#include "OneWay.hpp"
#include "Patrol.hpp"
#include "RemoteControl.hpp"
#include "Sink.hpp"
#include "FireDoughnut.hpp"
#include "Special.hpp"
#include "Switch.hpp"
#include "Teleport.hpp"
#include "Trampoline.hpp"
#include "Turn.hpp"
#include "Volatile.hpp"
#include "PlayerHead.hpp"
#include "PlayerHeels.hpp"
#include "PlayerHeadAndHeels.hpp"
#include "KindOfActivity.hpp"
#include "MoveKindOfActivity.hpp"
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"


namespace isomot
{

Behavior::Behavior( Item * whichItem, const std::string & behavior ) :
          nameOfBehavior( behavior )
        , item( whichItem )
        , whatToDo( 0 )
        , activity( Wait )
        , sender( 0 )
{
        if ( behavior != "behavior of Head" && behavior != "behavior of Heels" && behavior != "behavior of Head over Heels" )
        {
                std::cout << "creation of behavior \"" << behavior << "\" for " << whichItem->whichKindOfItem() << " \"" << whichItem->getLabel()
                                << "\" at x=" << whichItem->getX() << " y=" << whichItem->getY() << " z=" << whichItem->getZ()
                                << " with orientation \"" << whichItem->getOrientation().toString() << "\""
                                << std::endl ;
        }
}

Behavior::~Behavior( )
{

}

Behavior* Behavior::createBehaviorByName( Item* item, const std::string& behavior, void* extraData )
{
        Behavior* behaviorToReturn = 0;

        if ( behavior == "behavior of conveyor" )
        {
                behaviorToReturn = new ConveyorBelt( item, behavior );
        }
        else if ( behavior == "behavior of detector" )
        {
                behaviorToReturn = new Detector( item, behavior );
        }
        else if ( behavior == "behavior of driver" )
        {
                behaviorToReturn = new Drive( item, behavior );
        }
        else if ( behavior == "behavior of elevator" )
        {
                behaviorToReturn = new Elevator( item, behavior );
                behaviorToReturn->setMoreData( extraData );
        }
        else if ( behavior == "behavior of hunter in four directions" ||
                        behavior == "behavior of waiting hunter in four directions" ||
                        behavior == "behavior of hunter in eight directions" ||
                        behavior == "behavior of waiting hunter in eight directions" )
        {
                behaviorToReturn = new Hunter( item, behavior );
                if ( "behavior of waiting hunter in four directions" )
                {
                        behaviorToReturn->setMoreData( extraData );
                }
        }
        else if ( behavior == "behavior of impel" )
        {
                behaviorToReturn = new Impel( item, behavior );
        }
        else if ( behavior == "behavior of move then turn left and move" ||
                        behavior == "behavior of move then turn right and move" )
        {
                behaviorToReturn = new Turn( item, behavior );
        }
        else if ( behavior == "behavior of thing able to move by pushing" )
        {
                behaviorToReturn = new Mobile( item, behavior );
        }
        else if ( behavior == "behavior of there and back" )
        {
                behaviorToReturn = new OneWay( item, behavior, false );
        }
        else if ( behavior == "behavior of flying there and back" )
        {
                behaviorToReturn = new OneWay( item, behavior, true );
        }
        else if ( behavior == "behavior of random patroling in four primary directions" ||
                        behavior == "behavior of random patroling in four secondary directions" ||
                        behavior == "behavior of random patroling in eight directions" )
        {
                behaviorToReturn = new Patrol( item, behavior );
        }
        else if ( behavior == "behavior of remote control" ||
                        behavior == "behavior of remotely controlled one" )
        {
                behaviorToReturn = new RemoteControl( item, behavior );
        }
        else if ( behavior == "behavior of sinking downward" )
        {
                behaviorToReturn = new Sink( item, behavior );
        }
        else if ( behavior == "behavior of freezing doughnut" )
        {
                behaviorToReturn = new FireDoughnut( item, behavior );
        }
        else if ( behavior == "behavior of something special" )
        {
                behaviorToReturn = new Special( item, behavior );
                behaviorToReturn->setMoreData( extraData );
        }
        else if ( behavior == "behavior of switch" )
        {
                behaviorToReturn = new Switch( item, behavior );
        }
        else if ( behavior == "behavior of teletransport" )
        {
                behaviorToReturn = new Teleport( item, behavior );
        }
        else if ( behavior == "behavior of big leap for player" )
        {
                behaviorToReturn = new Trampoline( item, behavior );
        }
        else if ( behavior == "behavior of disappearance in time" ||
                        behavior == "behavior of disappearance on touch" ||
                        behavior == "behavior of disappearance on jump into" ||
                        behavior == "behavior of disappearance as soon as Head appears" ||
                        behavior == "behavior of slow disappearance on jump into" )
        {
                behaviorToReturn = new Volatile( item, behavior );
                behaviorToReturn->setMoreData( extraData );
        }
        else if ( behavior == "behaivor of final ball" )
        {
                behaviorToReturn = new CannonBall( item, behavior );
                behaviorToReturn->setMoreData( extraData );
        }
        else if ( behavior == "behavior of Head" )
        {
                behaviorToReturn = new PlayerHead( item, behavior );
                behaviorToReturn->setMoreData( extraData );
        }
        else if ( behavior == "behavior of Heels" )
        {
                behaviorToReturn = new PlayerHeels( item, behavior );
                behaviorToReturn->setMoreData( extraData );
        }
        else if ( behavior == "behavior of Head over Heels" )
        {
                behaviorToReturn = new PlayerHeadAndHeels( item, behavior );
                behaviorToReturn->setMoreData( extraData );
        }
        else // if ( behavior == "still" || behavior == "behavior of bubbles" )
        {
                // yeah, do nothing
                /* std::cout << "nil behavior \"" << behavior << "\" for item \"" << item->getLabel()
                                << "\" at x=" << item->getX() << " y=" << item->getY() << " z=" << item->getZ() << std::endl ; */
        }

        return behaviorToReturn;
}

void Behavior::changeActivityOfItem( const ActivityOfItem& activity, Item* sender )
{
        this->activity = activity;
        this->sender = sender;

        switch ( activity )
        {
                case MoveNorth:
                case MoveSouth:
                case MoveEast:
                case MoveWest:
                case MoveNortheast:
                case MoveNorthwest:
                case MoveSoutheast:
                case MoveSouthwest:
                case MoveUp:
                case MoveDown:
                        whatToDo = MoveKindOfActivity::getInstance();
                        break;

                case DisplaceNorth:
                case DisplaceSouth:
                case DisplaceEast:
                case DisplaceWest:
                case DisplaceNortheast:
                case DisplaceSoutheast:
                case DisplaceSouthwest:
                case DisplaceNorthwest:
                case ForceDisplaceNorth:
                case ForceDisplaceSouth:
                case ForceDisplaceEast:
                case ForceDisplaceWest:
                        whatToDo = DisplaceKindOfActivity::getInstance();
                        break;

                case Fall:
                        whatToDo = FallKindOfActivity::getInstance();
                        break;

                default:
                        ;
        }
}

void Behavior::propagateActivity( Item* sender, const ActivityOfItem& activity )
{
        Mediator* mediator = sender->getMediator();

        // as long as there are elements collided with issuer
        while ( ! mediator->isStackOfCollisionsEmpty () )
        {
                int id = mediator->popCollision();

                // item has to be free item or grid item
                if ( ( id >= FirstFreeId && ( id & 1 )) || ( id >= FirstGridId && !( id & 1 ) ) )
                {
                        Item* item = mediator->findItemById( id );

                        // change activity for existing item with non-null behavior
                        if ( item != 0 && item->getBehavior() != 0 )
                        {
                                item->getBehavior()->changeActivityOfItem( activity );
                        }
                }
        }
}

}

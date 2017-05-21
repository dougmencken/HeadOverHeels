
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

Behavior::Behavior( Item * whichItem, const BehaviorOfItem & behavior ) :
          theBehavior( behavior )
        , item( whichItem )
        , whatToDo( 0 )
        , activity( Wait )
        , sender( 0 )
{

}

Behavior::~Behavior( )
{

}

Behavior* Behavior::createBehavior( Item * item, const BehaviorOfItem& id, void * extraData )
{
        Behavior* behavior = 0;

        switch ( id )
        {
                case NoBehavior:
                        break;

                case BubblesBehavior:
                        break;

                case ConveyorBeltNortheast:
                case ConveyorBeltSouthwest:
                        behavior = new ConveyorBelt( item, id );
                        break;

                case DetectorBehavior:
                        behavior = new Detector( item, id );
                        break;

                case DriveBehavior:
                        behavior = new Drive( item, id );
                        break;

                case ElevatorBehavior:
                        behavior = new Elevator( item, id );
                        behavior->setMoreData( extraData );
                        break;

                case Hunter4Behavior:
                case HunterWaiting4Behavior:
                case Hunter8Behavior:
                case HunterWaiting8Behavior:
                        behavior = new Hunter( item, id );
                        if ( HunterWaiting4Behavior )
                        {
                                behavior->setMoreData( extraData );
                        }
                        break;

                case ImpelBehavior:
                        behavior = new Impel( item, id );
                        break;

                case TurnLeftBehavior:
                case TurnRightBehavior:
                        behavior = new Turn( item, id );
                        break;

                case MobileBehavior:
                        behavior = new Mobile( item, id );
                        break;

                case OneWayBehavior:
                case FlyingOneWayBehavior:
                        behavior = new OneWay( item, id, id == FlyingOneWayBehavior );
                        break;

                case Patrol4cBehavior:
                case Patrol4dBehavior:
                case Patrol8Behavior:
                        behavior = new Patrol( item, id );
                        break;

                case RemoteControlBehavior:
                case SteerBehavior:
                        behavior = new RemoteControl( item, id );
                        break;

                case SinkBehavior:
                        behavior = new Sink( item, id );
                        break;

                case FireDoughnutBehavior:
                        behavior = new FireDoughnut( item, id );
                        break;

                case SpecialBehavior:
                        behavior = new Special( item, id );
                        behavior->setMoreData( extraData );
                        break;

                case SwitchBehavior:
                        behavior = new Switch( item, id );
                        break;

                case TeleportBehavior:
                        behavior = new Teleport( item, id );
                        break;

                case TrampolineBehavior:
                        behavior = new Trampoline( item, id );
                        break;

                case VolatileTimeBehavior:
                case VolatileTouchBehavior:
                case VolatileWeightBehavior:
                case VolatilePuppyBehavior:
                case VolatileHeavyBehavior:
                        behavior = new Volatile( item, id );
                        behavior->setMoreData( extraData );
                        break;

                case CannonBallBehavior:
                        behavior = new CannonBall( item, id );
                        behavior->setMoreData( extraData );
                        break;

                case HeadBehavior:
                        behavior = new PlayerHead( item, id );
                        behavior->setMoreData( extraData );
                        break;

                case HeelsBehavior:
                        behavior = new PlayerHeels( item, id );
                        behavior->setMoreData( extraData );
                        break;

                case HeadAndHeelsBehavior:
                        behavior = new PlayerHeadAndHeels( item, id );
                        behavior->setMoreData( extraData );
                        break;
        }

        return behavior;
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

        // Mientras haya elementos que hayan chocado con el emisor
        while ( ! mediator->isStackOfCollisionsEmpty () )
        {
                // Identificador del primer elemento de la pila de colisiones
                int id = mediator->popCollision();

                // El elemento tiene que se un elemento libre o uno rejilla
                if ( ( id >= FirstFreeId && ( id & 1 )) || ( id >= FirstGridId && !( id & 1 ) ) )
                {
                        Item* item = mediator->findItemById( id );

                        // Si el elemento se ha encontrado y tiene comportamiento se cambia su estado
                        if ( item != 0 && item->getBehavior() != 0 )
                        {
                                item->getBehavior()->changeActivityOfItem( activity );
                        }
                }
        }
}

}

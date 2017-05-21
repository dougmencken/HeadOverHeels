
#include "Behavior.hpp"
#include "Item.hpp"
#include "FreeItem.hpp"
#include "Mediator.hpp"
#include "BehaviorState.hpp"
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
#include "Shot.hpp"
#include "Special.hpp"
#include "Switch.hpp"
#include "Teleport.hpp"
#include "Trampoline.hpp"
#include "Turn.hpp"
#include "Volatile.hpp"
#include "PlayerHead.hpp"
#include "PlayerHeels.hpp"
#include "PlayerHeadAndHeels.hpp"
#include "MoveState.hpp"
#include "DisplaceState.hpp"
#include "FallState.hpp"


namespace isomot
{

Behavior::Behavior( Item* item, const BehaviorId& id )
:       id( id ),
        item( item ),
        state( 0 ),
        sender( 0 ),
        stateId( StateWait )
{

}

Behavior::~Behavior( )
{

}

Behavior* Behavior::createBehavior( Item* item, const BehaviorId& id, void* extraData )
{
        Behavior* behavior = 0;

        // Construye el objeto adecuado en función del identificador
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

                case ShotBehavior:
                        behavior = new Shot( item, id );
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

void Behavior::changeStateId( const StateId& stateId, Item* sender )
{
        this->stateId = stateId;
        this->sender = sender;

        // Asigna el estado en función del identificador
        switch( stateId )
        {
                case StateMoveNorth:
                case StateMoveSouth:
                case StateMoveEast:
                case StateMoveWest:
                case StateMoveNortheast:
                case StateMoveNorthwest:
                case StateMoveSoutheast:
                case StateMoveSouthwest:
                case StateMoveUp:
                case StateMoveDown:
                        state = MoveState::getInstance();
                        break;

                case StateDisplaceNorth:
                case StateDisplaceSouth:
                case StateDisplaceEast:
                case StateDisplaceWest:
                case StateDisplaceNortheast:
                case StateDisplaceSoutheast:
                case StateDisplaceSouthwest:
                case StateDisplaceNorthwest:
                case StateForceDisplaceNorth:
                case StateForceDisplaceSouth:
                case StateForceDisplaceEast:
                case StateForceDisplaceWest:
                        state = DisplaceState::getInstance();
                        break;

                case StateFall:
                        state = FallState::getInstance();
                        break;

                default:
                        ;
        }
}

void Behavior::propagateState( Item* sender, const StateId& stateId )
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
                                item->getBehavior()->changeStateId( stateId );
                        }
                }
        }
}

}

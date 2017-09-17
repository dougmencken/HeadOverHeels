
#include "FireDoughnut.hpp"
#include "Mediator.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
#include "UserControlled.hpp"
#include "MoveKindOfActivity.hpp"


namespace isomot
{

FireDoughnut::FireDoughnut( Item * item, const std::string & behavior )
        : Behavior( item, behavior )
        , playerItem( 0 )
{
        speedTimer = new HPC();
        speedTimer->start();
}

FireDoughnut::~FireDoughnut( )
{
        delete speedTimer;
}

bool FireDoughnut::update ()
{
        FreeItem* freeItem = dynamic_cast< FreeItem * >( this->item );
        bool destroy = false;

        switch ( activity )
        {
                // El estado inicial establece la dirección
                case Wait:
                        switch ( freeItem->getDirection() )
                        {
                                case North:
                                        activity = MoveNorth;
                                        break;

                                case South:
                                        activity = MoveSouth;
                                        break;

                                case East:
                                        activity = MoveEast;
                                        break;

                                case West:
                                        activity = MoveWest;
                                        break;

                                default:
                                        ;
                        }

                        whatToDo = MoveKindOfActivity::getInstance();
                        break;

                case MoveNorth:
                        if ( speedTimer->getValue() > freeItem->getSpeed() )
                        {
                                // Almacena en la pila de colisiones los elementos que hay al norte
                                freeItem->setCollisionDetector( true );
                                freeItem->checkPosition( -1, 0, 0, Add );
                                // Si no hay colisión o ha chocado con el jugador, el disparo se mueve
                                if ( freeItem->getMediator()->isStackOfCollisionsEmpty() || freeItem->getMediator()->collisionWithByLabel( playerItem->getLabel() ) )
                                {
                                        freeItem->setCollisionDetector( false );
                                        whatToDo->move( this, &activity, false );
                                }
                                else
                                {
                                        // Si ha chocado con un elemento enemigo entonces lo paraliza
                                        if ( freeItem->getMediator()->collisionWithBadBoy() )
                                        {
                                                propagateActivity( this->item, Freeze );
                                        }

                                        // En caso de colisión con cualquier elemento (excepto el jugador), el disparo desaparece
                                        dynamic_cast< UserControlled * >( playerItem->getBehavior() )->setFireFromHooter( false );
                                        destroy = true;
                                }

                                // Se pone a cero el cronómetro para el siguiente ciclo
                                speedTimer->reset();

                                // Anima el elemento
                                freeItem->animate();
                        }
                        break;

                case MoveSouth:
                        if ( speedTimer->getValue() > freeItem->getSpeed() )
                        {
                                // Almacena en la pila de colisiones los elementos que hay al sur
                                freeItem->setCollisionDetector( true );
                                freeItem->checkPosition( 1, 0, 0, Add );
                                // Si no hay colisión o ha chocado con el jugador, el disparo se mueve
                                if ( freeItem->getMediator()->isStackOfCollisionsEmpty() || freeItem->getMediator()->collisionWithByLabel( playerItem->getLabel() ) )
                                {
                                        freeItem->setCollisionDetector( false );
                                        whatToDo->move( this, &activity, false );
                                }
                                else
                                {
                                        // Si ha chocado con un elemento enemigo entonces lo paraliza
                                        if ( freeItem->getMediator()->collisionWithBadBoy() )
                                        {
                                                propagateActivity( this->item, Freeze );
                                        }

                                        // En caso de colisión con cualquier elemento (excepto el jugador), el disparo desaparece
                                        dynamic_cast< UserControlled * >( playerItem->getBehavior() )->setFireFromHooter( false );
                                        destroy = true;
                                }

                                // Se pone a cero el cronómetro para el siguiente ciclo
                                speedTimer->reset();

                                // Anima el elemento
                                freeItem->animate();
                        }
                        break;

                case MoveEast:
                        if ( speedTimer->getValue() > freeItem->getSpeed() )
                        {
                                // Almacena en la pila de colisiones los elementos que hay al este
                                freeItem->setCollisionDetector( true );
                                freeItem->checkPosition( 0, -1, 0, Add );
                                // Si no hay colisión o ha chocado con el jugador, el disparo se mueve
                                if ( freeItem->getMediator()->isStackOfCollisionsEmpty() || freeItem->getMediator()->collisionWithByLabel( playerItem->getLabel() ) )
                                {
                                        freeItem->setCollisionDetector( false );
                                        whatToDo->move( this, &activity, false );
                                }
                                else
                                {
                                        // Si ha chocado con un elemento enemigo entonces lo paraliza
                                        if ( freeItem->getMediator()->collisionWithBadBoy() )
                                        {
                                                propagateActivity( this->item, Freeze );
                                        }

                                        // En caso de colisión con cualquier elemento (excepto el jugador), el disparo desaparece
                                        dynamic_cast< UserControlled * >( playerItem->getBehavior() )->setFireFromHooter( false );
                                        destroy = true;
                                }

                                // Se pone a cero el cronómetro para el siguiente ciclo
                                speedTimer->reset();

                                // Anima el elemento
                                freeItem->animate();
                        }
                        break;

                case MoveWest:
                        if ( speedTimer->getValue() > freeItem->getSpeed() )
                        {
                                // Almacena en la pila de colisiones los elementos que hay al oeste
                                freeItem->setCollisionDetector( true );
                                freeItem->checkPosition( 0, 1, 0, Add );
                                // Si no hay colisión o ha chocado con el jugador, el disparo se mueve
                                if ( freeItem->getMediator()->isStackOfCollisionsEmpty() || freeItem->getMediator()->collisionWithByLabel( playerItem->getLabel() ) )
                                {
                                        freeItem->setCollisionDetector( false );
                                        whatToDo->move( this, &activity, false );
                                }
                                else
                                {
                                        // Si ha chocado con un elemento enemigo entonces lo paraliza
                                        if ( freeItem->getMediator()->collisionWithBadBoy() )
                                        {
                                                propagateActivity( this->item, Freeze );
                                        }

                                        // En caso de colisión con cualquier elemento (excepto el jugador), el disparo desaparece
                                        dynamic_cast< UserControlled * >( playerItem->getBehavior() )->setFireFromHooter( false );
                                        destroy = true;
                                }

                                // Se pone a cero el cronómetro para el siguiente ciclo
                                speedTimer->reset();

                                // Anima el elemento
                                freeItem->animate();
                        }
                        break;

                default:
                        ;
        }

        return destroy;
}

void FireDoughnut::setPlayerItem( PlayerItem * player )
{
        this->playerItem = player;
}

}

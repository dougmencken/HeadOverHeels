
#include "CannonBall.hpp"
#include "FreeItem.hpp"
#include "Mediator.hpp"
#include "MoveKindOfActivity.hpp"
#include "Mediator.hpp"
#include "Room.hpp"


namespace isomot
{

CannonBall::CannonBall( Item * item, const std::string & behavior ) :
        Behavior( item, behavior )
{
        speedTimer = new HPC();
        speedTimer->start();
}

CannonBall::~CannonBall( )
{
        delete speedTimer;
}

bool CannonBall::update ()
{
        FreeItem* freeItem = dynamic_cast< FreeItem* >( this->item );
        bool destroy = false;

        switch ( activity )
        {
                case Wait:
                        this->changeActivityOfItem( MoveNorth );
                        break;

                case MoveNorth:
                        if ( speedTimer->getValue() > freeItem->getSpeed() )
                        {
                                // Almacena en la pila de colisiones los elementos que hay al norte
                                freeItem->checkPosition( -1, 0, 0, Add );

                                // Si no hay colisión, la bola se mueve
                                if ( freeItem->getMediator()->isStackOfCollisionsEmpty() )
                                {
                                        whatToDo->move( this, &activity, false );
                                }
                                else
                                {
                                        // En caso de colisión con cualquier elemento (excepto el jugador), el disparo desaparece
                                        destroy = true;

                                        // Crea el elemento en la misma posición que el volátil y a su misma altura
                                        FreeItem * freeItem = new FreeItem (
                                                bubblesData,
                                                item->getX(), item->getY(), item->getZ(),
                                                NoDirection
                                        );

                                        freeItem->assignBehavior( "behavior of disappearance in time", 0 );
                                        freeItem->setCollisionDetector( false );

                                        // Se añade a la sala actual
                                        item->getMediator()->getRoom()->addItem( freeItem );
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

void CannonBall::setMoreData( void * data )
{
        this->bubblesData = reinterpret_cast< ItemData * >( data );
}

}

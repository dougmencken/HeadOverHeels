
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
        bool isGone = false;

        switch ( activity )
        {
                case Wait:
                        this->changeActivityOfItem( MoveNorth );
                        break;

                case MoveNorth:
                        if ( speedTimer->getValue() > freeItem->getSpeed() )
                        {
                                // look for collisions with items that are to the north
                                freeItem->checkPosition( -1, 0, 0, Add );

                                // move ball when there’s no collision
                                if ( freeItem->getMediator()->isStackOfCollisionsEmpty() )
                                {
                                        whatToDo->move( this, &activity, false );
                                }
                                else
                                {
                                        // disappear in case of collision with any item but player
                                        isGone = true;

                                        // create “bubbles” item at the same position
                                        FreeItem * freeItem = new FreeItem (
                                                bubblesData,
                                                item->getX(), item->getY(), item->getZ(),
                                                NoDirection
                                        );

                                        freeItem->assignBehavior( "behavior of disappearance in time", 0 );
                                        freeItem->setCollisionDetector( false );

                                        item->getMediator()->getRoom()->addFreeItem( freeItem );
                                }

                                speedTimer->reset();

                                freeItem->animate();
                        }
                        break;

                default:
                        ;
        }

        return isGone;
}

void CannonBall::setMoreData( void * data )
{
        this->bubblesData = reinterpret_cast< ItemData * >( data );
}

}

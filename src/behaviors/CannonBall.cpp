
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
        speedTimer = new Timer();
        speedTimer->go();
}

CannonBall::~CannonBall( )
{
        delete speedTimer;
}

bool CannonBall::update ()
{
        bool isGone = false;

        switch ( activity )
        {
                case Wait:
                        this->changeActivityOfItem( MoveNorth );
                        break;

                case MoveNorth:
                        if ( speedTimer->getValue() > this->item->getSpeed() )
                        {
                                // look for collisions with items that are to the north
                                this->item->checkPosition( -1, 0, 0, Add );

                                // move ball when there’s no collision
                                if ( this->item->getMediator()->isStackOfCollisionsEmpty() )
                                {
                                        MoveKindOfActivity::getInstance()->move( this, &activity, false );
                                }
                                else
                                {
                                        // disappear in case of collision
                                        isGone = true;

                                        // create bubbles at the same position
                                        FreeItem * bubbles = new FreeItem (
                                                bubblesData,
                                                item->getX(), item->getY(), item->getZ(),
                                                Nowhere
                                        );

                                        bubbles->assignBehavior( "behavior of disappearance in time", nilPointer );
                                        bubbles->setCollisionDetector( false );

                                        item->getMediator()->getRoom()->addFreeItem( bubbles );
                                }

                                speedTimer->reset();

                                this->item->animate();
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

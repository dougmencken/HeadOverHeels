
#include "FinalBall.hpp"
#include "FreeItem.hpp"
#include "Mediator.hpp"
#include "MoveKindOfActivity.hpp"
#include "Mediator.hpp"
#include "Room.hpp"


namespace iso
{

FinalBall::FinalBall( const ItemPtr & item, const std::string & behavior )
        : Behavior( item, behavior )
        , speedTimer( new Timer() )
{
        speedTimer->go();
}

FinalBall::~FinalBall( )
{
}

bool FinalBall::update ()
{
        bool isGone = false;

        switch ( activity )
        {
                case Activity::Wait:
                        this->changeActivityOfItem( Activity::MoveNorth );
                        break;

                case Activity::MoveNorth:
                        if ( speedTimer->getValue() > this->item->getSpeed() )
                        {
                                // look for collisions with items that are to the north
                                this->item->canAdvanceTo( -1, 0, 0 );

                                // move ball when there’s no collision
                                if ( this->item->getMediator()->isStackOfCollisionsEmpty() )
                                {
                                        MoveKindOfActivity::getInstance().move( this, &activity, false );
                                }
                                else
                                {
                                        // disappear in case of collision
                                        isGone = true;

                                        // create bubbles at the same position
                                        FreeItemPtr bubbles( new FreeItem (
                                                item->getDataOfItem()->getItemDataManager()->findDataByLabel( "bubbles" ),
                                                item->getX(), item->getY(), item->getZ(),
                                                Way::Nowhere ) );

                                        bubbles->setBehaviorOf( "behavior of disappearance in time" );
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

}


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

bool FinalBall::update ()
{
        switch ( activity )
        {
                case Activity::Wait:
                        this->changeActivityOfItem( Activity::MoveNorth );
                        break;

                case Activity::MoveNorth:
                        if ( speedTimer->getValue() > this->item->getSpeed() )
                        {
                                speedTimer->reset();

                                item->animate();

                                // look for collisions with items that are to the north
                                this->item->canAdvanceTo( -1, 0, 0 );

                                // move ball when there’s no collision
                                if ( this->item->getMediator()->isStackOfCollisionsEmpty() )
                                {
                                        MoveKindOfActivity::getInstance().move( this, &activity, false );
                                }
                                else
                                {
                                        item->setIgnoreCollisions( true );

                                        item->metamorphInto( "bubbles", "vanishing final ball" );
                                        item->setBehaviorOf( "behavior of disappearance in time" );
                                }
                        }
                        break;

                default:
                        ;
        }

        return false ;
}

}

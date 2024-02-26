
#include "Conveyor.hpp"

#include "Item.hpp"
#include "FreeItem.hpp"
#include "AvatarItem.hpp"
#include "Displacing.hpp"
#include "Falling.hpp"
#include "Mediator.hpp"
#include "SoundManager.hpp"

#include <stack>


namespace behaviors
{

Conveyor::Conveyor( const ItemPtr & item, const std::string & behavior )
        : Behavior( item, behavior )
        , speedTimer( new Timer() )
        , animationTimer( new Timer() )
{
        speedTimer->go();
        animationTimer->go();
}

Conveyor::~Conveyor( )
{
}

bool Conveyor::update ()
{
        Mediator* mediator = item->getMediator();

        switch ( activity )
        {
                case activities::Activity::Waiting:
                        if ( speedTimer->getValue() > item->getSpeed() )
                        {
                                if ( ! item->canAdvanceTo( 0, 0, 1 ) )
                                {
                                        // copy the stack of collisions
                                        std::stack< std::string > itemsAbove ;
                                        while ( ! mediator->isStackOfCollisionsEmpty() )
                                                itemsAbove.push( mediator->popCollision() );

                                        while ( ! itemsAbove.empty () ) // for each such item
                                        {
                                                ItemPtr collision = mediator->findItemByUniqueName( itemsAbove.top() );
                                                itemsAbove.pop() ;

                                                // is it free item
                                                if ( collision != nilPointer &&
                                                        ( collision->whichItemClass() == "free item" || collision->whichItemClass() == "avatar item" ) )
                                                {
                                                        FreeItem & itemAbove = dynamic_cast< FreeItem & >( *collision );

                                                        if ( /* the carrier is this conveyor */ this->item->getUniqueName() == itemAbove.getCarrier()
                                                                        || /* or any other carrying item */ ! itemAbove.getCarrier().empty() )
                                                        {
                                                                if ( itemAbove.getBehavior() != nilPointer )
                                                                {
                                                                        Activity activityOfItemAbove = itemAbove.getBehavior()->getCurrentActivity() ;
                                                                        bool outOfGravity = ( activityOfItemAbove == activities::Activity::Jumping
                                                                                                  || activityOfItemAbove == activities::Activity::Vanishing ) ;

                                                                        if ( ! outOfGravity ) {
                                                                                if ( item->getOrientation() == "south" )
                                                                                        itemAbove.getBehavior()->setCurrentActivity( activities::Activity::DraggedSouth );
                                                                                else if ( item->getOrientation() == "west" )
                                                                                        itemAbove.getBehavior()->setCurrentActivity( activities::Activity::DraggedWest );
                                                                                else if ( item->getOrientation() == "north" )
                                                                                        itemAbove.getBehavior()->setCurrentActivity( activities::Activity::DraggedNorth );
                                                                                else if ( item->getOrientation() == "east" )
                                                                                        itemAbove.getBehavior()->setCurrentActivity( activities::Activity::DraggedEast );
                                                                        }

                                                                        // play the sound of conveyor
                                                                        SoundManager::getInstance().play( item->getKind (), "function" );
                                                                }
                                                        }
                                                }
                                        }
                                }

                                speedTimer->reset();
                        }

                        if ( animationTimer->getValue() > 2 * item->getSpeed() )
                        {
                                item->animate ();
                                animationTimer->reset();
                        }

                        break;

                default:
                        activity = activities::Activity::Waiting;
        }

        return false;
}

}

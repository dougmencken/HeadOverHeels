
#include "Behavior.hpp"

#include "Item.hpp"
#include "FreeItem.hpp"
#include "Mediator.hpp"

#include "Moving.hpp"
#include "Displacing.hpp"
#include "Falling.hpp"


namespace behaviors
{

Behavior::Behavior( const ItemPtr & itemThatBehaves, const std::string & behaviorName )
        : nameOfBehavior( behaviorName )
        , item( itemThatBehaves )
        , activity( activities::Activity::Waiting )
        , affectedBy( nilPointer )
{
#if defined( DEBUG ) && DEBUG
        if ( behavior != "behavior of Head" && behavior != "behavior of Heels" && behavior != "behavior of Head over Heels" )
        {
                std::cout << "creation of behavior \"" << behavior << "\" for " << whichItem->whichItemClass()
                                << " \"" << whichItem->getUniqueName() << "\" ( \"" << whichItem->getKind () << "\" )"
                                << " at x=" << whichItem->getX() << " y=" << whichItem->getY() << " z=" << whichItem->getZ()
                                << " with angular orientation \"" << whichItem->getHeading() << "\""
                                << std::endl ;
        }
#endif
}

Behavior::~Behavior( )
{
}

void Behavior::propagateActivity( const Item& sender, const Activity& activity )
{
        Mediator* mediator = sender.getMediator();

        while ( mediator->isThereAnyCollision () ) // there are items collided with the sender
        {
                ItemPtr item = mediator->findCollisionPop( );

                // is it free item or grid item
                if ( item != nilPointer &&
                        ( item->whichItemClass() == "grid item" || item->whichItemClass() == "free item" || item->whichItemClass() == "avatar item" ) )
                {
                        // change activity for item with behavior
                        if ( item->getBehavior() != nilPointer )
                        {
                                item->getBehavior()->setCurrentActivity( activity );
                        }
                }
        }
}

}

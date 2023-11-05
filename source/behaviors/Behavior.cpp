
#include "Behavior.hpp"

#include "Item.hpp"
#include "FreeItem.hpp"
#include "Mediator.hpp"

#include "KindOfActivity.hpp"
#include "Moving.hpp"
#include "Displacing.hpp"
#include "Falling.hpp"


namespace behaviors
{

Behavior::Behavior( const ItemPtr & whichItem, const std::string & behavior )
        : nameOfBehavior( behavior )
        , item( whichItem )
        , activity( activities::Activity::Wait )
        , sender( nilPointer )
{
#if defined( DEBUG ) && DEBUG
        if ( behavior != "behavior of Head" && behavior != "behavior of Heels" && behavior != "behavior of Head over Heels" )
        {
                std::cout << "creation of behavior \"" << behavior << "\" for " << whichItem->whichItemClass()
                                << " \"" << whichItem->getUniqueName() << "\" ( \"" << whichItem->getKind () << "\" )"
                                << " at x=" << whichItem->getX() << " y=" << whichItem->getY() << " z=" << whichItem->getZ()
                                << " with orientation \"" << whichItem->getOrientation() << "\""
                                << std::endl ;
        }
#endif
}

Behavior::~Behavior( )
{
}

void Behavior::propagateActivity( const Item& sender, const ActivityOfItem& activity )
{
        Mediator* mediator = sender.getMediator();

        // as long as there are elements collided with issuer
        while ( ! mediator->isStackOfCollisionsEmpty () )
        {
                ItemPtr item = mediator->findCollisionPop( );

                // is it free item or grid item
                if ( item != nilPointer &&
                        ( item->whichItemClass() == "grid item" || item->whichItemClass() == "free item" || item->whichItemClass() == "avatar item" ) )
                {
                        // change activity for item with behavior
                        if ( item->getBehavior() != nilPointer )
                        {
                                item->getBehavior()->changeActivityOfItem( activity );
                        }
                }
        }
}

}


#include "Behavior.hpp"

#include "Item.hpp"
#include "FreeItem.hpp"
#include "Mediator.hpp"

#include "KindOfActivity.hpp"
#include "MoveKindOfActivity.hpp"
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"


namespace iso
{

Behavior::Behavior( const ItemPtr & whichItem, const std::string & behavior )
        : nameOfBehavior( behavior )
        , item( whichItem )
        , activity( Activity::Wait )
        , sender( nilPointer )
{
#if defined( DEBUG ) && DEBUG
        if ( behavior != "behavior of Head" && behavior != "behavior of Heels" && behavior != "behavior of Head over Heels" )
        {
                std::cout << "creation of behavior \"" << behavior << "\" for " << whichItem->whichKindOfItem()
                                << " \"" << whichItem->getUniqueName() << "\" ( \"" << whichItem->getLabel() << "\" )"
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
                        ( item->whichKindOfItem() == "grid item" || item->whichKindOfItem() == "free item" || item->whichKindOfItem() == "avatar item" ) )
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

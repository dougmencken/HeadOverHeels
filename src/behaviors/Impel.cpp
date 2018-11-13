
#include "Impel.hpp"
#include "Item.hpp"
#include "FreeItem.hpp"
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "Mediator.hpp"
#include "Room.hpp"


namespace iso
{

Impel::Impel( const ItemPtr & item, const std::string & behavior )
        : Behavior( item, behavior )
        , speedTimer( new Timer() )
        , fallTimer( new Timer() )
{
        activity = Activity::Wait ;

        speedTimer->go();
        fallTimer->go();
}

Impel::~Impel()
{
}

bool Impel::update ()
{
        FreeItem& freeItem = dynamic_cast< FreeItem& >( * this->item );
        bool vanish = false;

        switch ( activity )
        {
                case Activity::Wait:
                        break;

                case Activity::DisplaceNorth:
                case Activity::DisplaceSouth:
                case Activity::DisplaceEast:
                case Activity::DisplaceWest:
                case Activity::DisplaceNortheast:
                case Activity::DisplaceNorthwest:
                case Activity::DisplaceSoutheast:
                case Activity::DisplaceSouthwest:
                        // is it time to move
                        if ( speedTimer->getValue() > freeItem.getSpeed() )
                        {
                                if ( ! DisplaceKindOfActivity::getInstance().displace( this, &activity, true ) )
                                {
                                        activity = Activity::Wait;
                                }

                                speedTimer->reset();
                        }

                        freeItem.animate();
                        break;

                case Activity::Fall:
                        // look for reaching floor in a room without floor
                        if ( freeItem.getZ() == 0 && ! freeItem.getMediator()->getRoom()->hasFloor() )
                        {
                                // item disappears
                                vanish = true;
                        }
                        // is it time to fall
                        else if ( fallTimer->getValue() > freeItem.getWeight() )
                        {
                                if ( ! FallKindOfActivity::getInstance().fall( this ) )
                                {
                                        activity = Activity::Wait;
                                }

                                fallTimer->reset();
                        }
                        break;

                default:
                        ;
        }

        return vanish;
}

}


#include "Impel.hpp"

#include "Item.hpp"
#include "FreeItem.hpp"
#include "Displacing.hpp"
#include "Falling.hpp"
#include "Mediator.hpp"
#include "Room.hpp"


namespace behaviors
{

Impel::Impel( const ItemPtr & item, const std::string & behavior )
        : Behavior( item, behavior )
        , speedTimer( new Timer() )
        , fallTimer( new Timer() )
{
        activity = activities::Activity::Wait ;

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
                case activities::Activity::Wait:
                        break;

                case activities::Activity::DisplaceNorth:
                case activities::Activity::DisplaceSouth:
                case activities::Activity::DisplaceEast:
                case activities::Activity::DisplaceWest:
                case activities::Activity::DisplaceNortheast:
                case activities::Activity::DisplaceNorthwest:
                case activities::Activity::DisplaceSoutheast:
                case activities::Activity::DisplaceSouthwest:
                        // is it time to move
                        if ( speedTimer->getValue() > freeItem.getSpeed() )
                        {
                                if ( ! activities::Displacing::getInstance().displace( this, &activity, true ) )
                                {
                                        activity = activities::Activity::Wait;
                                }

                                speedTimer->reset();
                        }

                        freeItem.animate();
                        break;

                case activities::Activity::Fall:
                        // look for reaching floor in a room without floor
                        if ( freeItem.getZ() == 0 && ! freeItem.getMediator()->getRoom()->hasFloor() )
                        {
                                // item disappears
                                vanish = true;
                        }
                        // is it time to fall
                        else if ( fallTimer->getValue() > freeItem.getWeight() )
                        {
                                if ( ! activities::Falling::getInstance().fall( this ) )
                                {
                                        activity = activities::Activity::Wait;
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

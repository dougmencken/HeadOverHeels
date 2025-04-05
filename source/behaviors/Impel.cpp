
#include "Impel.hpp"

#include "FreeItem.hpp"
#include "Displacing.hpp"
#include "Falling.hpp"
#include "Mediator.hpp"
#include "Room.hpp"


namespace behaviors
{

Impel::Impel( FreeItem & item, const std::string & behavior )
        : Behavior( item, behavior )
        , speedTimer( new Timer() )
        , fallTimer( new Timer() )
{
        speedTimer->go() ;
        fallTimer->go() ;
}

bool Impel::update ()
{
        FreeItem & impelItem = dynamic_cast< FreeItem & >( getItem() );

        bool present = true ;

        switch ( getCurrentActivity() )
        {
                case activities::Activity::Waiting:
                        break;

                case activities::Activity::Pushed:
                        // is it time to move
                        if ( speedTimer->getValue() > impelItem.getSpeed() )
                        {
                                if ( ! activities::Displacing::getInstance().displace( *this, true ) )
                                        beWaiting() ;

                                speedTimer->go() ;
                        }

                        impelItem.animate() ;
                        break;

                case activities::Activity::Falling:
                        if ( impelItem.getZ() == 0 && ! impelItem.getMediator()->getRoom()->hasFloor() ) {
                                // disappear if reached the bottom of a room without floor
                                present = false ;
                        }
                        // is it time to fall
                        else if ( fallTimer->getValue() > impelItem.getWeight() )
                        {
                                if ( ! activities::Falling::getInstance().fall( *this ) )
                                        beWaiting() ;

                                fallTimer->go() ;
                        }
                        break;

                default:
                        ;
        }

        return present ;
}

}

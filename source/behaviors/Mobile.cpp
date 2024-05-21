
#include "Mobile.hpp"

#include "FreeItem.hpp"
#include "Room.hpp"
#include "Mediator.hpp"
#include "Displacing.hpp"
#include "Falling.hpp"
#include "SoundManager.hpp"


namespace behaviors
{

Mobile::Mobile( FreeItem & item, const std::string & behavior )
        : Behavior( item, behavior )
        , speedTimer( new Timer() )
        , fallTimer( new Timer() )
{
        speedTimer->go ();
        fallTimer->go ();
}

bool Mobile::update ()
{
        FreeItem & mobileItem = dynamic_cast< FreeItem & >( getItem() );

        bool present = true ;

        switch ( getCurrentActivity () )
        {
                case activities::Activity::Waiting :
                        if ( activities::Falling::getInstance().fall( *this ) ) {
                                // the item falls
                                fallTimer->reset() ;
                                setCurrentActivity( activities::Activity::Falling );
                        }
                        break;

                case activities::Activity::PushedNorth :
                case activities::Activity::PushedSouth :
                case activities::Activity::PushedEast :
                case activities::Activity::PushedWest :
                case activities::Activity::PushedNortheast :
                case activities::Activity::PushedSoutheast :
                case activities::Activity::PushedSouthwest :
                case activities::Activity::PushedNorthwest :
                        // is it time to move
                        if ( speedTimer->getValue() > mobileItem.getSpeed() ) {
                                const ItemPtr & otherItem = getWhatAffectedThisBehavior ();
                                if ( otherItem == nilPointer || otherItem->getUniqueName() != mobileItem.getUniqueName() )
                                        SoundManager::getInstance().play( mobileItem.getKind(), "push" );

                                activities::Displacing::getInstance().displace( *this, true );

                        /* ///// */ if ( speedTimer->getValue() > 1.25 * mobileItem.getSpeed() ) {
                                setCurrentActivity( activities::Activity::Falling );

                                speedTimer->reset() ;
                        /* ///// */ }
                        }

                        mobileItem.animate() ;
                        break;

                case activities::Activity::DraggedNorth :
                case activities::Activity::DraggedSouth :
                case activities::Activity::DraggedEast :
                case activities::Activity::DraggedWest :
                        // on a conveyor
                        if ( speedTimer->getValue() > mobileItem.getSpeed() ) {
                                activities::Displacing::getInstance().displace( *this, true );

                                setCurrentActivity( activities::Activity::Falling );

                                speedTimer->reset() ;
                        }
                        break;

                case activities::Activity::Falling :
                        if ( mobileItem.getZ() == 0 && ! mobileItem.getMediator()->getRoom()->hasFloor() ) {
                                // reached the bottom of a room with no floor
                                present = false ;
                        }
                        // is it time to fall
                        else if ( fallTimer->getValue() > mobileItem.getWeight() ) {
                                if ( ! activities::Falling::getInstance().fall( *this ) ) {
                                        // play the end of the fall sound
                                        SoundManager::getInstance().play( mobileItem.getKind(), "fall" );

                                        setCurrentActivity( activities::Activity::Waiting );
                                }

                                fallTimer->reset() ;
                        }
                        break;

                case activities::Activity::Vanishing :
                        present = false ;
                        break;

                default:
                        ;
        }

        return present ;
}

}

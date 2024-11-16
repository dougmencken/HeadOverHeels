
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
                                fallTimer->go() ;
                                setCurrentActivity( activities::Activity::Falling );
                        }
                        break;

                case activities::Activity::Pushed :
                        // is it time to move
                        if ( speedTimer->getValue() > mobileItem.getSpeed() ) {
                                const AbstractItemPtr & otherItem = getWhatAffectedThisBehavior ();
                                if ( otherItem == nilPointer || otherItem->getUniqueName() != mobileItem.getUniqueName() )
                                        SoundManager::getInstance().play( mobileItem.getKind(), "push" );

                                activities::Displacing::getInstance().displace( *this, true );

                                setCurrentActivity( activities::Activity::Waiting );

                                speedTimer->go() ;
                        }

                        mobileItem.animate() ;
                        break;

                case activities::Activity::Dragged :
                        // on a conveyor
                        if ( speedTimer->getValue() > mobileItem.getSpeed() ) {
                                activities::Displacing::getInstance().displace( *this, true );

                                setCurrentActivity( activities::Activity::Falling );

                                speedTimer->go() ;
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

                                fallTimer->go() ;
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

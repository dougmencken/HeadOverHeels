
#include "Sink.hpp"

#include "GridItem.hpp"
#include "Falling.hpp"
#include "SoundManager.hpp"


namespace behaviors
{

Sink::Sink( GridItem & item, const std::string & behavior )
        : Behavior( item, behavior )
        , fallTimer( new Timer() )
{
        fallTimer->go();
}

bool Sink::update ()
{
        GridItem & sinkingItem = dynamic_cast< GridItem & >( getItem() );

        switch ( getCurrentActivity() )
        {
                case activities::Activity::Waiting:
                        // begin to fall when there’s an item above
                        if ( ! sinkingItem.canAdvanceTo( 0, 0, 1 ) )
                                setCurrentActivity( activities::Activity::Falling );

                        break;

                case activities::Activity::Falling:
                        // is it time to fall down one unit
                        if ( fallTimer->getValue() > sinkingItem.getWeight() ) {
                                // when can’t fall or when there’s no item above it any more
                                if ( ! activities::Falling::getInstance().fall( *this ) || sinkingItem.canAdvanceTo( 0, 0, 1 ) )
                                        setCurrentActivity( activities::Activity::Waiting );

                                fallTimer->reset();
                        }
                        break;

                default:
                        setCurrentActivity( activities::Activity::Waiting );
        }

        SoundManager::getInstance().play( sinkingItem.getKind(), SoundManager::activityToNameOfSound( getCurrentActivity() ) );

        return true ;
}

}

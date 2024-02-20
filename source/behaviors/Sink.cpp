
#include "Sink.hpp"

#include "Item.hpp"
#include "GridItem.hpp"
#include "Falling.hpp"
#include "SoundManager.hpp"


namespace behaviors
{

Sink::Sink( const ItemPtr & item, const std::string & behavior )
        : Behavior( item, behavior )
        , fallTimer( new Timer() )
{
        fallTimer->go();
}

Sink::~Sink()
{
}

bool Sink::update ()
{
        GridItem& gridItem = dynamic_cast< GridItem& >( * this->item );

        switch ( activity )
        {
                case activities::Activity::Waiting:
                        // begin to fall when there’s an item above
                        if ( ! gridItem.canAdvanceTo( 0, 0, 1 ) )
                        {
                                this->setActivityOfItem( activities::Activity::Fall );
                        }
                        break;

                case activities::Activity::Fall:
                        // is it time to lower one unit yet
                        if ( fallTimer->getValue() > gridItem.getWeight() )
                        {
                                // when can’t fall any more or when there’s no item above it
                                if ( ! activities::Falling::getInstance().fall( this ) || gridItem.canAdvanceTo( 0, 0, 1 ) )
                                {
                                        activity = activities::Activity::Waiting ;
                                }

                                fallTimer->reset();
                        }
                        break;

                default:
                        activity = activities::Activity::Waiting;
        }

        SoundManager::getInstance().play( gridItem.getKind (), SoundManager::activityToString( activity ) );

        return false;
}

}

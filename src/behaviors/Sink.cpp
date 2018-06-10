
#include "Sink.hpp"
#include "Item.hpp"
#include "GridItem.hpp"
#include "FallKindOfActivity.hpp"
#include "SoundManager.hpp"


namespace isomot
{

Sink::Sink( Item * item, const std::string & behavior ) :
        Behavior( item, behavior )
{
        fallTimer = new Timer();
        fallTimer->go();
}

Sink::~Sink()
{
        delete fallTimer;
}

bool Sink::update ()
{
        GridItem * gridItem = dynamic_cast< GridItem * >( this->item );

        switch ( activity )
        {
                case Wait:
                        // begin to fall when there’s an item on top
                        if ( ! gridItem->canAdvanceTo( 0, 0, 1 ) )
                        {
                                this->changeActivityOfItem( Fall );
                        }
                        break;

                case Fall:
                        // is it time to lower one unit yet
                        if ( fallTimer->getValue() > gridItem->getWeight() )
                        {
                                // when can’t fall any more or when there’s no item on top any longer
                                if ( ! FallKindOfActivity::getInstance()->fall( this )
                                        || gridItem->canAdvanceTo( 0, 0, 1 ) )
                                {
                                        activity = Wait;
                                }

                                fallTimer->reset();
                        }
                        break;

                default:
                        activity = Wait;
        }

        SoundManager::getInstance()->play( gridItem->getLabel(), activity );

        return false;
}

}


#include "Trampoline.hpp"
#include "Item.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "SoundManager.hpp"


namespace iso
{

Trampoline::Trampoline( const ItemPtr & item, const std::string & behavior ) :
        Behavior( item, behavior )
        , folded( false )
        , rebounding( false )
        , plainFrame( 0 )
        , foldedFrame( 1 )
        , speedTimer( new Timer() )
        , fallTimer( new Timer() )
        , reboundTimer( new Timer() )
{
        speedTimer->go();
        fallTimer->go();
        reboundTimer->go();
}

Trampoline::~Trampoline()
{
}

bool Trampoline::update ()
{
        FreeItem& freeItem = dynamic_cast< FreeItem& >( * this->item );
        bool vanish = false;

        switch ( activity )
        {
                case Activity::Wait:
                        // fold trampoline when there are items on top of it
                        if ( ! freeItem.canAdvanceTo( 0, 0, 1 ) )
                        {
                                folded = true;
                                rebounding = false;
                                freeItem.changeFrame( foldedFrame );
                        }
                        else
                        {
                                // continue to bounce trampoline
                                if ( rebounding && reboundTimer->getValue() < 0.600 )
                                {
                                        freeItem.animate();

                                        // play sound of bouncing
                                        if ( reboundTimer->getValue() > 0.100 )
                                        {
                                                SoundManager::getInstance().play( freeItem.getLabel(), Activity::IsActive );
                                        }
                                }
                                else
                                {
                                        // begin bouncing when item on top moves away
                                        if ( folded )
                                        {
                                                rebounding = true;
                                                reboundTimer->reset();
                                        }

                                        // it is no longer folded
                                        folded = false;

                                        freeItem.changeFrame( plainFrame );
                                }
                        }

                        // look if it falls down
                        if ( FallKindOfActivity::getInstance().fall( this ) )
                        {
                                fallTimer->reset();
                                activity = Activity::Fall;
                        }
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
                                // play sound of displacing
                                SoundManager::getInstance().play( freeItem.getLabel(), activity );

                                this->changeActivityOfItem( activity );
                                DisplaceKindOfActivity::getInstance().displace( this, &activity, true );

                                if ( activity != Activity::Fall )
                                {
                                        activity = Activity::Wait;
                                }

                                speedTimer->reset();
                        }
                        break;

                case Activity::Fall:
                        // look for reaching floor in a room without floor
                        if ( item->getZ() == 0 && ! item->getMediator()->getRoom()->hasFloor() )
                        {
                                // item disappears
                                vanish = true;
                        }
                        // is it time to lower by one unit
                        else if ( fallTimer->getValue() > freeItem.getWeight() )
                        {
                                // item falls
                                this->changeActivityOfItem( activity );
                                if ( ! FallKindOfActivity::getInstance().fall( this ) )
                                {
                                        // play sound of falling down
                                        SoundManager::getInstance().play( freeItem.getLabel(), activity );
                                        activity = Activity::Wait;
                                }

                                fallTimer->reset();
                        }
                        break;

                case Activity::Vanish:
                        vanish = true;
                        break;

                default:
                        ;
        }

        return vanish;
}

}


#include "Trampoline.hpp"
#include "Item.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "SoundManager.hpp"


namespace isomot
{

Trampoline::Trampoline( Item * item, const std::string & behavior ) :
        Behavior( item, behavior )
        , folded( false )
        , rebounding( false )
        , regularFrame( 0 )
        , foldedFrame( 1 )
{
        speedTimer = new Timer();
        fallTimer = new Timer();
        reboundTimer = new Timer();

        speedTimer->go();
        fallTimer->go();
        reboundTimer->go();
}

Trampoline::~Trampoline()
{
        delete speedTimer;
        delete fallTimer;
        delete reboundTimer;
}

bool Trampoline::update ()
{
        FreeItem * freeItem = dynamic_cast< FreeItem * >( this->item );
        bool vanish = false;

        switch ( activity )
        {
                case Wait:
                        // fold trampoline when there are items on top of it
                        if ( ! freeItem->canAdvanceTo( 0, 0, 1 ) )
                        {
                                folded = true;
                                rebounding = false;
                                freeItem->changeFrame( foldedFrame );
                        }
                        else
                        {
                                // continue to bounce trampoline
                                if ( rebounding && reboundTimer->getValue() < 0.600 )
                                {
                                        freeItem->animate();

                                        // play sound of bouncing
                                        if ( reboundTimer->getValue() > 0.100 )
                                        {
                                                SoundManager::getInstance()->play( freeItem->getLabel(), IsActive );
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

                                        freeItem->changeFrame(regularFrame);
                                }
                        }

                        // look if it falls down
                        if ( FallKindOfActivity::getInstance()->fall( this ) )
                        {
                                fallTimer->reset();
                                activity = Fall;
                        }
                        break;

                case DisplaceNorth:
                case DisplaceSouth:
                case DisplaceEast:
                case DisplaceWest:
                case DisplaceNortheast:
                case DisplaceNorthwest:
                case DisplaceSoutheast:
                case DisplaceSouthwest:
                        // is it time to move
                        if ( speedTimer->getValue() > freeItem->getSpeed() )
                        {
                                // play sound of displacing
                                SoundManager::getInstance()->play( freeItem->getLabel(), activity );

                                this->changeActivityOfItem( activity );
                                DisplaceKindOfActivity::getInstance()->displace( this, &activity, true );

                                if ( activity != Fall )
                                {
                                        activity = Wait;
                                }

                                speedTimer->reset();
                        }
                        break;

                case Fall:
                        // look for reaching floor in a room without floor
                        if ( item->getZ() == 0 && item->getMediator()->getRoom()->getKindOfFloor() == "none" )
                        {
                                // item disappears
                                vanish = true;
                        }
                        // is it time to lower by one unit
                        else if ( fallTimer->getValue() > freeItem->getWeight() )
                        {
                                // item falls
                                this->changeActivityOfItem( activity );
                                if ( ! FallKindOfActivity::getInstance()->fall( this ) )
                                {
                                        // play sound of falling down
                                        SoundManager::getInstance()->play( freeItem->getLabel(), activity );
                                        activity = Wait;
                                }

                                fallTimer->reset();
                        }
                        break;

                case Vanish:
                        vanish = true;
                        break;

                default:
                        ;
        }

        return vanish;
}

}

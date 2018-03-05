
#include "Turn.hpp"
#include "Item.hpp"
#include "ItemData.hpp"
#include "FreeItem.hpp"
#include "MoveKindOfActivity.hpp"
#include "DisplaceKindOfActivity.hpp"
#include "FallKindOfActivity.hpp"
#include "Mediator.hpp"
#include "Room.hpp"
#include "SoundManager.hpp"


namespace isomot
{

Turn::Turn( Item * item, const std::string & behavior ) :
        Behavior( item, behavior )
{
        speedTimer = new Timer();
        fallTimer = new Timer();
        speedTimer->go();
        fallTimer->go();
}

Turn::~Turn()
{
        delete speedTimer;
        delete fallTimer;
}

bool Turn::update ()
{
        bool isGone = false;
        FreeItem* freeItem = dynamic_cast< FreeItem * >( this->item );

        switch ( activity )
        {
                case Wait:
                        begin();
                        break;

                case MoveNorth:
                case MoveSouth:
                case MoveEast:
                case MoveWest:
                        if ( ! freeItem->isFrozen() )
                        {
                                if ( speedTimer->getValue() > freeItem->getSpeed() )
                                {
                                        if ( ! MoveKindOfActivity::getInstance()->move( this, &activity, true ) )
                                        {
                                                turn();

                                                SoundManager::getInstance()->play( freeItem->getLabel(), Collision );
                                        }

                                        speedTimer->reset();
                                }

                                freeItem->animate();
                        }
                        break;

                case DisplaceNorth:
                case DisplaceSouth:
                case DisplaceEast:
                case DisplaceWest:
                case DisplaceNortheast:
                case DisplaceSoutheast:
                case DisplaceSouthwest:
                case DisplaceNorthwest:
                        SoundManager::getInstance()->play( freeItem->getLabel(), activity );

                        DisplaceKindOfActivity::getInstance()->displace( this, &activity, true );

                        activity = Wait;

                        // inactive item continues to be inactive
                        if ( freeItem->isFrozen() )
                        {
                                activity = Freeze;
                        }
                        break;

                case Fall:
                        // look for reaching floor in a room without floor
                        if ( freeItem->getZ() == 0 && freeItem->getMediator()->getRoom()->getKindOfFloor() == "none" )
                        {
                                isGone = true;
                        }
                        // is it time to lower by one unit
                        else if ( fallTimer->getValue() > freeItem->getWeight() )
                        {
                                if ( ! FallKindOfActivity::getInstance()->fall( this ) )
                                {
                                        SoundManager::getInstance()->play( freeItem->getLabel(), activity );
                                        activity = Wait;
                                }

                                fallTimer->reset();
                        }
                        break;

                case Freeze:
                        freeItem->setFrozen( true );
                        break;

                case WakeUp:
                        freeItem->setFrozen( false );
                        activity = Wait;
                        break;

                default:
                        ;
        }

        return isGone;
}

void Turn::begin()
{
        switch ( this->item->getOrientation().getIntegerOfWay () )
        {
                case North:
                        activity = MoveNorth;
                        break;

                case South:
                        activity = MoveSouth;
                        break;

                case East:
                        activity = MoveEast;
                        break;

                case West:
                        activity = MoveWest;
                        break;

                default:
                        ;
        }
}

void Turn::turn()
{
        switch ( this->item->getOrientation().getIntegerOfWay () )
        {
                case North:
                        activity = ( getNameOfBehavior() == "behavior of move then turn left and move" ? MoveWest : MoveEast );
                        this->item->changeOrientation( getNameOfBehavior() == "behavior of move then turn left and move" ? West : East );
                        break;

                case South:
                        activity = ( getNameOfBehavior() == "behavior of move then turn left and move" ? MoveEast : MoveWest );
                        this->item->changeOrientation( getNameOfBehavior() == "behavior of move then turn left and move" ? East : West );
                        break;

                case East:
                        activity = ( getNameOfBehavior() == "behavior of move then turn left and move" ? MoveNorth : MoveSouth );
                        this->item->changeOrientation( getNameOfBehavior() == "behavior of move then turn left and move" ? North : South );
                        break;

                case West:
                        activity = ( getNameOfBehavior() == "behavior of move then turn left and move" ? MoveSouth : MoveNorth );
                        this->item->changeOrientation( getNameOfBehavior() == "behavior of move then turn left and move" ? South : North );
                        break;

                default:
                        ;
        }
}

}

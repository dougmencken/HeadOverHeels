
#include "Doughnut.hpp"
#include "Mediator.hpp"
#include "FreeItem.hpp"
#include "PlayerItem.hpp"
#include "UserControlled.hpp"
#include "MoveKindOfActivity.hpp"


namespace isomot
{

Doughnut::Doughnut( Item * item, const std::string & behavior )
        : Behavior( item, behavior )
        , playerItem( 0 )
{
        speedTimer = new Timer();
        speedTimer->go();
}

Doughnut::~Doughnut( )
{
        delete speedTimer;
}

bool Doughnut::update ()
{
        FreeItem* freeItem = dynamic_cast< FreeItem * >( this->item );
        bool vanish = false;

        switch ( activity )
        {
                case Activity::Wait:
                        switch ( this->item->getOrientation().getIntegerOfWay () )
                        {
                                case Way::North:
                                        activity = Activity::MoveNorth;
                                        break;

                                case Way::South:
                                        activity = Activity::MoveSouth;
                                        break;

                                case Way::East:
                                        activity = Activity::MoveEast;
                                        break;

                                case Way::West:
                                        activity = Activity::MoveWest;
                                        break;

                                default:
                                        ;
                        }

                        break;

                case Activity::MoveNorth:
                case Activity::MoveSouth:
                case Activity::MoveEast:
                case Activity::MoveWest:
                        if ( speedTimer->getValue() > freeItem->getSpeed() )
                        {
                                // look for collisions with items
                                freeItem->setCollisionDetector( true );

                                if ( activity == Activity::MoveNorth ) {
                                        // -1, 0, 0 for collisions at north
                                        freeItem->canAdvanceTo( -1, 0, 0 );
                                } else if ( activity == Activity::MoveSouth ) {
                                        // 1, 0, 0 for collisions at south
                                        freeItem->canAdvanceTo( 1, 0, 0 );
                                } else if ( activity == Activity::MoveEast ) {
                                        // 0, -1, 0 for collisions at east
                                        freeItem->canAdvanceTo( 0, -1, 0 );
                                } else if ( activity == Activity::MoveWest ) {
                                        // 0, 1, 0 for collisions at west
                                        freeItem->canAdvanceTo( 0, 1, 0 );
                                }

                                // if there’s no collision or collision is with player, move doughnut
                                if ( freeItem->getMediator()->isStackOfCollisionsEmpty() || freeItem->getMediator()->collisionWithByLabel( playerItem->getLabel() ) )
                                {
                                        freeItem->setCollisionDetector( false );
                                        MoveKindOfActivity::getInstance()->move( this, &activity, false );
                                }
                                else
                                {
                                        // freeze “ bad boy ” on collision with it
                                        if ( freeItem->getMediator()->collisionWithBadBoy() )
                                        {
                                                propagateActivity( this->item, Activity::Freeze );
                                        }

                                        // doughnut disappears after collison with any item but player
                                        dynamic_cast< UserControlled * >( playerItem->getBehavior() )->setFireFromHooter( false );
                                        vanish = true;
                                }

                                speedTimer->reset();

                                freeItem->animate();
                        }
                        break;

                default:
                        ;
        }

        return vanish;
}

}

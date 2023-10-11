
#include "Doughnut.hpp"

#include "Mediator.hpp"
#include "FreeItem.hpp"
#include "UserControlled.hpp"
#include "MoveKindOfActivity.hpp"


namespace iso
{

Doughnut::Doughnut( const ItemPtr & item, const std::string & behavior )
        : Behavior( item, behavior )
        , character( nilPointer )
        , speedTimer( new Timer() )
{
        speedTimer->go();
}

Doughnut::~Doughnut( )
{
}

bool Doughnut::update ()
{
        FreeItem& freeItem = dynamic_cast< FreeItem& >( * this->item );
        bool vanish = false;

        switch ( activity )
        {
                case Activity::Wait:
                        switch ( Way( item->getOrientation() ).getIntegerOfWay () )
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
                        if ( speedTimer->getValue() > freeItem.getSpeed() )
                        {
                                // look for collisions with items
                                freeItem.setCollisionDetector( true );

                                if ( activity == Activity::MoveNorth ) {
                                        // -1, 0, 0 for collisions at north
                                        freeItem.canAdvanceTo( -1, 0, 0 );
                                } else if ( activity == Activity::MoveSouth ) {
                                        // 1, 0, 0 for collisions at south
                                        freeItem.canAdvanceTo( 1, 0, 0 );
                                } else if ( activity == Activity::MoveEast ) {
                                        // 0, -1, 0 for collisions at east
                                        freeItem.canAdvanceTo( 0, -1, 0 );
                                } else if ( activity == Activity::MoveWest ) {
                                        // 0, 1, 0 for collisions at west
                                        freeItem.canAdvanceTo( 0, 1, 0 );
                                }

                                Mediator* mediator = freeItem.getMediator() ;
                                bool collisionWithCharacter = ( mediator->collisionWithByLabel( this->character->getOriginalLabel() ) != nilPointer ) ;

                                // if no collisions or a collision with the character
                                if ( mediator->isStackOfCollisionsEmpty() || collisionWithCharacter )
                                {
                                        freeItem.setCollisionDetector( false );

                                        // move a doughnut
                                        MoveKindOfActivity::getInstance().move( this, &activity, false );
                                }
                                else
                                {
                                        // freeze “ bad boy ” on a collision with it
                                        if ( mediator->collisionWithBadBoy() != nilPointer )
                                        {
                                                propagateActivity( *this->item, Activity::Freeze );
                                        }

                                        // doughnut disappears after collison with any item but player
                                        dynamic_cast< UserControlled * >( this->character->getBehavior() )->setFireFromHooter( false );
                                        vanish = true;
                                }

                                speedTimer->reset();

                                freeItem.animate();
                        }
                        break;

                default:
                        ;
        }

        return vanish;
}

}

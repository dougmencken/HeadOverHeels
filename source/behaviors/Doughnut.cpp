
#include "Doughnut.hpp"

#include "Mediator.hpp"
#include "FreeItem.hpp"
#include "PlayerControlled.hpp"
#include "Moving.hpp"


namespace behaviors
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
                case activities::Activity::Waiting:
                        switch ( Way( item->getHeading() ).getIntegerOfWay () )
                        {
                                case Way::North:
                                        activity = activities::Activity::MovingNorth;
                                        break;

                                case Way::South:
                                        activity = activities::Activity::MovingSouth;
                                        break;

                                case Way::East:
                                        activity = activities::Activity::MovingEast;
                                        break;

                                case Way::West:
                                        activity = activities::Activity::MovingWest;
                                        break;

                                default:
                                        ;
                        }

                        break;

                case activities::Activity::MovingNorth:
                case activities::Activity::MovingSouth:
                case activities::Activity::MovingEast:
                case activities::Activity::MovingWest:
                        if ( speedTimer->getValue() > freeItem.getSpeed() )
                        {
                                // look for collisions with other items
                                freeItem.setIgnoreCollisions( false );

                                if ( activity == activities::Activity::MovingNorth ) {
                                        // -1, 0, 0 for collisions at north
                                        freeItem.canAdvanceTo( -1, 0, 0 );
                                } else if ( activity == activities::Activity::MovingSouth ) {
                                        // 1, 0, 0 for collisions at south
                                        freeItem.canAdvanceTo( 1, 0, 0 );
                                } else if ( activity == activities::Activity::MovingEast ) {
                                        // 0, -1, 0 for collisions at east
                                        freeItem.canAdvanceTo( 0, -1, 0 );
                                } else if ( activity == activities::Activity::MovingWest ) {
                                        // 0, 1, 0 for collisions at west
                                        freeItem.canAdvanceTo( 0, 1, 0 );
                                }

                                Mediator* mediator = freeItem.getMediator() ;
                                bool collisionWithCharacter = ( mediator->collisionWithSomeKindOf( this->character->getOriginalKind() ) != nilPointer ) ;

                                // if no collisions or a collision with the character
                                if ( ! mediator->isThereAnyCollision() || collisionWithCharacter )
                                {
                                        freeItem.setIgnoreCollisions( true );

                                        // move a doughnut
                                        activities::Moving::getInstance().move( this, &activity, false );
                                }
                                else
                                {
                                        // freeze “ bad boy ” on a collision with it
                                        if ( mediator->collisionWithBadBoy() != nilPointer )
                                        {
                                                propagateActivity( *this->item, activities::Activity::Freeze );
                                        }

                                        // a doughnut disappears after a collison
                                        dynamic_cast< PlayerControlled * >( this->character->getBehavior().get () )->unsetDoughnutInRoom() ;
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

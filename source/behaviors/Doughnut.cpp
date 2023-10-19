
#include "Doughnut.hpp"

#include "Mediator.hpp"
#include "FreeItem.hpp"
#include "PlayerControlled.hpp"
#include "MoveKindOfActivity.hpp"


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
                case activities::Activity::Wait:
                        switch ( Way( item->getOrientation() ).getIntegerOfWay () )
                        {
                                case Way::North:
                                        activity = activities::Activity::MoveNorth;
                                        break;

                                case Way::South:
                                        activity = activities::Activity::MoveSouth;
                                        break;

                                case Way::East:
                                        activity = activities::Activity::MoveEast;
                                        break;

                                case Way::West:
                                        activity = activities::Activity::MoveWest;
                                        break;

                                default:
                                        ;
                        }

                        break;

                case activities::Activity::MoveNorth:
                case activities::Activity::MoveSouth:
                case activities::Activity::MoveEast:
                case activities::Activity::MoveWest:
                        if ( speedTimer->getValue() > freeItem.getSpeed() )
                        {
                                // look for collisions with other items
                                freeItem.setIgnoreCollisions( false );

                                if ( activity == activities::Activity::MoveNorth ) {
                                        // -1, 0, 0 for collisions at north
                                        freeItem.canAdvanceTo( -1, 0, 0 );
                                } else if ( activity == activities::Activity::MoveSouth ) {
                                        // 1, 0, 0 for collisions at south
                                        freeItem.canAdvanceTo( 1, 0, 0 );
                                } else if ( activity == activities::Activity::MoveEast ) {
                                        // 0, -1, 0 for collisions at east
                                        freeItem.canAdvanceTo( 0, -1, 0 );
                                } else if ( activity == activities::Activity::MoveWest ) {
                                        // 0, 1, 0 for collisions at west
                                        freeItem.canAdvanceTo( 0, 1, 0 );
                                }

                                Mediator* mediator = freeItem.getMediator() ;
                                bool collisionWithCharacter = ( mediator->collisionWithByLabel( this->character->getOriginalLabel() ) != nilPointer ) ;

                                // if no collisions or a collision with the character
                                if ( mediator->isStackOfCollisionsEmpty() || collisionWithCharacter )
                                {
                                        freeItem.setIgnoreCollisions( true );

                                        // move a doughnut
                                        activities::MoveKindOfActivity::getInstance().move( this, &activity, false );
                                }
                                else
                                {
                                        // freeze “ bad boy ” on a collision with it
                                        if ( mediator->collisionWithBadBoy() != nilPointer )
                                        {
                                                propagateActivity( *this->item, activities::Activity::Freeze );
                                        }

                                        // a doughnut disappears after a collison
                                        dynamic_cast< PlayerControlled * >( this->character->getBehavior().get () )->setFireFromHooter( false );
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

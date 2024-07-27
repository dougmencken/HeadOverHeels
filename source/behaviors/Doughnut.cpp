
#include "Doughnut.hpp"

#include "FreeItem.hpp"
#include "AvatarItem.hpp"
#include "Mediator.hpp"
#include "PlayerControlled.hpp"
#include "Moving.hpp"
#include "PropagateActivity.hpp"


namespace behaviors
{

Doughnut::Doughnut( FreeItem & item, const std::string & behavior )
        : Behavior( item, behavior )
        , speedTimer( new Timer() )
{
        speedTimer->go();
}

bool Doughnut::update ()
{
        FreeItem & donutItem = dynamic_cast< FreeItem & >( getItem () );

        bool present = true ;

        switch ( getCurrentActivity() )
        {
                case activities::Activity::Waiting:
                {
                        const std::string & heading = donutItem.getHeading() ;

                             if ( heading == "north" ) setCurrentActivity( activities::Activity::MovingNorth );
                        else if ( heading == "south" ) setCurrentActivity( activities::Activity::MovingSouth );
                        else if ( heading == "east"  ) setCurrentActivity( activities::Activity::MovingEast );
                        else if ( heading == "west"  ) setCurrentActivity( activities::Activity::MovingWest );
                }
                        break;

                case activities::Activity::MovingNorth:
                case activities::Activity::MovingSouth:
                case activities::Activity::MovingEast:
                case activities::Activity::MovingWest:
                        if ( speedTimer->getValue() > donutItem.getSpeed() )
                        {
                                // look for collisions with other items
                                donutItem.setIgnoreCollisions( false );

                                Activity busyness = getCurrentActivity() ;

                                if ( busyness == activities::Activity::MovingNorth ) {
                                        // for collisions on north
                                        donutItem.canAdvanceTo( -1, 0, 0 );
                                }
                                else if ( busyness == activities::Activity::MovingSouth ) {
                                        // for collisions on south
                                        donutItem.canAdvanceTo( 1, 0, 0 );
                                }
                                else if ( busyness == activities::Activity::MovingEast ) {
                                        // for collisions on east
                                        donutItem.canAdvanceTo( 0, -1, 0 );
                                }
                                else if ( busyness == activities::Activity::MovingWest ) {
                                        // for collisions on west
                                        donutItem.canAdvanceTo( 0, 1, 0 );
                                }

                                Mediator* mediator = donutItem.getMediator() ;

                                // is there initial collision with the character who released the doughnut
                                bool initialCollision = false ;
                                const std::vector< AvatarItemPtr > & charactersInRoom = mediator->getRoom()->getCharactersYetInRoom() ;
                                for ( unsigned int c = 0 ; c < charactersInRoom.size() ; ++ c )
                                        if ( mediator->collisionWithSomeKindOf( charactersInRoom[ c ]->getOriginalKind() ) != nilPointer
                                                        && charactersInRoom[ c ]->hasTool( "horn" ) ) {
                                                initialCollision = true ;
                                                break ;
                                        }

                                // ignore such collisions
                                if ( initialCollision ) donutItem.setIgnoreCollisions( true );

                                // if no collisions or an initial collision
                                if ( ! mediator->isThereAnyCollision() || initialCollision )
                                {
                                        // move the doughnut
                                        activities::Moving::getInstance().move( *this, false );
                                }
                                else {
                                        // freeze a bad boy on a collision
                                        if ( mediator->collisionWithBadBoy() != nilPointer )
                                                activities::PropagateActivity::spreadEasily( donutItem, activities::Activity::Freeze );

                                        // the doughnut disappears after a collision
                                        present = false ;
                                }

                                speedTimer->go() ;

                                donutItem.animate() ;
                        }
                        break;

                default:
                        ;
        }

        return present ;
}

}


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

                             if ( heading == "north" ) setCurrentActivity( activities::Activity::Moving, Motion2D::movingNorth() );
                        else if ( heading == "south" ) setCurrentActivity( activities::Activity::Moving, Motion2D::movingSouth() );
                        else if ( heading == "east"  ) setCurrentActivity( activities::Activity::Moving, Motion2D::movingEast() );
                        else if ( heading == "west"  ) setCurrentActivity( activities::Activity::Moving, Motion2D::movingWest() );
                }
                        break ;

                case activities::Activity::Moving:
                        if ( speedTimer->getValue() > donutItem.getSpeed() )
                        {
                                Motion2D vector = get2DVelocityVector() ;

                                // look for collisions with other items
                                donutItem.setIgnoreCollisions( false );
                                donutItem.canAdvanceTo( vector.getMotionAlongX(), vector.getMotionAlongY(), 0 );

                                Mediator* mediator = donutItem.getMediator() ;
                                bool collision = mediator->isThereAnyCollision() ;

                                // maybe there’s an initial collision with the character who released a doughnut
                                bool initialCollision = false ;
                                const std::vector< AvatarItemPtr > & charactersInRoom = mediator->getRoom()->getCharactersYetInRoom() ;
                                std::string whoReleased( "" ) ;

                                for ( unsigned int c = 0 ; c < charactersInRoom.size() ; ++ c )
                                        if ( charactersInRoom[ c ]->hasTool( "horn" ) ) {
                                                whoReleased = charactersInRoom[ c ]->getOriginalKind() ;
                                                break ;
                                        }

                                initialCollision = ( mediator->collisionWithSomeKindOf( whoReleased ) != nilPointer );

                                // ignore such collisions
                                if ( initialCollision ) donutItem.setIgnoreCollisions( true );

                                // if no collisions or an initial collision
                                if ( ! collision || initialCollision ) {
                                        // move the doughnut
                                        activities::Moving::getInstance().move( *this, false );
                                }
                                else {
                                        // freeze a bad boy on a collision
                                        if ( mediator->collisionWithBadBoy() != nilPointer )
                                                activities::PropagateActivity::spreadEasily( donutItem, activities::Activity::Freeze, Motion2D::rest() );

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


#include "CreatorOfBehaviors.hpp"

#include "Conveyor.hpp"
#include "Detector.hpp"
#include "Driven.hpp"
#include "Elevator.hpp"
#include "Hunter.hpp"
#include "Impel.hpp"
#include "Mobile.hpp"
#include "ThereAndBack.hpp"
#include "Patrol.hpp"
#include "RemoteControl.hpp"
#include "Sink.hpp"
#include "Doughnut.hpp"
#include "Bonus.hpp"
#include "Switch.hpp"
#include "Teleport.hpp"
#include "Trampoline.hpp"
#include "Turn.hpp"
#include "Volatile.hpp"
#include "FinalBall.hpp"

#include "CharacterHead.hpp"
#include "CharacterHeels.hpp"
#include "CharacterHeadAndHeels.hpp"

#include "AvatarItem.hpp"
#include "FreeItem.hpp"
#include "GridItem.hpp"

#include "util.hpp"


namespace behaviors
{

autouniqueptr< Behavior > CreatorOfBehaviors::createBehaviorByName( AbstractItem & item, const std::string & behavior )
{
        Behavior * behaviorToReturn = nilPointer ;

        if ( item.whichItemClass() == "avatar item" )
        {
                ::AvatarItem & avatar = dynamic_cast< ::AvatarItem & >( item );

                if ( behavior == "behavior of Head" )
                        behaviorToReturn = new CharacterHead( avatar );
                else
                if ( behavior == "behavior of Heels" )
                        behaviorToReturn = new CharacterHeels( avatar );
                else
                if ( behavior == "behavior of Head over Heels" )
                        behaviorToReturn = new CharacterHeadAndHeels( avatar );
        }
        else if ( item.whichItemClass() == "free item" )
        {
                ::FreeItem & free = dynamic_cast< ::FreeItem & >( item );

                if ( behavior == "behavior of bonus" )
                        behaviorToReturn = new Bonus( free, behavior );
                else
                if ( behavior == "behavior of detector" )
                        behaviorToReturn = new Detector( free, behavior );
                else
                if ( behavior == "behavior of freezing doughnut" )
                        behaviorToReturn = new Doughnut( free, behavior );
                else
                if ( behavior == "behavior of driven" )
                        behaviorToReturn = new Driven( free, behavior );
                else
                if ( behavior == "behavior of elevator" )
                        behaviorToReturn = new Elevator( free, behavior );
                else
                if ( behavior == "behaivor of final ball" )
                        behaviorToReturn = new FinalBall( free, behavior );
                else
                if ( behavior == "behavior of hunter in four directions" ||
                                behavior == "behavior of waiting hunter in four directions" ||
                                behavior == "behavior of hunter in eight directions" ||
                                behavior == "behavior of waiting hunter in eight directions" )
                        behaviorToReturn = new Hunter( free, behavior );
                else
                if ( behavior == "behavior of impel" )
                        behaviorToReturn = new Impel( free, behavior );
                else
                if ( behavior == "behavior of thing able to move by pushing" )
                        behaviorToReturn = new Mobile( free, behavior );
                else
                if ( behavior == "behavior of random patroling in four primary directions" ||
                                behavior == "behavior of random patroling in four secondary directions" ||
                                behavior == "behavior of random patroling in eight directions" )
                        behaviorToReturn = new Patrol( free, behavior );
                else
                if ( behavior == "behavior of remote control"
                                || behavior == "behavior of remotely controlled one" )
                        behaviorToReturn = new RemoteControl( free, behavior );
                else
                if ( behavior == "behavior of switch" )
                        behaviorToReturn = new Switch( free, behavior );
                else
                if ( behavior == "behavior of there and back" )
                        behaviorToReturn = new ThereAndBack( free, behavior, false );
                else
                if ( behavior == "behavior of flying there and back" )
                        behaviorToReturn = new ThereAndBack( free, behavior, true );
                else
                if ( behavior == "behavior of spring stool" )
                        behaviorToReturn = new Trampoline( free, behavior );
                else
                if ( behavior == "behavior of move then turn left and move"
                                || behavior == "behavior of move then turn right and move" )
                        behaviorToReturn = new Turn( free, behavior );
        }
        else if ( item.whichItemClass() == "grid item" )
        {
                ::GridItem & onGrid = dynamic_cast< ::GridItem & >( item );

                if ( behavior == "behavior of conveyor" )
                        behaviorToReturn = new Conveyor( onGrid, behavior );
                else
                if ( behavior == "behavior of sinking downward" )
                        behaviorToReturn = new Sink( onGrid, behavior );
                else
                if ( behavior == "behavior of teletransport" )
                        behaviorToReturn = new Teleport( onGrid, behavior );
        }

        if ( behaviorToReturn == nilPointer ) // none of the above
        {
                if ( behavior.find( "vanishing" ) != std::string::npos /* && item.whichItemClass() == "described item" */ )
                        behaviorToReturn = new Volatile( dynamic_cast< ::DescribedItem & >( item ), behavior );
                else
                if ( behavior.empty () || behavior == "still" || behavior == "behavior of bubbles" )
                {
                        // no behavior, return nil
                }
                else {
                        std::cerr << "unknown behavior \"" << behavior << "\" for "
                                        << item.whichItemClass() << " \"" << item.getUniqueName()
                                                << "\" in CreatorOfBehaviors::createBehaviorByName" << std::endl ;
                }
        }

        return autouniqueptr< Behavior >( behaviorToReturn );
}

}

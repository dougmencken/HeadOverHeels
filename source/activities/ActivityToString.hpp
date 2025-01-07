// The free and open source remake of Head over Heels
//
// Copyright © 2025 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef ActivityToString_hpp_
#define ActivityToString_hpp_

#include "Elevator.hpp"

#include <sstream>

namespace activities
{

class ActivityToString
{

public:

        static std::string toString( unsigned int activity )
        {
                switch ( activity ) {
                        case Activity::Waiting : return "waiting" ;
                        case Activity::Blinking : return "blinking" ;

                        case Activity::Moving : return "moving" ;

                        case Activity::Automoving : return "automoving" ;

                        case Activity::Jumping : return "jumping" ;
                        case Activity::Falling : return "falling" ;
                        case Activity::Gliding : return "gliding" ;

                        case Activity::TakingItem : return "taking" ;
                        case Activity::DroppingItem : return "dropping" ;
                        case Activity::TakeAndJump : return "take & jump" ;
                        case Activity::DropAndJump : return "drop & jump" ;

                        case Activity::Pushed : return "pushed" ;

                        case Activity::PushedUp : return "pushed up" ;

                        case Activity::Dragged : return "dragged" ;

                        case Activity::CancelDragging : return "cancel dragging" ;

                        case ActivityOfElevator::GoingDown : return "elevator is going down" ;
                        case ActivityOfElevator::GoingUp : return "elevator is going up" ;
                        case ActivityOfElevator::ReachedBottom : return "elevator reached bottom" ;
                        case ActivityOfElevator::ReachedTop : return "elevator reached top" ;

                        case Activity::BeginTeletransportation : return "begin teletransportation" ;
                        case Activity::EndTeletransportation : return "end teletransportation" ;

                        case Activity::Freeze : return "freeze" ;
                        case Activity::WakeUp : return "wake up" ;

                        case Activity::MetLethalItem : return "met a lethal item" ;
                        case Activity::Vanishing : return "vanishing" ;

                        case Activity::Mistake : return "mistake" ;
                }

                std::ostringstream otherActivity ;
                otherActivity << "some other activity (" << std::dec << activity << ")" ;
                return otherActivity.str () ;
        }

} ;

}

#endif

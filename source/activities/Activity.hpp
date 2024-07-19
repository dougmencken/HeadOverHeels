// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Activity_hpp_
#define Activity_hpp_

namespace activities
{

class Activity
{

public:

        Activity( unsigned int business ) : activity( business ) { }
        operator unsigned int() const {  return activity ;  }

private:

        unsigned int activity ;

public: /* constants */

        static const unsigned int Waiting               =  0 ;
        static const unsigned int Blinking              =  8 ;

        static const unsigned int MovingNorth           = 24 ;
        static const unsigned int MovingSouth           = 25 ;
        static const unsigned int MovingEast            = 26 ;
        static const unsigned int MovingWest            = 27 ;

        static const unsigned int MovingNortheast       = 32 ;
        static const unsigned int MovingSoutheast       = 33 ;
        static const unsigned int MovingSouthwest       = 34 ;
        static const unsigned int MovingNorthwest       = 35 ;

        static const unsigned int AutomovingNorth       = 41 ;
        static const unsigned int AutomovingSouth       = 42 ;
        static const unsigned int AutomovingEast        = 43 ;
        static const unsigned int AutomovingWest        = 44 ;

        static const unsigned int Jumping               = 80 ;
        static const unsigned int Falling               = 90 ;
        static const unsigned int Gliding               = 98 ;

        static const unsigned int TakingItem                    = 100 ;
        static const unsigned int DroppingItem                  = 105 ;
        static const unsigned int TakeAndJump                   = 110 ;
        static const unsigned int DropAndJump                   = 115 ;

        static const unsigned int PushedNorth                   = 124 ;
        static const unsigned int PushedSouth                   = 125 ;
        static const unsigned int PushedEast                    = 126 ;
        static const unsigned int PushedWest                    = 127 ;

        static const unsigned int PushedNortheast               = 132 ;
        static const unsigned int PushedSoutheast               = 133 ;
        static const unsigned int PushedSouthwest               = 134 ;
        static const unsigned int PushedNorthwest               = 135 ;

        static const unsigned int PushedUp                      = 150 ;
        /* static const unsigned int PushedDown = 160 ; */

        static const unsigned int DraggedNorth                  = 181 ;
        static const unsigned int DraggedSouth                  = 182 ;
        static const unsigned int DraggedEast                   = 183 ;
        static const unsigned int DraggedWest                   = 184 ;

        static const unsigned int CancelDragging                = 190 ;

        static const unsigned int BeginTeletransportation       = 222 ;
        static const unsigned int EndTeletransportation         = 234 ;

        static const unsigned int Freeze = 271 ;
        static const unsigned int WakeUp = 272 ;

        static const unsigned int MetLethalItem                 = 290 ;
        static const unsigned int Vanishing                     = 299 ;

        static const unsigned int Collision = 330 ;
        static const unsigned int Function = 360 ;

        static const unsigned int Mistake = 599 ;

} ;

}

#endif

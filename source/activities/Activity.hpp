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

        static const unsigned int Waiting       = 0 ;
        static const unsigned int Blinking      = 8 ;

        static const unsigned int MoveNorth = 24 ;
        static const unsigned int MoveSouth = 25 ;
        static const unsigned int MoveEast = 26 ;
        static const unsigned int MoveWest = 27 ;
        static const unsigned int MoveNortheast = 32 ;
        static const unsigned int MoveSoutheast = 33 ;
        static const unsigned int MoveSouthwest = 34 ;
        static const unsigned int MoveNorthwest = 35 ;
        static const unsigned int MoveDown = 60 ;
        static const unsigned int MoveUp = 70 ;
        static const unsigned int AutoMoveNorth = 81 ;
        static const unsigned int AutoMoveSouth = 82 ;
        static const unsigned int AutoMoveEast = 83 ;
        static const unsigned int AutoMoveWest = 84 ;

        static const unsigned int Jump = 90 ;
        static const unsigned int Fall = 95 ;
        static const unsigned int Glide = 98 ;
        static const unsigned int TakeItem = 100 ;
        static const unsigned int ItemTaken = 101 ;
        static const unsigned int DropItem = 102 ;
        static const unsigned int TakeAndJump = 115 ;
        static const unsigned int DropAndJump = 116 ;

        static const unsigned int PushedNorth = 124 ;
        static const unsigned int PushedSouth = 125 ;
        static const unsigned int PushedEast = 126 ;
        static const unsigned int PushedWest = 127 ;
        static const unsigned int PushedNortheast = 132 ;
        static const unsigned int PushedSoutheast = 133 ;
        static const unsigned int PushedSouthwest = 134 ;
        static const unsigned int PushedNorthwest = 135 ;
        static const unsigned int PushedUp = 150 ;
        /* static const unsigned int PushedDown = 160 ; */

        static const unsigned int DraggedNorth = 181 ;
        static const unsigned int DraggedSouth = 182 ;
        static const unsigned int DraggedEast = 183 ;
        static const unsigned int DraggedWest = 184 ;
        static const unsigned int CancelDragNorth = 191 ;
        static const unsigned int CancelDragSouth = 192 ;
        static const unsigned int CancelDragEast = 193 ;
        static const unsigned int CancelDragWest = 194 ;

        static const unsigned int BeginTeletransportation = 222 ;
        static const unsigned int EndTeletransportation = 223 ;

        static const unsigned int Freeze = 281 ;
        static const unsigned int WakeUp = 282 ;
        static const unsigned int StopAtTop = 287 ;
        static const unsigned int StopAtBottom = 288 ;
        static const unsigned int MeetMortalItem = 290 ;
        static const unsigned int Vanish = 299 ;
        static const unsigned int Collision = 330 ;
        static const unsigned int Function = 360 ;

        static const unsigned int Mistake = 599 ;

} ;

}

#endif

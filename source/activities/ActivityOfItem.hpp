// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef ActivityOfItem_hpp_
#define ActivityOfItem_hpp_

namespace activities
{

class ActivityOfItem
{

public:

        ActivityOfItem( unsigned int business ) : activity( business ) { }
        operator unsigned int() const {  return activity ;  }

private:

        unsigned int activity ;

} ;

class Activity /* only constants here */
{

public:

        static const unsigned int Wait = 0 ;
        static const unsigned int Blink = 8 ;

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

        static const unsigned int DisplaceNorth = 124 ;
        static const unsigned int DisplaceSouth = 125 ;
        static const unsigned int DisplaceEast = 126 ;
        static const unsigned int DisplaceWest = 127 ;
        static const unsigned int DisplaceNortheast = 132 ;
        static const unsigned int DisplaceSoutheast = 133 ;
        static const unsigned int DisplaceSouthwest = 134 ;
        static const unsigned int DisplaceNorthwest = 135 ;
        static const unsigned int DisplaceUp = 160 ;
        static const unsigned int DisplaceDown = 170 ;

        static const unsigned int ForcePushNorth = 181 ;
        static const unsigned int ForcePushSouth = 182 ;
        static const unsigned int ForcePushEast = 183 ;
        static const unsigned int ForcePushWest = 184 ;
        static const unsigned int CancelPushingNorth = 191 ;
        static const unsigned int CancelPushingSouth = 192 ;
        static const unsigned int CancelPushingEast = 193 ;
        static const unsigned int CancelPushingWest = 194 ;

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

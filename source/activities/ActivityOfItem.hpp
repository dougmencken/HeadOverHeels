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
        static const unsigned int Push = 1 ;
        static const unsigned int Move = 2 ;
        static const unsigned int MoveNorth = 64 ;
        static const unsigned int MoveSouth = 65 ;
        static const unsigned int MoveEast = 66 ;
        static const unsigned int MoveWest = 67 ;
        static const unsigned int MoveNortheast = 68 ;
        static const unsigned int MoveSoutheast = 69 ;
        static const unsigned int MoveSouthwest = 70 ;
        static const unsigned int MoveNorthwest = 71 ;
        static const unsigned int MoveUp = 72 ;
        static const unsigned int MoveDown = 73 ;
        static const unsigned int Blink = 88 ;
        static const unsigned int Jump = 90 ;
        static const unsigned int RegularJump = 91 ;
        static const unsigned int HighJump = 92 ;
        static const unsigned int Fall = 95 ;
        static const unsigned int Glide = 98 ;
        static const unsigned int TakeItem = 100 ;
        static const unsigned int ItemTaken = 101 ;
        static const unsigned int DropItem = 102 ;
        static const unsigned int TakeAndJump = 105 ;
        static const unsigned int DropAndJump = 106 ;
        static const unsigned int DisplaceNorth = 120 ;
        static const unsigned int DisplaceSouth = 121 ;
        static const unsigned int DisplaceEast = 122 ;
        static const unsigned int DisplaceWest = 123 ;
        static const unsigned int DisplaceNortheast = 124 ;
        static const unsigned int DisplaceSoutheast = 125 ;
        static const unsigned int DisplaceSouthwest = 126 ;
        static const unsigned int DisplaceNorthwest = 127 ;
        static const unsigned int DisplaceUp = 128 ;
        static const unsigned int DisplaceDown = 129 ;
        static const unsigned int BeginWayOutTeletransport = 140 ;
        static const unsigned int WayOutTeletransport = 141 ;
        static const unsigned int BeginWayInTeletransport = 145 ;
        static const unsigned int WayInTeletransport = 146 ;
        static const unsigned int AutoMove = 150 ;
        static const unsigned int AutoMoveNorth = 151 ;
        static const unsigned int AutoMoveSouth = 152 ;
        static const unsigned int AutoMoveEast = 153 ;
        static const unsigned int AutoMoveWest = 154 ;
        static const unsigned int ForceDisplace = 160 ;
        static const unsigned int ForceDisplaceNorth = 161 ;
        static const unsigned int ForceDisplaceSouth = 162 ;
        static const unsigned int ForceDisplaceEast = 163 ;
        static const unsigned int ForceDisplaceWest = 164 ;
        static const unsigned int CancelDisplaceNorth = 171 ;
        static const unsigned int CancelDisplaceSouth = 172 ;
        static const unsigned int CancelDisplaceEast = 173 ;
        static const unsigned int CancelDisplaceWest = 174 ;
        static const unsigned int Freeze = 180 ;
        static const unsigned int WakeUp = 181 ;
        static const unsigned int StopAtTop = 182 ;
        static const unsigned int StopAtBottom = 183 ;
        static const unsigned int MeetMortalItem = 190 ;
        static const unsigned int Vanish = 191 ;
        static const unsigned int FireDoughnut = 192 ;
        static const unsigned int Rebound = 200 ;
        static const unsigned int SwitchIt = 210 ;
        static const unsigned int Collision = 220 ;
        static const unsigned int IsActive = 230 ;
        static const unsigned int Mistake = 399 ;

} ;

}

#endif

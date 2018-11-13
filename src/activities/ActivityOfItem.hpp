// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef ActivityOfItem_hpp_
#define ActivityOfItem_hpp_

namespace iso
{

class ActivityOfItem
{

public:

        ActivityOfItem( unsigned int business ) : activity( business ) { }
        operator unsigned int() const {  return activity ;  }

private:

        unsigned int activity ;

} ;

namespace Activity
{

        const unsigned int Wait = 0 ;
        const unsigned int Push = 1 ;
        const unsigned int Move = 2 ;
        const unsigned int MoveNorth = 64 ;
        const unsigned int MoveSouth = 65 ;
        const unsigned int MoveEast = 66 ;
        const unsigned int MoveWest = 67 ;
        const unsigned int MoveNortheast = 68 ;
        const unsigned int MoveSoutheast = 69 ;
        const unsigned int MoveSouthwest = 70 ;
        const unsigned int MoveNorthwest = 71 ;
        const unsigned int MoveUp = 72 ;
        const unsigned int MoveDown = 73 ;
        const unsigned int Blink = 88 ;
        const unsigned int Jump = 90 ;
        const unsigned int RegularJump = 91 ;
        const unsigned int HighJump = 92 ;
        const unsigned int Fall = 95 ;
        const unsigned int Glide = 98 ;
        const unsigned int TakeItem = 100 ;
        const unsigned int ItemTaken = 101 ;
        const unsigned int DropItem = 102 ;
        const unsigned int TakeAndJump = 105 ;
        const unsigned int DropAndJump = 106 ;
        const unsigned int DisplaceNorth = 120 ;
        const unsigned int DisplaceSouth = 121 ;
        const unsigned int DisplaceEast = 122 ;
        const unsigned int DisplaceWest = 123 ;
        const unsigned int DisplaceNortheast = 124 ;
        const unsigned int DisplaceSoutheast = 125 ;
        const unsigned int DisplaceSouthwest = 126 ;
        const unsigned int DisplaceNorthwest = 127 ;
        const unsigned int DisplaceUp = 128 ;
        const unsigned int DisplaceDown = 129 ;
        const unsigned int BeginWayOutTeletransport = 140 ;
        const unsigned int WayOutTeletransport = 141 ;
        const unsigned int BeginWayInTeletransport = 145 ;
        const unsigned int WayInTeletransport = 146 ;
        const unsigned int AutoMove = 150 ;
        const unsigned int AutoMoveNorth = 151 ;
        const unsigned int AutoMoveSouth = 152 ;
        const unsigned int AutoMoveEast = 153 ;
        const unsigned int AutoMoveWest = 154 ;
        const unsigned int ForceDisplace = 160 ;
        const unsigned int ForceDisplaceNorth = 161 ;
        const unsigned int ForceDisplaceSouth = 162 ;
        const unsigned int ForceDisplaceEast = 163 ;
        const unsigned int ForceDisplaceWest = 164 ;
        const unsigned int CancelDisplaceNorth = 171 ;
        const unsigned int CancelDisplaceSouth = 172 ;
        const unsigned int CancelDisplaceEast = 173 ;
        const unsigned int CancelDisplaceWest = 174 ;
        const unsigned int Freeze = 180 ;
        const unsigned int WakeUp = 181 ;
        const unsigned int StopAtTop = 182 ;
        const unsigned int StopAtBottom = 183 ;
        const unsigned int MeetMortalItem = 190 ;
        const unsigned int Vanish = 191 ;
        const unsigned int FireDoughnut = 192 ;
        const unsigned int Rebound = 200 ;
        const unsigned int SwitchIt = 210 ;
        const unsigned int Collision = 220 ;
        const unsigned int IsActive = 230 ;
        const unsigned int Mistake = 399 ;

}

}

#endif

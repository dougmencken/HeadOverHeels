// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef UserControlled_hpp_
#define UserControlled_hpp_

#include <vector>
#include <map>

#include "Behavior.hpp"
#include "PlayerItem.hpp"
#include "Timer.hpp"


namespace iso
{

/**
 * Models behavior of character, that is item controlled by user. Defines attributes
 * and actions which every character may do
 */

class UserControlled : public Behavior
{

public:

        UserControlled( const ItemPtr & item, const std::string & behavior ) ;

        virtual ~UserControlled( ) ;

       /**
        * Updates behavior of the element in each cycle
        * @return false if the element is supposed to die since this update, true otherwise
        */
        virtual bool update () = 0 ;

       /**
        * Updates player’s behavior from commands given by the user
        */
        virtual void behave () = 0 ;

protected:

       /**
        * Character waits, game shows first frame of character’s animation for current orientation
        */
        virtual void wait( PlayerItem & player ) ;

       /**
        * Character moves at speed specified in item’s data, in the direction of north or south or west or east
        */
        virtual void move( PlayerItem & player ) ;

       /**
        * Character moves automatically
        * at distance equal to number of automatic steps,
        * in direction of north or south or west or east,
        * at speed specified in item’s data
        */
        virtual void autoMove( PlayerItem & player ) ;

       /**
        * Move player at speed of item that pushes it in one of eight directions
        */
        virtual void displace( PlayerItem & player ) ;

       /**
        * Cancels movement by moving in the opposite direction of displacing, leaving item stopped
        * at current point. Used when character is dragged by conveyor belt or some similar item
        */
        virtual void cancelDisplace( PlayerItem & player ) ;

       /**
        * Character falls down at speed from item’s data
        */
        virtual void fall( PlayerItem & player ) ;

       /**
        * Character jumps, details of jumping are in subclasses
        */
        virtual void jump( PlayerItem & player ) ;

       /**
        * Character in air, falling or has jumped, glides at speed from item’s data
        * in direction of north or south or west or east
        */
        virtual void glide( PlayerItem & player ) ;

       /**
        * Character teleports from another room
        */
        virtual void wayInTeletransport( PlayerItem & player ) ;

       /**
        * Character teleports to another room
        */
        virtual void wayOutTeletransport( PlayerItem & player ) ;

       /**
        * Character collides with a mortal item
        */
        virtual void collideWithMortalItem( PlayerItem & player ) ;

       /**
        * Character releases something that freezes moving items
        */
        virtual void useHooter( PlayerItem & player ) ;

       /**
        * Take item underneath player
        */
        virtual void takeItem( PlayerItem & player ) ;

       /**
        * Drop item just below player
        */
        virtual void dropItem( PlayerItem & player ) ;

protected:

       /**
        * Defines player’s jump by pairs of horizontal and vertical offsets on each cycle
        */
        std::vector < std::pair< int /* xy */, int /* z */ > > jumpVector ;

       /**
        * Defines long jump of player from trampoline or with bunny
        */
        std::vector < std::pair< int /* xy */, int /* z */ > > highJumpVector ;

        unsigned int jumpPhase ;

       /**
        * Number of phases for normal jump
        */
        unsigned int jumpPhases ;

       /**
        * Number of phases for long jump
        */
        unsigned int highJumpPhases ;

       /**
        * Number of steps to take when character moves automatically
        */
        const unsigned int automaticSteps ;

       /**
        * Number of steps that remains after entering room through door
        */
        unsigned int automaticStepsThruDoor ;

       /**
        * Number of steps at double speed
        */
        unsigned int highSpeedSteps ;

       /**
        * Number of steps for character with immunity
        */
        unsigned int shieldSteps ;

       /**
        * Frames of falling, one for each of north south west east
        */
        std::map < std::string, unsigned int > fallFrames ;

       /**
        * Is there a fire from hooter in room
        */
        bool donutFromHooterIsHere ;

        std::string labelOfBubbles ;

        std::string labelOfFiredDoughnut ;

       /**
        * Timer for speed of movement
        */
        autouniqueptr < Timer > speedTimer ;

       /**
        * Timer for speed of falling
        */
        autouniqueptr < Timer > fallTimer ;

       /**
        * Timer for speed of gliding
        */
        autouniqueptr < Timer > glideTimer ;

       /**
        * Timer for blinking
        */
        autouniqueptr < Timer > timerForBlinking ;

public:

        void setFireFromHooter ( bool isHere ) {  this->donutFromHooterIsHere = isHere ;  }

};

}

#endif

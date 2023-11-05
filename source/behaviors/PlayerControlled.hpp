// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef PlayerControlled_hpp_
#define PlayerControlled_hpp_

#include <vector>
#include <map>

#include "Behavior.hpp"
#include "Timer.hpp"

class AvatarItem ;


namespace behaviors
{

/**
 * Models the behavior of a character controlled by the player
 */

class PlayerControlled : public Behavior
{

public:

        PlayerControlled( const ItemPtr & item, const std::string & behavior ) ;

        virtual ~PlayerControlled( ) ;

       /**
        * Updates the character's behavior by the user controls
        */
        virtual void behave () = 0 ;

protected:

       /**
        * Character waits, game shows first frame of character’s animation for current orientation
        */
        virtual void wait( AvatarItem & character ) ;

       /**
        * Character moves at speed specified in item’s data, in the direction of north or south or west or east
        */
        virtual void move( AvatarItem & character ) ;

       /**
        * Character moves automatically
        * at distance equal to number of automatic steps,
        * in direction of north or south or west or east,
        * at speed specified in item’s data
        */
        virtual void autoMove( AvatarItem & character ) ;

       /**
        * Move the character at the speed of an item that pushes it in one of eight directions
        */
        virtual void displace( AvatarItem & character ) ;

        /**
         * Cancels the existing displace by moving in the opposite direction, leaving the item
         * stopped at the current point. Used when the character is dragged by a conveyor
         */
        virtual void cancelDisplace( AvatarItem & character ) ;

       /**
        * Character falls down at speed from item’s data
        */
        virtual void fall( AvatarItem & character ) ;

       /**
        * Character jumps, details of jumping are in subclasses
        */
        virtual void jump( AvatarItem & character ) ;

       /**
        * Character in air, falling or has jumped, glides at speed from item’s data
        * in direction of north or south or west or east
        */
        virtual void glide( AvatarItem & character ) ;

        /**
         * Character teleports to another room
         */
        virtual void enterTeletransport( AvatarItem & character ) ;
        virtual void exitTeletransport( AvatarItem & character ) ;

       /**
        * Character collides with a mortal item
        */
        virtual void collideWithMortalItem( AvatarItem & character ) ;

       /**
        * Character releases something that freezes moving items
        */
        virtual void useHooter( AvatarItem & character ) ;

       /**
        * Take item underneath character
        */
        virtual void takeItem( AvatarItem & character ) ;

       /**
        * Drop item just below character
        */
        virtual void dropItem( AvatarItem & character ) ;

protected:

       /**
        * Defines the character’s jump by pairs of horizontal and vertical offsets on each cycle
        */
        std::vector < std::pair< int /* xy */, int /* z */ > > jumpVector ;

       /**
        * Defines the long jump of character from a trampoline or with a bunny's bonus
        */
        std::vector < std::pair< int /* xy */, int /* z */ > > highJumpVector ;

        bool isLosingLife ;

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

        std::string kindOfBubbles ;

        std::string kindOfFiredDoughnut ;

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

} ;

}

#endif

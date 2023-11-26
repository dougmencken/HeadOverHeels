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
         * Move the character at the speed of an item that pushes it
         */
        virtual void displace( AvatarItem & character ) ;

        /**
         * Move in the opposite direction of pushing, leaving the character at the current place.
         * It happens when the moving character is dragged by a conveyor
         */
        virtual void cancelDragging( AvatarItem & character ) ;

        /**
         * The character is falling down
         */
        virtual void fall( AvatarItem & character ) ;

        /**
         * The character jumps
         */
        virtual void jump( AvatarItem & character ) ;

        /**
         * The character glides in the air when falling or jumping
         */
        virtual void glide( AvatarItem & character ) ;

        /**
         * The character teleports to another room
         */
        virtual void enterTeletransport( AvatarItem & character ) ;
        virtual void exitTeletransport( AvatarItem & character ) ;

        /**
         * The character collides with a mortal item
         */
        virtual void collideWithMortalItem( AvatarItem & character ) ;

        /**
         * The character releases something that can freeze another item
         */
        virtual void useHooter( AvatarItem & character ) ;

        /**
         * Take an item below the character
         */
        virtual void takeItem( AvatarItem & character ) ;

        /**
         * Drop a taken item just under the character
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

        int jumpPhase ;

        /**
         * true when on a trampoline or has a bonus high jump
         */
        bool highJump ;

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

        std::string kindOfBubbles ;

        std::string kindOfFiredDoughnut ;

        /**
         * Timer for the speed of movement
         */
        autouniqueptr < Timer > speedTimer ;

        /**
         * Timer for the speed of falling
         */
        autouniqueptr < Timer > fallTimer ;

        /**
         * Timer for the speed of gliding
         */
        autouniqueptr < Timer > glideTimer ;

        /**
         * Timer for blinking
         */
        autouniqueptr < Timer > timerForBlinking ;

public:

        void unsetDoughnutInRoom () {  this->donutFromHooterInRoom = false ;  }

private:

        /**
         * is there a doughnut from the hooter in the room?
         */
        bool donutFromHooterInRoom ;

} ;

}

#endif

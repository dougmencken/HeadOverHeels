// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
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
 * Models the behavior of a character whose actions are controlled by the player
 */

class PlayerControlled : public Behavior
{

public:

        PlayerControlled( const ItemPtr & item, const std::string & behavior ) ;

        virtual ~PlayerControlled( ) ;

        /**
         * Updates the character’s behavior according to the player’s controls
         */
        virtual void behave () = 0 ;

        virtual void setCurrentActivity ( const Activity & newActivity ) ;

        virtual void changeActivityDueTo ( const Activity & newActivity, const ItemPtr & dueTo ) ;

        bool isInvulnerableToLethalItems () const ;

protected:

        virtual void wait( AvatarItem & character ) ;

        virtual void move( AvatarItem & character ) ;

        virtual void automove( AvatarItem & character ) ;

        /**
         * The character is moved by another item at the speed of the pusher
         */
        virtual void displace( AvatarItem & character ) ;

        /**
         * The character moves while being dragged by a conveyor, moving in the opposite way cancels dragging
         */
        virtual void handleMoveKeyWhenDragged () ;

        /**
         * Moving in the opposite direction of dragging leaves the character at the current place.
         * Happens when a moving character is dragged by a conveyor
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
         * The character collides with a lethal item
         */
        virtual void collideWithALethalItem( AvatarItem & character ) ;

        /**
         * The character releases a doughnut that can freeze another item
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

        // defines the character’s jump via the pairs of horizontal and vertical offsets on each cycle
        std::vector < std::pair< int /* xy */, int /* z */ > > jumpVector ;

        // defines the long jump from a spring or with a bunny’s bonus
        std::vector < std::pair< int /* xy */, int /* z */ > > highJumpVector ;

        int jumpPhase ;

        // true when on a spring stool or has a bonus high jump
        bool highJump ;

        // pasos automáticos
        // the number of steps to take when character moves automatically after entering a room through door
        static const unsigned int automatic_steps = 16 ;

        // how many automove steps remained
        unsigned int automoveStepsRemained ;

        // the number of steps at the double speed
        unsigned int quickSteps ;

        // the frames of falling, one for each of north south west east
        std::map < std::string, unsigned int > fallFrames ;

        std::string kindOfBubbles ;

        std::string kindOfFiredDoughnut ;

        // timer for the motion speed
        autouniqueptr < Timer > speedTimer ;

        // timer for the speed of falling
        autouniqueptr < Timer > fallTimer ;

        // timer for the speed of gliding
        autouniqueptr < Timer > glideTimer ;

        // timer for blinking
        autouniqueptr < Timer > timerForBlinking ;

public:

        void unsetDoughnutInRoom () {  this->donutFromHooterInRoom = false ;  }

private:

        // is there a doughnut from the hooter in the room?
        bool donutFromHooterInRoom ;

        bool isLosingLife ;

} ;

}

#endif

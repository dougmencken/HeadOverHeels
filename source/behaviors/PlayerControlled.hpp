// The free and open source remake of Head over Heels
//
// Copyright © 2025 Douglas Mencken dougmencken@gmail.com
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

#ifdef DEBUG
#  define DEBUG_ACTIVITIES      1
#  define DEBUG_WAITING         0
#endif


namespace behaviors
{

/**
 * Models the behavior of a character whose actions are controlled by the player
 */

/* abstract */
class PlayerControlled : public Behavior
{

public:

        PlayerControlled( AvatarItem & item, const std::string & behavior ) ;

        virtual ~PlayerControlled( ) ;

        virtual bool update () ;

        /**
         * Updates the character’s behavior according to the player’s controls
         */
        virtual void behave () = 0 ;

        virtual void setCurrentActivity ( const Activity & newActivity, const Motion2D & velocity ) ;

        virtual void changeActivityDueTo ( const Activity & newActivity, const AbstractItemPtr & dueTo )
        {
                changeActivityDueTo( newActivity, Motion2D::rest(), dueTo );
        }

        virtual void changeActivityDueTo ( const Activity & newActivity, const Motion2D & velocity, const AbstractItemPtr & dueTo ) ;

        bool isInvulnerableToLethalItems () const ;

protected:

        virtual void wait () ;

        virtual void move () ;

        virtual void automove () ;

        /**
         * Sets the current activity according to the typed movement key
         * @return true if any movement key is typed or false if not
         */
        virtual bool moveKeySetsActivity () ;

        /*
         * Changes the item’s heading according to the typed movement key
         * without affecting the activity. Used when a character is gliding
         * @return true if any movement key is typed, false otherwise
         */
        virtual bool moveKeyChangesHeading () ;

        /**
         * The character is moved by another item at the speed of the pusher
         */
        virtual void displace () ;

        /**
         * The character moves while being dragged by a conveyor, moving in the opposite way cancels dragging
         */
        virtual void handleMoveKeyWhenDragged () ;

        /**
         * The character is falling down
         */
        virtual void fall () ;

        /**
         * The character glides in the air when falling or jumping
         */
        virtual void glide () ;

        /**
         * Jump or teleport if the device is below, since
         * the key for teleporting is the same as for jumping
         */
        virtual void toJumpOrTeleport () ;

        /**
         * The character jumps
         */
        virtual void jump () ;

        /**
         * The character teleports to another room
         */
        virtual void enterTeletransport () ;
        virtual void exitTeletransport () ;

        /**
         * The character collides with a lethal item
         */
        virtual void collideWithALethalItem () ;

        /**
         * The character releases a doughnut that can freeze another item
         */
        virtual void useHooter () ;

        /**
         * Take an item below the character
         */
        virtual void takeItem () ;

        /**
         * Drop a taken item just under the character
         */
        virtual void dropItem () ;

protected:

        // defines the character’s jump via the pairs of horizontal and vertical offsets on each cycle
        std::vector < std::pair< int /* xy */, int /* z */ > > jumpVector ;

        // defines the big jump from a spring stool or with a bonus
        std::vector < std::pair< int /* xy */, int /* z */ > > bigJumpVector ;

        int jumpPhase ;

        // true when on a spring stool or has a bonus big jump
        bool bigJump ;

        // pasos automáticos
        // the number of steps to take when character moves automatically after entering a room through door
        static const unsigned int automatic_steps = /* Room::Single_Tile_Size */ 16 ;

        // how many automove steps remained
        unsigned int automoveStepsRemained ;

        // the number of steps at the double speed
        unsigned int quickSteps ;

        // the frames of falling, one for each of north south west east
        std::map < std::string, unsigned int > fallFrames ;

        // the frames of blinking, may be empty if the character doesn’t blink
        std::map < std::string, unsigned int > blinkFrames ;

        // timer for the motion speed
        autouniqueptr < Timer > speedTimer ;

        // timer for the speed of falling
        autouniqueptr < Timer > fallTimer ;

        // timer for the speed of gliding
        autouniqueptr < Timer > glideTimer ;

        // timer for blinking
        autouniqueptr < Timer > timerForBlinking ;

private:

        bool isLosingLife ;

} ;

}

#endif

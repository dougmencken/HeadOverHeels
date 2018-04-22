// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef UserControlled_hpp_
#define UserControlled_hpp_

#include <vector>

#include "Behavior.hpp"
#include "Timer.hpp"


namespace isomot
{

class Item ;
class ItemData ;
class ItemDataManager ;
class PlayerItem ;


/**
 * Models behavior of character, that is item controlled by user. Defines attributes
 * and actions which every character may do
 */

class UserControlled : public Behavior
{

public:

        UserControlled( Item * item, const std::string & behavior ) ;

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

       /**
        * Changes current activity of item’s behavior
        */
        virtual void changeActivityOfItem ( const ActivityOfItem& activityOf, Item* sender = 0 ) ;

protected:

        static const size_t howManyBlinkFrames = 4 ;

       /**
        * Character waits, game shows first frame of character’s animation for current orientation
        */
        virtual void wait( PlayerItem * player ) ;

       /**
        * Character moves at speed specified in item’s data, in the direction of north or south or west or east
        */
        virtual void move( PlayerItem * player ) ;

       /**
        * Character moves automatically
        * at distance equal to number of automatic steps,
        * in direction of north or south or west or east,
        * at speed specified in item’s data
        */
        virtual void autoMove( PlayerItem * player ) ;

       /**
        * Move player at speed of item that pushes it in one of eight directions
        */
        virtual void displace( PlayerItem * player ) ;

       /**
        * Cancels movement by moving in the opposite direction of displacing, leaving item stopped
        * at current point. Used when character is dragged by conveyor belt or some similar item
        */
        virtual void cancelDisplace( PlayerItem * player ) ;

       /**
        * Character falls down at speed from item’s data
        */
        virtual void fall( PlayerItem * player ) ;

       /**
        * Character jumps, details of jumping are in subclasses
        */
        virtual void jump( PlayerItem * player ) ;

       /**
        * Character in air, falling or has jumped, glides at speed from item’s data
        * in direction of north or south or west or east
        */
        virtual void glide( PlayerItem * player ) ;

       /**
        * Character teleports from another room
        */
        virtual void wayInTeletransport( PlayerItem * player ) ;

       /**
        * Character teleports to another room
        */
        virtual void wayOutTeletransport( PlayerItem * player ) ;

       /**
        * Character collides with a mortal item
        */
        virtual void collideWithMortalItem( PlayerItem * player ) ;

       /**
        * Character releases something that freezes moving items
        */
        virtual void useHooter( PlayerItem * player ) ;

       /**
        * Take item underneath player
        */
        virtual void takeItem( PlayerItem * player ) ;

       /**
        * Drop item just below player
        */
        virtual void dropItem( PlayerItem * player ) ;

protected:

       /**
        * Defines player’s jump by pairs of horizontal and vertical offsets on each cycle
        */
        std::vector < std::pair< int /* xy */, int /* z */ > > jumpVector ;

       /**
        * Defines long jump of player from trampoline or with bunny
        */
        std::vector < std::pair< int /* xy */, int /* z */ > > highJumpVector ;

       /**
        * Index of phase of jump
        */
        unsigned int jumpIndex ;

       /**
        * Number of phases for normal jump
        */
        unsigned int jumpFrames ;

       /**
        * Number of phases for long jump
        */
        unsigned int highJumpFrames ;

       /**
        * Number of steps to take when character moves automatically
        */
        unsigned int automaticSteps ;

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
        unsigned int fallFrames[ 4 ] ;

       /**
        * Blank frame, used during teleportation
        */
        unsigned int nullFrame ;

       /**
        * Is there a fire from hooter in room
        */
        bool fireFromHooterIsPresent ;

       /**
        * Item used as transition when changing room via teleport
        */
        std::string labelOfTransitionViaTeleport ;

       /**
        * Item used as fire from hooter
        */
        std::string labelOfFireFromHooter ;

        ItemDataManager* itemDataManager ;

       /**
        * Timer for speed of movement
        */
        Timer* speedTimer ;

       /**
        * Timer for speed of falling
        */
        Timer* fallTimer ;

       /**
        * Timer for speed of gliding
        */
        Timer* glideTimer ;

       /**
        * Timer for blinking
        */
        Timer* blinkingTimer ;

public:

       /**
        * Number of phases of jump
        */
        unsigned int getJumpFrames () const {  return jumpFrames ;  }

        void setFireFromHooter ( bool isHere ) {  this->fireFromHooterIsPresent = isHere ;  }

        void setMoreData ( void * data ) ;

};

}

#endif

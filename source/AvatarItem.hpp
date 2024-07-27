// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef AvatarItem_hpp_
#define AvatarItem_hpp_

#include <vector>

#include "FreeItem.hpp"
#include "DescriptionOfItem.hpp"
#include "Timer.hpp"
#include "Picture.hpp"


/**
 * An avatar is a game character controlled by the player
 */

class AvatarItem : public FreeItem
{

public:

        AvatarItem( const DescriptionOfItem & description, int x, int y, int z, const std::string & heading ) ;

        AvatarItem( const AvatarItem & toCopy ) ;

        virtual ~AvatarItem( ) { }

        virtual std::string whichItemClass () const {  return "avatar item" ;  }

        bool isHead () const {  return getOriginalKind() == "head" ;  }

        bool isHeels () const {  return getOriginalKind() == "heels" ;  }

        bool isHeadOverHeels () const {  return getOriginalKind() == "headoverheels" ;  }

        /**
         * Updates the character’s behavior according to the player’s controls
         */
        virtual void behaveCharacter () ;

        void wait () ;

        bool isActiveCharacter () const ;

        unsigned char getLives() const ;

        void addLives( unsigned char lives ) ;

        void loseLife () ;

        /**
         * the character takes a magic tool, one of "horn" or "handbag"
         */
        void takeMagicTool( const std::string & tool ) ;

        unsigned short getDonuts () const ;

        void addDonuts( unsigned short howMany ) ;

        /**
         * the character releases a doughnut
         */
        void useDoughnutHorn () ;

        unsigned short getQuickSteps () const ;

        void activateBonusQuickSteps () ;

        void decrementBonusQuickSteps () ;

        unsigned short getHighJumps () const ;

        void addBonusHighJumps ( unsigned char howMany ) ;

        void decrementBonusHighJumps () ;

        short getShieldPoints () const ;
        double getShieldSeconds () const ;

        void activateShield () ;
        void activateShieldForSeconds ( double seconds ) ;

        void decrementShieldOverTime () ;

        /**
         * Character just liberated a planet
         */
        void liberateCurrentPlanet() ;

        /**
         * Save game when the character meets the reincarnation fish
         */
        void saveGame () ;

        virtual bool addToPosition ( int x, int y, int z ) ;

        void placeItemInBag ( const std::string & kindOfItem, const std::string & behavior ) ;

        void emptyBag () ;

        const DescriptionOfItem * getDescriptionOfTakenItem () const {  return descriptionOfTakenItem ;  }

        const std::string & getBehaviorOfTakenItem () const {  return this->behaviorOfTakenItem ;  }

        /**
         * Metamorph into another kind of item, such as into bubbles when a character teleports
         */
        virtual void metamorphInto ( const std::string & newKind, const std::string & initiatedBy ) ;

        /**
         * Does this character have the temporary invulnerability or not
         */
        bool hasShield () const {  return getShieldPoints() > 0 ;  }

        /**
         * Does this character have the magic tool, the horn or the bag, or doesn’t
         */
        bool hasTool ( const std::string & tool ) const ;

        const std::string & getWayOfExit () const {  return this->wayOfExit ;  }

        void setWayOfExit ( const std::string & way ) ;

        const std::string & getWayOfEntry () const {  return this->wayOfEntry ;  }

        void setWayOfEntry ( const std::string & way ) ;

        bool isWalkingThroughDoorAt( const std::string & where ) ;

private:

        // set the behavior according to the kind of character
        void characterToBehaviour () ;

        void autoMoveOnEntry () ;

        /**
         * @param where the door mentioned by its position in the room
         * @return true if this item is *not* under that door or if no door there
         */
        bool isNotUnderDoorAt ( const std::string & where ) ;

        ////////bool isUnderSomeDoor () ;

        /**
         * The way by which the character leaves the room
         */
        std::string wayOfExit ;

        /**
         * How the character enters the room:
         * through a door, or via teleport, or going below the floor or above the ceiling
         */
        std::string wayOfEntry ;

        autouniqueptr < Timer > shieldTimer ;

        const DescriptionOfItem * descriptionOfTakenItem ;

        std::string behaviorOfTakenItem ;

} ;

typedef multiptr < AvatarItem > AvatarItemPtr ;

#endif

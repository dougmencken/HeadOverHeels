// The free and open source remake of Head over Heels
//
// Copyright © 2026 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Mediator_hpp_
#define Mediator_hpp_

#include <string>
#include <vector>
#include <deque>

#include <WrappersAllegro.hpp>

#include <pthread.h>

#include "Mediated.hpp"
#include "Room.hpp"


class FloorTile ;
class Door ;


/**
 * Intermediary for the coexistence and interaction of various items within the same room
 */

class Mediator
{

public:

        /**
         * @param room the room in which this mediator negotiates
         */
        Mediator( Room * whichRoom ) ;

        virtual ~Mediator( ) ;

        Room * getRoom () const {  return this->room ;  }

        /**
         * Update every item’s behavior
         */
        void update () ;

        /**
         * Begin updating items in a distinct thread
         */
        void beginUpdating () ;

        void endUpdating () ;

        /**
         * Decrease shield for an active player character by 'seconds'
         */
        void decreaseShieldForActiveCharacter ( double seconds ) ;

        void wantToMaskWithFreeItemImageAt ( const FreeItem & item, int x, int y ) ;

        void wantToMaskWithFreeItem ( const FreeItem & item ) ;

        void wantToMaskWithGridItemAt ( const GridItem & gridItem, int x, int y, int z, std::pair < int, int > offset ) ;

        void wantToMaskWithGridItem ( const GridItem & gridItem ) ;

        void wantShadowFromGridItem ( const GridItem & gridItem ) ;

        void wantShadowFromFreeItemAt ( const FreeItem & freeItem, int x, int y, int z ) ;

        void wantShadowFromFreeItem ( const FreeItem & freeItem ) ;

        void shadeFreeItemsBeneathItemAt ( const DescribedItem & item, int x, int y, int z ) ;

        void castShadowOnFloor ( FloorTile & floorTile ) ;

        void castShadowOnGridItem ( GridItem & gridItem ) ;

        void castShadowOnFreeItem ( FreeItem & freeItem ) ;

        void maskFreeItem ( FreeItem & freeItem ) ;

        /**
         * Look for the Z coordinate of the highest position in the column
         * to place the given item above
         * @return the value of Z or zero if can’t get it
         */
        int findHighestZ ( const DescribedItem & item ) ;

        /**
         * Look for an item in the room by its unique name
         */
        DescribedItemPtr findItemByUniqueName ( const std::string & uniqueName ) ;

        /**
         * Look for an item in the room by its kind
         * When there are several items of this kind, the first found one is returned
         */
        DescribedItemPtr findItemOfKind ( const std::string & kind ) ;

        /**
         * Look for an item in the room with the given behavior
         */
        DescribedItemPtr findItemBehavingAs ( const std::string & behavior ) ;

        /**
         * Collect collisions between the given item and other items in the active room
         * @return true if any collision is found, or false otherwise
         */
        bool collectCollisionsWith ( const std::string & uniqueNameOfItem ) ;

        void addCollisionWith ( const std::string & what ) ;

        bool isThereAnyCollision () {  return ! this->collisions.empty() ;  }

        unsigned int howManyCollisions () {  return this->collisions.size() ;  }

        void clearCollisions () {  this->collisions.clear() ;  }

        /**
         * @return the unique name of item, or empty string if there are no collisions
         */
        std::string popCollision () ;

        DescribedItemPtr findCollisionPop () {  return findItemByUniqueName( popCollision() ) ;  }

        /**
         * Is there a collision with some item of a given kind
         * @return the item with which collision is happened or nil if there’s no collision
         */
        DescribedItemPtr collisionWithSomeKindOf ( const std::string & kind ) ;

        /**
         * Is there a collision with an item of a given behavior
         * @return the item with which collision is happened or nil if there’s no collision
         */
        DescribedItemPtr collisionWithBehavingAs ( const std::string & behavior ) ;

        DescribedItemPtr collisionWithBadBoy () ;

        /**
         * Activate the next character in the room
         * @return true if it’s possible to swap or false if there’s only one character in this room
         */
        bool pickNextCharacter () ;

        /**
         * Toggle the switch if it is in room, most rooms don’t have a switch. If it turns on
         * then free items stop moving and volatile items are no longer volatile. If it goes off
         * then such items restore their original behavior
         */
        void toggleSwitchInRoom () ;

        void markToSortGridItems () {  this->needToSortGridItems = true ;  }

        void markToSortFreeItems () {  this->needToSortFreeItems = true ;  }

        bool isThreadRunning () const {  return threadRunning ;  }

        const AvatarItemPtr & getActiveCharacter () const {  return this->currentlyActiveCharacter ;  }

        std::string getNameOfActiveCharacter () const
        {
                return ( this->currentlyActiveCharacter != nilPointer )
                                ? this->currentlyActiveCharacter->getOriginalKind ()
                                : "" ;
        }

        /**
         * @return true if the character with the given name is found in the room
         *              and activated there or was already active
         */
        bool activateCharacterByName ( const std::string & name ) ;

        void deactivateAnyCharacter () {  this->currentlyActiveCharacter = AvatarItemPtr() ;  }

        /**
         * The waiting, or inactive, character
         * @return avatar item or nil if there’s no more character in this room
         */
        AvatarItemPtr getWaitingCharacter () const ;

        const std::string & getLastActiveCharacterBeforeJoining () const {  return lastActiveCharacterBeforeJoining ;  }

        void lockGridItemsMutex () {  pthread_mutex_lock( &gridItemsMutex ) ;  }
        void unlockGridItemsMutex () {  pthread_mutex_unlock( &gridItemsMutex ) ;  }

        void lockFreeItemsMutex () {  pthread_mutex_lock( &freeItemsMutex ) ;  }
        void unlockFreeItemsMutex () {  pthread_mutex_unlock( &freeItemsMutex ) ;  }

private:

        /**
         * The room where this mediator deals
         */
        Room * room ;

        // for updating of items
        pthread_t updateThread ;

        volatile bool threadRunning ;

        pthread_mutex_t gridItemsMutex ;

        pthread_mutex_t freeItemsMutex ;

        pthread_mutex_t collisionsMutex ;

        void lockCollisionsMutex () {  pthread_mutex_lock( & this->collisionsMutex ) ;  }
        void unlockCollisionsMutex () {  pthread_mutex_unlock( & this->collisionsMutex ) ;  }

        bool needToSortGridItems ;

        bool needToSortFreeItems ;

        bool switchInRoomIsOn ;

        /**
         * Items that can take one life from a character and that may be frozen by a donut or with a switch
         */
        std::vector < std::string > badBoys ;

        /**
         * The character yet controlled by the player
         */
        AvatarItemPtr currentlyActiveCharacter ;

        /**
         * The character which was active just before joining them both
         */
        std::string lastActiveCharacterBeforeJoining ;

        std::deque < std::string > collisions ;

        static void * updateFromThread ( void * mediatorAsVoid ) ;

};

#endif

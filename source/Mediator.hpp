// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Mediator_hpp_
#define Mediator_hpp_

#include <string>
#include <list>
#include <vector>
#include <deque>

#include <WrappersAllegro.hpp>

#include <pthread.h>
#include <sys/time.h>

#include "Mediated.hpp"
#include "Room.hpp"


class FloorTile ;
class Door ;


/**
 * Mediator for various items in one room. It collects requests sent by items at some events,
 * and forwards them to other items
 */

class Mediator
{

public:

       /**
        * @param room Room in which this mediator negotiates
        */
        Mediator( Room* room ) ;

        virtual ~Mediator( ) ;

        static void * updateThread ( void * mediatorAsVoid ) ;

       /**
        * Update behavior of every item for one cycle
        */
        void update () ;

       /**
        * Begin update of items in separate thread
        */
        void beginUpdate () ;

       /**
        * End update of items
        */
        void endUpdate () ;

        void wantToMaskWithFreeItemAt ( const FreeItem & item, std::pair < int, int > offset ) ;

        void wantToMaskWithFreeItem ( const FreeItem & item ) ;

        void wantToMaskWithGridItemAt ( const GridItem & gridItem, int x, int y, int z, std::pair < int, int > offset ) ;

        void wantToMaskWithGridItem ( const GridItem & gridItem ) ;

        void wantShadowFromGridItem ( const GridItem & gridItem ) ;

        void wantShadowFromFreeItemAt ( const FreeItem & freeItem, int x, int y, int z ) ;

        void wantShadowFromFreeItem ( const FreeItem & freeItem ) ;

        void shadeFreeItemsBeneathItemAt ( const Item & item, int x, int y, int z ) ;

        void castShadowOnFloor ( FloorTile & floorTile ) ;

        void castShadowOnGridItem ( GridItem & gridItem ) ;

        void castShadowOnFreeItem ( FreeItem & freeItem ) ;

        void maskFreeItem ( FreeItem & freeItem ) ;

       /**
        * Find item in room by its unique name
        */
        ItemPtr findItemByUniqueName ( const std::string & uniqueName ) ;

       /**
        * Look for an item in the room by its kind
        * When there are several items of this kind, the first found one is returned
        */
        ItemPtr findItemOfKind ( const std::string & kind ) ;

       /**
        * Look for an item in the room by its behavior
        */
        ItemPtr findItemByBehavior ( const std::string & behavior ) ;

       /**
        * Look for collisions between the given item and other items in room
        * @return true if collisions were found or false otherwise
        */
        bool lookForCollisionsOf ( const std::string & uniqueNameOfItem ) ;

       /**
        * Search for Z coordinate which is the highest position to place the given item
        * @return value of Z or zero if can’t get it
        */
        int findHighestZ ( const Item & item ) ;

        void pushCollision ( const std::string& uniqueName ) ;

       /**
        * @return unique name of item or empty string when stack is empty
        */
        std::string popCollision () ;

        void clearStackOfCollisions () {  collisions.clear() ;  }

        bool isStackOfCollisionsEmpty () {  return collisions.empty() ;  }

        unsigned int depthOfStackOfCollisions () {  return collisions.size() ;  }

        ItemPtr findCollisionPop () {  return findItemByUniqueName( popCollision() ) ;  }

       /**
        * Is there collision with item of a given kind
        * @return item with which collision is happened or nil if there’s no collision
        */
        ItemPtr collisionWithSomeKindOf ( const std::string & kind ) ;

       /**
        * Is there collision with item of a given behavior
        * @return item with which collision is happened or nil if there’s no collision
        */
        ItemPtr collisionWithBehavingAs ( const std::string & behavior ) ;

        ItemPtr collisionWithBadBoy () ;

       /**
        * Activate the next character in the room
        * @return true if it’s possible to swap or false if there’s only one character in this room
        */
        bool pickNextCharacter () ;

       /**
        * Toggle the switch if it is in room, most rooms do not have a switch. If it turns on
        * then free items stop moving and volatile items are no longer volatile. If it goes off
        * then such items restore their original behavior
        */
        void toggleSwitchInRoom () ;

        void markToSortGridItems () {  this->needGridItemsSorting = true ;  }

        void markToSortFreeItems () {  this->needFreeItemsSorting = true ;  }

        void lockGridItemsMutex () {  pthread_mutex_lock( &gridItemsMutex ) ;  }

        void lockFreeItemsMutex () {  pthread_mutex_lock( &freeItemsMutex ) ;  }

        void unlockGridItemsMutex () {  pthread_mutex_unlock( &gridItemsMutex ) ;  }

        void unlockFreeItemsMutex () {  pthread_mutex_unlock( &freeItemsMutex ) ;  }

        void lockCollisionsMutex () {  pthread_mutex_lock( &collisionsMutex ) ;  }

        void unlockCollisionsMutex () {  pthread_mutex_unlock( &collisionsMutex ) ;  }

private:

        /**
         * The room where this mediator deals
         */
        Room * room ;

        // for updating of items
        pthread_t thread ;

        volatile bool threadRunning ;

        pthread_mutex_t gridItemsMutex ;

        pthread_mutex_t freeItemsMutex ;

        pthread_mutex_t collisionsMutex ;

        bool needGridItemsSorting ;

        bool needFreeItemsSorting ;

        bool switchInRoomIsOn ;

        /**
         * Items that can take one life from a character and that may be frozen by a donut or with a switch
         */
        std::vector < std::string > badBoys ;

        /**
         * The character yet controlled by the player
         */
        AvatarItemPtr activeCharacter ;

        std::string nameOfActiveCharacter ;

        /**
         * The character which was active just before joining them both
         */
        std::string lastActiveCharacterBeforeJoining ;

        /**
         * The unique names of items collided in the room
         */
        std::deque < std::string > collisions ;

public:

        bool isThreadRunning () {  return threadRunning ;  }

        const std::string & getLastActiveCharacterBeforeJoining () {  return lastActiveCharacterBeforeJoining ;  }

        Room * getRoom () const {  return room ;  }

        AvatarItemPtr & getActiveCharacter () {  return activeCharacter ;  }

        const std::string & getNameOfActiveCharacter () const {  return nameOfActiveCharacter ;  }

        void setActiveCharacter ( const AvatarItemPtr & character ) ;

        /**
         * The waiting character
         * @return avatar item or nil if there’s no more character in this room
         */
        AvatarItemPtr getWaitingCharacter () const ;

};

#endif

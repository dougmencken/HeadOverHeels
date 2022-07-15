// The free and open source remake of Head over Heels
//
// Copyright © 2022 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
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

#include "Ism.hpp"
#include "Room.hpp"


namespace iso
{

class Mediated ;
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
        * Find item in room by its label
        * @param label Label of item, when there are several elements with this label return the first one found
        */
        ItemPtr findItemByLabel ( const std::string & label ) ;

       /**
        * Find item in room by its behavior
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
        * Is there collision with item of a given label
        * @return item with which collision happened or nil if there’s no collision
        */
        ItemPtr collisionWithByLabel ( const std::string& label ) ;

       /**
        * Is there collision with item of a given behavior
        * @return item with which collision happened or nil if there’s no collision
        */
        ItemPtr collisionWithByBehavior ( const std::string& behavior ) ;

        ItemPtr collisionWithBadBoy () ;

       /**
        * Activate next character in room
        * @return true if it’s possible to swap or false if there’s only one player in this room
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
        * Room where this mediator deals
        */
        Room * room ;

       /**
        * Thread for update of items
        */
        pthread_t thread ;

        volatile bool threadRunning ;

        pthread_mutex_t gridItemsMutex ;

        pthread_mutex_t freeItemsMutex ;

        pthread_mutex_t collisionsMutex ;

        bool needGridItemsSorting ;

        bool needFreeItemsSorting ;

        bool switchInRoomIsOn ;

       /**
        * Items that may take life from player and that may be freezed by doughnut or by switch
        */
        std::vector < std::string > badBoys ;

       /**
        * Character yet controlled by user
        */
        PlayerItemPtr activeCharacter ;

        std::string labelOfActiveCharacter ;

       /**
        * Character which was active just before joining them
        */
        std::string lastActiveCharacterBeforeJoining ;

       /**
        * Collisions happened in room, stores unique names of items
        */
        std::deque < std::string > collisions ;

public:

        bool isThreadRunning () {  return threadRunning ;  }

        const std::string & getLastActiveCharacterBeforeJoining () {  return lastActiveCharacterBeforeJoining ;  }

        Room * getRoom () const {  return room ;  }

        PlayerItemPtr & getActiveCharacter () {  return activeCharacter ;  }

        const std::string & getLabelOfActiveCharacter () const {  return labelOfActiveCharacter ;  }

        void setActiveCharacter ( const PlayerItemPtr & character ) ;

       /**
        * Waiting character
        * @return player item or nil if there’re no more players in this room
        */
        PlayerItemPtr getWaitingCharacter () const ;

};

}

#endif

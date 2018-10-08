// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
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

#include <WrappersAllegro.hpp>

#include <pthread.h>
#include <sys/time.h>

#include "Ism.hpp"
#include "Room.hpp"


namespace isomot
{

class Mediated ;
class Item ;
class FloorTile ;
class GridItem ;
class FreeItem ;
class PlayerItem ;
class Door ;
class ItemDataManager ;


void * updateThread ( void * thisClass ) ;


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

        void remaskWithFreeItem( FreeItem* item ) ;

        void remaskWithGridItem( GridItem* gridItem ) ;

        void reshadeWithGridItem( GridItem* gridItem ) ;

        void reshadeWithFreeItem( FreeItem* freeItem ) ;

        void castShadowOnFloor( FloorTile* floorTile ) ;

        void castShadowOnGridItem( GridItem* gridItem ) ;

        void castShadowOnFreeItem( FreeItem* freeItem ) ;

        void maskFreeItem ( FreeItem* freeItem ) ;

       /**
        * Find item in room by its unique name
        * @return item found or nil if no such item
        */
        Item * findItemByUniqueName ( const std::string& uniqueName ) ;

       /**
        * Find item in room by its label
        * @param label Label of item, when there are several elements with this label return the first one found
        * @return item found or nil if no such item
        */
        Item * findItemByLabel ( const std::string& label ) ;

       /**
        * Find item in room by its behavior
        */
        Item * findItemByBehavior ( const std::string& behavior ) ;

       /**
        * Look for collisions between the given item and other items in room
        * @return true if collisions were found or false otherwise
        */
        bool findCollisionWithItem ( Item* item ) ;

       /**
        * Search for Z coordinate which is the highest position to place the given item
        * @return value of Z or zero if can’t get it
        */
        int findHighestZ ( Item* item ) ;

        void addGridItemToList ( GridItem* gridItem ) ;

        void addFreeItemToList ( FreeItem* freeItem ) ;

        void removeGridItemFromList ( GridItem* gridItem ) ;

        void removeFreeItemFromList ( FreeItem* freeItem ) ;

        void pushCollision ( const std::string& uniqueName ) ;

       /**
        * @return unique name of item or empty string when stack is empty
        */
        std::string popCollision () ;

        void clearStackOfCollisions () {  collisions.clear() ;  }

        bool isStackOfCollisionsEmpty () {  return collisions.empty() ;  }

        unsigned int depthOfStackOfCollisions () {  return collisions.size() ;  }

        Item * findCollisionPop () {  return this->findItemByUniqueName( this->popCollision() ) ;  }

       /**
        * Is there collision with item of a given label
        * @return item with which collision happened or nil if there’s no collision
        */
        Item * collisionWithByLabel ( const std::string& label ) ;

       /**
        * Is there collision with item of a given behavior
        * @return item with which collision happened or nil if there’s no collision
        */
        Item * collisionWithByBehavior ( const std::string& behavior ) ;

        Item * collisionWithBadBoy () ;

       /**
        * Activate next character in room
        * @param itemDataManager Needed to create composite player from simple ones
        *                        or to create simple players from composite,
        *                        you may use nil when you’re sure that there’s no joining or splitting
        * @return true if it’s possible to swap or false if there’s only one player in this room
        */
        bool pickNextCharacter ( ItemDataManager* itemDataManager ) ;

       /**
        * Toggle the switch if it is in room, most rooms do not have a switch. If it turns on
        * then free items stop moving and volatile items are no longer volatile. If it goes off
        * then such items restore their original behavior
        */
        void toggleSwitchInRoom () ;

        void activateGridItemsSorting () {  this->gridItemsSorting = true ;  }

        void activateFreeItemsSorting () {  this->freeItemsSorting = true ;  }

        void lockGridItemMutex () {  pthread_mutex_lock( &gridItemsMutex ) ;  }

        void lockFreeItemMutex () {  pthread_mutex_lock( &freeItemsMutex ) ;  }

        void unlockGridItemMutex () {  pthread_mutex_unlock( &gridItemsMutex ) ;  }

        void unlockFreeItemMutex () {  pthread_mutex_unlock( &freeItemsMutex ) ;  }

protected:

        void shadeFreeItemsBeneathItem ( Item * item ) ;

private:

        static bool sortColumnOfGridItems ( GridItem * g1, GridItem * g2 ) ;

        static bool sortListOfFreeItems ( FreeItem * f1, FreeItem * f2 ) ;

private:

       /**
        * Room where this mediator deals
        */
        Room * room ;

       /**
        * Thread for update of items
        */
        pthread_t thread ;

        bool threadRunning ;

        pthread_mutex_t gridItemsMutex ;

        pthread_mutex_t freeItemsMutex ;

        bool gridItemsSorting ;

        bool freeItemsSorting ;

        bool switchInRoomIsOn ;

       /**
        * Set of grid items that form structure of room. Each column is list of grid items
        * sorted thus next item’s Z is greater than preceding item’s Z
        */
        std::vector < std::list < GridItem * > > gridItems ;

       /**
        * List of free items in room
        */
        std::list < FreeItem * > freeItems ;

       /**
        * Items that may take life from player and that may be freezed by doughnut or by switch
        */
        std::vector < std::string > badBoys ;

       /**
        * Character yet controlled by user
        */
        PlayerItem * activeCharacter ;

        std::string labelOfActiveCharacter ;

       /**
        * Character which was active just before joining them
        */
        std::string lastActiveCharacterBeforeJoining ;

       /**
        * Collisions happened in room, stores unique names of items
        */
        std::vector < std::string > collisions ;

public:

        bool isThreadRunning () {  return threadRunning ;  }

        const std::string& getLastActiveCharacterBeforeJoining () {  return lastActiveCharacterBeforeJoining ;  }

        Room* getRoom () const {  return room ;  }

        const std::vector < std::list < GridItem * > > & getGridItems () const {  return gridItems ;  }

        const std::list < FreeItem * > & getFreeItems () const {  return freeItems ;  }

        PlayerItem * getActiveCharacter () const {  return this->activeCharacter ;  }

        const std::string& getLabelOfActiveCharacter () const {  return labelOfActiveCharacter ;  }

        void setActiveCharacter ( PlayerItem * character ) ;

       /**
        * Waiting character
        * @return player item or nil if there’re no more players in this room
        */
        PlayerItem * getWaitingCharacter () const ;

};


class EqualUniqueNameOfItem : public std::binary_function< Item *, std::string, bool >
{

public:
        bool operator() ( Item* item, const std::string& uniqueName ) const ;

};

class EqualLabelOfItem : public std::binary_function< Item *, std::string, bool >
{

public:
        bool operator() ( Item* item, const std::string& label ) const ;

};

class EqualBehaviorOfItem : public std::binary_function< Item *, std::string, bool >
{

public:
        bool operator() ( Item* item, const std::string& behavior ) const ;

};

}

#endif

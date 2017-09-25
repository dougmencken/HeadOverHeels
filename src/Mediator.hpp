// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Mediator_hpp_
#define Mediator_hpp_

#include <string>
#include <algorithm>
#include <list>
#include <vector>
#include <stack>
#include <utility>
#include <functional>
#include <allegro.h>
#ifdef __WIN32
  #include <winalleg.h>
#endif
#include <pthread.h>
#include <sys/time.h>

#include "Ism.hpp"

#include "Room.hpp"

#include "TableOfTransparencies.hpp"


namespace isomot
{

class Mediated;
class Item;
class FloorTile;
class GridItem;
class FreeItem;
class PlayerItem;
class Door;
class ItemDataManager;


/**
 * In Isomot, column is group of grid items to draw along Z axis
 */
typedef std::list < GridItem* > Column ;

/**
 * Update for all items
 */
void* updateThread ( void* thisClass ) ;


/**
 * Mediator for various items in one room. It collects requests sent by items at some events,
 * and forwards them to other items
 */

class Mediator
{

public:

       /**
        * Constructor
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

       /**
        * Señala para el motor qué elementos libres deben volverse a enmascarar. Se usa cuando un elemento
        * libre cambia su imagen dado que el cambio afecta al resto de elementos cuyas imágenes se solapan con
        * la suya
        */
        void markItemsForMasking( FreeItem* item ) ;

       /**
        * Señala para el motor qué elementos libres deben volverse a enmascarar. Se usa cuando un elemento
        * rejilla cambia su posición dado que el cambio afecta al resto de elementos cuyas imágenes se
        * solapan con la suya y espacialmente quedan detrás de él
        */
        void markItemsForMasking( GridItem* gridItem ) ;

       /**
        * Señala para el motor qué elementos deben sombrearse. Se usa cuando un elemento rejilla es añadido a la
        * sala o cambia su sombra dado que el cambio afecta al resto de elementos que se encuentren debajo de él
        */
        void markItemsForShady( GridItem* gridItem ) ;

       /**
        * Señala para el motor qué elementos deben sombrearse. Se usa cuando un elemento libre es añadido a la
        * sala o cambia su sombra dado que el cambio afecta al resto de elementos que se encuentren debajo de él
        */
        void markItemsForShady( FreeItem* freeItem ) ;

       /**
        * Sombrea una loseta
        */
        void castShadowOnFloor( FloorTile* floorTile ) ;

       /**
        * Sombrea un elemento rejilla
        */
        void castShadowOnGrid( GridItem* gridItem ) ;

       /**
        * Sombrea un elemento libre
        */
        void castShadowOnFreeItem( FreeItem* freeItem ) ;

       /**
        * Enmascara un elemento libre
        */
        void mask ( FreeItem* freeItem ) ;

       /**
        * Find item in room by its identifier
        * @param id Identifier of item given by engine
        * @return item found or 0 if no such item
        */
        Item* findItemById ( int id ) ;

       /**
        * Find item in room by its label
        * @param label Label of item, when there are several elements with this label return the first found one
        * @return item found or 0 if no such item
        */
        Item* findItemByLabel ( const std::string& label ) ;

       /**
        * Find item in room by its behavior
        */
        Item* findItemByBehavior ( const std::string& behavior ) ;

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

       /**
        * Inserta un elemento rejilla en su estructura de datos correspondiente
        */
        void addItem ( GridItem* gridItem ) ;

       /**
        * Inserta un elemento libre en su estructura de datos correspondiente
        */
        void addItem ( FreeItem* freeItem ) ;

       /**
        * Inserta un jugador en su estructura de datos correspondiente
        */
        void addItem ( PlayerItem* playerItem ) ;

       /**
        * Elimina un elemento rejilla de su lista correspondiente
        */
        void removeItem ( GridItem* gridItem ) ;

       /**
        * Elimina un elemento rejilla de su lista correspondiente
        */
        void removeItem ( FreeItem* freeItem ) ;

       /**
        * Elimina un jugador de su lista correspondiente. Además selecciona a otro jugador
        * como jugador activo
        */
        void removeItem ( PlayerItem* playerItem ) ;

        void addToTableOfTransparencies ( unsigned char percent )
        {
                transparencies->addToDegreeOfTransparency( percent ) ;
        }

        void removeFromTableOfTransparencies ( unsigned char percent )
        {
                transparencies->removeFromDegreeOfTransparency( percent ) ;
        }

       /**
        * Añade un elemento a la pila de colisiones
        * @param id Identificador del elemento
        */
        void pushCollision ( int id ) ;

       /**
        * Saca el primer elemento de la pila de colisiones
        * @return Un identificador de un elemento asignado por el motor ó 0 si la pila está vacía
        */
        int popCollision () ;

       /**
        * Vacía la pila de colisiones
        */
        void clearStackOfCollisions () {  collisions.clear() ;  }

       /**
        * Indica si la pila de colisiones está vacía
        * @return true si está vacía o false en caso contrario
        */
        bool isStackOfCollisionsEmpty () {  return collisions.empty() ;  }

       /**
        * Número de elementos en la pila de colisiones
        * @return Un número positivo ó 0 si la pila está vacía
        */
        unsigned int depthOfStackOfCollisions () {  return collisions.size() ;  }

       /**
        * Indica si un elemento ha chocado con otro de un tipo determinado
        * @return El elemento con el que se ha producido la colisión ó 0 si no hay colisión
        */
        Item* collisionWithByLabel ( const std::string& label ) ;

       /**
        * Indica si un elemento ha chocado con otro de un comportamiento determinado
        * @return El elemento con el que se ha producido la colisión ó 0 si no hay colisión
        */
        Item* collisionWithByBehavior ( const std::string& behavior ) ;

        Item* findCollisionPop () {  return this->findItemById( this->popCollision() ) ;  }

       /**
        * Indica si un elemento ha chocado con otro capaz de quitar vida a un jugador y que pueda
        * ser detenido por la acción de un disparo
        * @return El elemento con el que se ha producido la colisión ó 0 si no hay colisión
        */
        Item* collisionWithBadBoy () ;

       /**
        * Select next player present in room
        * @param itemDataManager Needed to create composite player from simple ones
        *                        or to create simple players from composite
         * @return true if it’s possible to swap or false if there’s only one player in this room
        */
        bool selectNextPlayer ( ItemDataManager* itemDataManager ) ;

       /**
        * Select player which is alive after active player is over by loosing all its lives
        * @param itemDataManager Needed to create simple player from composite
        * @return true if there’s alive player to swap or false if there’re no more players
        */
        bool selectAlivePlayer ( ItemDataManager* itemDataManager ) ;

       /**
        * Toggle the switch if it is present in a room, most rooms do not have a switch. If it
        * turns on then free items stop moving and volatile items are no longer volatile. If it
        * goes off then such items restore their original behavior
        */
        void toggleSwitchInRoom () ;

       /**
        * Activa la ordenación de las listas de elementos rejilla
        */
        void activateGridItemsSorting () {  this->gridItemsSorting = true ;  }

       /**
        * Activa la ordenación de la lista de elementos libres
        */
        void activateFreeItemsSorting () {  this->freeItemsSorting = true ;  }

       /**
        * Activa el mútex de los elementos rejilla
        */
        void lockGridItemMutex () {  pthread_mutex_lock( &gridItemsMutex ) ;  }

       /**
        * Activa el mútex de los elementos libres
        */
        void lockFreeItemMutex () {  pthread_mutex_lock( &freeItemsMutex ) ;  }

       /**
        * Desactiva el mútex de los elementos rejilla
        */
        void unlockGridItemMutex () {  pthread_mutex_unlock( &gridItemsMutex ) ;  }

       /**
        * Desactiva el mútex de los elementos libres
        */
        void unlockFreeItemMutex () {  pthread_mutex_unlock( &freeItemsMutex ) ;  }

protected:

       /**
        * Señala para el motor qué elementos libres deben sombrearse. Se usa cuando un elemento es añadido a la
        * sala o cambia su sombra dado que el cambio afecta al resto de elementos que se encuentren debajo de él
        * @param item Elemento que solicita la operación
        */
        void markFreeItemsForShady ( Item* item ) ;

private:

       /**
        * Predicado binario para la ordenación una lista de elementos rejilla
        */
        static bool sortGridItemList ( GridItem* g1, GridItem* g2 ) ;

       /**
        * Predicado binario para la ordenación la lista de elementos libres
        */
        static bool sortFreeItemList ( FreeItem* f1, FreeItem* f2 ) ;

private:

        friend class Room ;

       /**
        * Identificador del subproceso de actualización de los elementos
        */
        pthread_t thread ;

       /**
        * Mútex para controlar el acceso a los elementos rejilla
        */
        pthread_mutex_t gridItemsMutex ;

       /**
        * Mútex para controlar el acceso a los elementos libres
        */
        pthread_mutex_t freeItemsMutex ;

       /**
        * Indica si el subproceso de actualización elementos está o no ejecutándose
        */
        bool threadRunning ;

       /**
        * Indica si las listas de elementos rejilla están ordenadas
        */
        bool gridItemsSorting ;

       /**
        * Indica si la lista de elementos libres está ordenada
        */
        bool freeItemsSorting ;

        bool switchInRoomIsOn ;

       /**
        * Player which was active just before change of players
        */
        std::string lastActivePlayer ;

       /**
        * Sala en la que negocia este mediador
        */
        Room* room ;

       /**
        * Conjunto de elementos que forman la estructura de una sala. Cada columna del vector está compuesta de
        * una lista de elementos rejilla cuya coordenada Z es mayor que la coordenada del elemento precedente,
        * es decir, forman una pila, aunque dicha pila puede tener huecos
        */
        std::vector < Column > structure ;

       /**
        * Conjunto de elementos libres de una sala
        */
        std::list < FreeItem * > freeItems ;

       /**
        * Jugadores presentes en la sala
        */
        std::vector < PlayerItem * > playerItems ;

       /**
        * Items that may take life from player and that may be freezed by doughnut or by switch
        */
        std::vector < std::string > badBoys ;

       /**
        * Jugador controlado por el usuario
        */
        PlayerItem * activePlayer ;

       /**
        * Pila de colisiones de la sala. Almacena los identificadores de los elementos
        */
        std::vector < int > collisions ;

        TableOfTransparencies * transparencies ;

public:

       /**
        * Indica si el subproceso de actualización de elementos está ejecutándose
        * @return true si está en ejecución o false si está detenido
        */
        bool isThreadRunning () {  return threadRunning ;  }

       /**
        * Coordenadas de pantalla X donde está situada la coordenada origen de la sala
        */
        int getX0 () const {  return room->getX0() ;  }

       /**
        * Coordenadas de pantalla Y donde está situada la coordenada origen de la sala
        */
        int getY0 () const {  return room->getY0() ;  }

       /**
        * Devuelve el número de losetas de la sala en el eje X
        */
        int getTilesX () const {  return room->tilesNumber.first ;  }

       /**
        * Devuelve el número de losetas de la sala en el eje X
        */
        int getTilesY () const {  return room->tilesNumber.second ;  }

       /**
        * Returns length of side of single tile in isometric units, a multiple of two
        */
        int getSizeOfOneTile () const {  return room->tileSize ;  }

       /**
        * Degree for opacity of shadows
        * @return A value between 0, no shadows, up to 256, fully opaque shadows
        */
        short getDegreeOfShading () const {  return room->shadingScale ;  }

       /**
        * Returns limit of room in isometric units
        * @param direction Un punto cardinal indicativo del límite que se quiere obtener
        */
        unsigned short getBound ( const Direction& direction ) ;

       /**
        * Sala en la que negocia este mediador
        */
        Room* getRoom () const {  return room ;  }

        std::list < FreeItem * > getFreeItems () {  return freeItems ;  }

       /**
        * Establece el jugador controlado por el usuario
        */
        void setActivePlayer ( PlayerItem* playerItem ) ;

       /**
        * Jugador controlado por el usuario
        * @return Un jugador ó 0 si no hay ninguno en la sala. El segundo caso puede darse durante
        * la transición entre salas
        */
        PlayerItem* getActivePlayer () const {  return this->activePlayer ;  }

       /**
        * Jugador inactivo, aquel que no está controlando el usuario
        * @return Un jugador ó 0 si no hay más jugadores
        */
        PlayerItem* getHiddenPlayer () const ;

       /**
        * Acceso a las puertas de la sala
        * @param direction Un punto cardinal que señala el muro donde se encuentra la puerta
        * @return Una puerta ó 0 si la puerta no existe para la dirección indicada
        */
        Door* getDoor ( const Direction& direction ) ;

};


class EqualItemId : public std::binary_function< Item *, int, bool >
{

public:
        bool operator() ( Item* item, int id ) const ;

};

class EqualPlayerItemId : public std::binary_function< PlayerItem *, int, bool >
{

public:
        bool operator() ( PlayerItem* playerItem, int id ) const ;

};

class EqualItemLabel : public std::binary_function< Item *, std::string, bool >
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

// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef PlayerItem_hpp_
#define PlayerItem_hpp_

#include <vector>
#include "FreeItem.hpp"
#include "Timer.hpp"


namespace isomot
{

class ItemData ;
class GameManager ;


/**
 * Item of player controller by the user
 */

class PlayerItem : public FreeItem
{

public:

        PlayerItem( ItemData* itemData, int x, int y, int z, const Way& orientation ) ;

        /**
         * Constructor copia. No copia los atributos que son punteros
         * @param playerItem Un objeto de esta clase
         */
        PlayerItem( const PlayerItem& playerItem ) ;

        virtual ~PlayerItem( ) ;

        virtual std::string whichKindOfItem () const {  return "player item" ;  }

        /**
         * Change activity of player by way of entry to room
         */
        void autoMoveOnEntry ( const Way& wayOfEntry ) ;

        virtual void behave () ;

        /**
         * Updates behavior of item
         */
        virtual bool update () ;

        /**
         * El jugador espera a que el usuario le dé una orden, lo que implica detener al jugador o, lo que
         * es lo mismo, establecer el fotograma de parada
         */
        void wait () ;

        void fillWithData ( const GameManager * data ) ;

        bool isActivePlayer () ;

        /**
         * Añade vidas al jugador
         * @param lives Número de vidas a sumar
         */
        void addLives( unsigned char lives ) ;

        /**
         * El jugador pierde una vida
         */
        void loseLife () ;

        /**
         * Player takes magic item
         * @label Label of item
         */
        void takeTool( const std::string& label ) ;

        void addDoughnuts( const unsigned short howMany ) ;

        void useDoughnut () ;

        /**
         * Activa el movimiento a doble velocidad del personaje
         */
        void activateHighSpeed () ;

        /**
         * El jugador consume una unidad de tiempo del movimiento a doble velocidad
         */
        void decreaseHighSpeed () ;

        /**
         * Añade un número de grandes saltos al jugador
         * @param highJumps Un número entre 0 y 10
         */
        void addHighJumps ( unsigned char highJumps ) ;

        /**
         * El jugador consume un gran salto
         */
        void decreaseHighJumps () ;

        /**
         * Activa la inmunidad del personaje
         */
        void activateShield () ;

        /**
         * El jugador consume una unidad de tiempo de inmunidad
         */
        void decreaseShield () ;

        /**
         * El jugador libera un planeta
         */
        void liberatePlanet() ;

        /**
         * Save game when player meets reincarnation fish
         */
        void save () ;

        void saveAt ( int x, int y, int z ) ;

protected:

        /**
         * @param newX new X coordinate, ignored when whatToChange doesn’t have CoordinateX
         * @param newY new Y coordinate, ignored when whatToChange doesn’t have CoordinateY
         * @param newZ new Z coordinate, ignored when whatToChange doesn’t have CoordinateZ
         * @param whatToChange what to change: CoordinateX, CoordinateY, CoordinateZ, or any sum of them like CoordinatesXYZ
         * @param how how to interpret new value, change or add
         * @return true if data may be changed or false when there’s a collision
         */
        virtual bool updatePosition ( int newX, int newY, int newZ, const Coordinate& whatToChange, const ChangeOrAdd& how ) ;

        /**
         * Check if player hits a door, if yes then move player
         * @param way door mentioned by its position in room
         * @param id identifier of item, assigned by engine, in stack of collisions
         */
        bool isCollidingWithDoor ( const Way& way, int id, const PlayerItem& previousPosition ) ;

        /**
         * See if player is not under a door or that door doesn’t exist
         * @param way door mentioned by its position in room
         * @return true if the player is under that given door
         */
        bool isNotUnderDoor ( const Way& way ) ;

        /**
         * See if player crosses limits of room, if yes then change rooms
         * @param way door mentioned by its position in room
         */
        bool isCollidingWithRoomBorder( const Way& way ) ;

private:

        /**
         * Número de vidas del jugador
         */
        unsigned char lives ;

        /**
         * Tiempo restante de movimiento a doble velocidad. Si el jugador no posee esta habilidad
         * entonces vale cero. Por observación se concluye que:
         * 1. Si el jugador va paso a paso no gasta
         * 2. Si el jugador se mueve pero colisiona, no gasta
         * 3. Consume una unidad de tiempo cada 8 pasos, siempre y cuando se den 4 seguidos
         * 4. Cuando salta no consume
         */
        unsigned int highSpeed ;

        /**
         * Número de grandes saltos del jugador
         */
        unsigned int highJumps ;

        /**
         * Tiempo restante de inmunidad. Si el jugador no posee esta habilidad entonces vale cero
         */
        double shield ;

        /**
         * Tools of player
         */
        std::vector< std::string > tools ;

        unsigned short howManyDoughnuts ;

        /**
         * Way of player when it leaves room
         */
        Way exit ;

        /**
         * How player enters room: through door, or via teleport, or going below floor or above ceiling
         */
        Way entry ;

        /**
         * Time of immunity, player gots 25 seconds of immunity when rabbit is caught
         */
        Timer * shieldTimer ;

        /**
         * How many seconds left during which player is immune
         */
        double shieldTime ;

        ItemData* takenItemData ;

        BITMAP* takenItemImage ;

        std::string takenItemBehavior ;

        /**
         * Used when morphing to bubbles and back
         */
        ItemData* originalDataOfItem ;

protected:

        void setLives ( unsigned char lives ) {  this->lives = lives ;  }

        /**
         * Tiempo restante de doble velocidad
         * @param highSpeed Number between 0 and 99
         */
        void setHighSpeed ( unsigned int highSpeed ) {  this->highSpeed = highSpeed ;  }

        /**
         * Establece el número de grandes saltos del jugador
         * @param highJumps Number between 0 and 10
         */
        void setHighJumps ( unsigned int highJumps ) {  this->highJumps = highJumps ;  }

        /**
         * Establece el tiempo total de inmunidad y activa el cronómetro si está activado
         * @param shield Un número de segundos
         */
        void setShieldTime ( double shield ) ;

        /**
         * Asigna los objetos de utilidad que tiene el jugador
         */
        void setTools ( const std::vector< std::string >& tools ) {  this->tools = tools ;  }

        void setDoughnuts ( const unsigned short howMany ) {  this->howManyDoughnuts = howMany ;  }

        double getShieldTime () const {  return this->shieldTime ;  }

public:

        /**
         * Set data for item in player’s handbag
         */
        void assignTakenItem ( ItemData * itemData, BITMAP * itemImage, const std::string & behavior ) ;

        ItemData * getOriginalDataOfItem () const {  return originalDataOfItem ;  }

        /**
         * Data of item in handbag
         */
        ItemData* getTakenItemData () const {  return this->takenItemData ;  }

        /**
         * Image of item taken by player
         */
        BITMAP* getTakenItemImage () const {  return this->takenItemImage ;  }

        /**
         * Behavior of item taken by player
         */
        std::string getTakenItemBehavior () const {  return this->takenItemBehavior ;  }

        unsigned char getLives () const {  return this->lives ;  }

        /**
         * Returns remaining steps at double speed between 0 and 99
         */
        unsigned int getHighSpeed () const {  return this->highSpeed ;  }

        /**
         * Returns number of high jumps between 0 and 10
         */
        unsigned int getHighJumps () const {  return this->highJumps ;  }

        bool hasShield () const {  return this->shieldTime > 0 ;  }

        /**
         * Character has its magic item, horn or bag, or not
         */
        bool hasTool ( const std::string& label ) const ;

        unsigned short getDoughnuts () const {  return this->howManyDoughnuts ;  }

        /**
         * Way of player when it leaves room
         */
        void setWayOfExit ( const Way& way ) ;

        Way getWayOfExit () const {  return this->exit ;  }

        /**
         * How player enters room
         */
        Way getWayOfEntry () const {  return this->entry ;  }

        void setWayOfEntry ( const Way& way ) {  this->entry = way ;  }

};

}

#endif

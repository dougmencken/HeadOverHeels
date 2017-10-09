// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
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
#include "HPC.hpp"


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

        /**
         * Constructor
         * @param itemData Datos invariables del elemento
         * @param x Posición espacial X
         * @param y Posición espacial Y
         * @param z Posición espacial Z o a qué distancia está el elemento del suelo
         * @param direction Dirección inicial del elemento
         */
        PlayerItem( ItemData* itemData, int x, int y, int z, const Direction& direction ) ;

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
        void autoMoveOnEntry ( const Direction& entry ) ;

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
         * Set data of item in handbag
         */
        void assignTakenItem( ItemData* itemData, BITMAP* takenItemImage, const std::string& behavior ) ;

        /**
         * Get data of item in bag
         */
        std::string consultTakenItem( ItemData* itemData ) ;

        /**
         * Get image of item taken by player
         */
        BITMAP * consultTakenItemImage () ;

        /**
         * Save game when player meets reincarnation fish
         */
        void save () ;

        void saveAt ( int x, int y, int z ) ;

protected:

        /**
         * Cambia el dato solicitado del elemento
         * @param value El nuevo valor
         * @param x La nueva coordenada X. El parámetro se ignora si datum no es CoordinatesXYZ
         * @param y La nueva coordenada Y. El parámetro se ignora si datum no es CoordinatesXYZ
         * @param z La nueva coordenada Z. El parámetro se ignora si datum no es CoordinatesXYZ
         * @param datum El dato que se quiere modificar: CoordinateX, la coordenada X; CoordinateY, la
         * coordenada Y; CoordinateZ, la coordenada Z; WidthX, la anchura en el eje X; WidthY, la anchura en
         * el eje Y; o, Height, la altura
         * @param how Modo para interpretar el nuevo valor. Puede usarse Change para cambiarlo o Add para sumarlo
         * @param jumpIndex Índice del salto utilizado para el desplazamiento de elementos situados encima
         * del jugador durante el ascenso
         * @return true si se pudo cambiar el dato o false si hubo colisión y no hubo cambio
         */
        virtual bool changeData ( int value, int x, int y, int z, const Datum& datum, const WhatToDo& how ) ;

        /**
         * Check if player hits a door, if yes then move player
         * @param direction Una puerta identificada por su posición en la sala
         * @param id Assigned by engine identifier of item in stack of collisions
         */
        bool isCollidingWithDoor ( const Direction& direction, int id, const PlayerItem& previousPosition ) ;

        /**
         * See if player is not under a door or that door doesn’t exist
         * @param direction Una puerta identificada por su posición en la sala
         * @return true if the player is under that given door
         */
        bool isNotUnderDoor ( const Direction& direction ) ;

        /**
         * See if player crosses limits of room, if yes then change rooms
         * @param direction Una puerta identificada por su posición en la sala
         */
        bool isCollidingWithRoomBorder( const Direction& direction ) ;

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
         * Direction of player when it leaves room
         */
        Direction exit ;

        /**
         * How player enters room: through door, or via teleport, or going below floor or above ceiling
         */
        Direction entry ;

        /**
         * Time of immunity, player gots 25 seconds of immunity when rabbit is caught
         */
        HPC * shieldTimer ;

        /**
         * How many seconds left during which player is immune
         */
        double shieldTime ;

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

public:

        unsigned char getLives () const {  return this->lives ;  }

        /**
         * Returns remaining steps at double speed between 0 and 99
         */
        unsigned int getHighSpeed () const {  return this->highSpeed ;  }

        /**
         * Returns number of high jumps between 0 and 10
         */
        unsigned int getHighJumps () const {  return this->highJumps ;  }

        /**
         * Returns time of immunity in seconds
         */
        double getShieldTime () const {  return this->shieldTime ;  }

        /**
         * Character has its magic item, horn or bag, or not
         */
        bool hasTool ( const std::string& label ) const ;

        unsigned short getDoughnuts () const {  return this->howManyDoughnuts ;  }

        /**
         * Direction of player when it leaves room
         */
        void setDirectionOfExit ( const Direction& direction ) ;

        Direction getDirectionOfExit () const {  return this->exit ;  }

        /**
         * How player enters room
         */
        Direction getDirectionOfEntry () const {  return this->entry ;  }

        void setDirectionOfEntry ( const Direction& direction ) {  this->entry = direction ;  }

};

}

#endif

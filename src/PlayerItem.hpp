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

class ItemData;
class Mediator;

/**
 * Los elementos jugador, los jugadores, son aquellos elementos controlador por el usuario
 * Son una especialización de los elementos libres porque pueden atravesar puertas, cambiar
 * de sala, coger otros elementos o destruirlos
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

        virtual void behave () ;

        /**
         * Actualiza el comportamiento del elemento
         */
        virtual bool update () ;

        /**
         * El jugador espera a que el usuario le dé una orden, lo que implica detener al jugador o, lo que
         * es lo mismo, establecer el fotograma de parada
         */
        void wait () ;

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
         * El jugador coge un objeto de utilidad
         * @label Etiqueta del elemento
         */
        void takeTool( short label ) ;

        /**
         * Aumenta la cantidad de munición que tiene el jugador (rosquillas)
         * @param ammo Un número mayor o igual que cero
         */
        void addAmmo( const unsigned short ammo ) ;

        /**
         * El jugador consume una unidad de munición
         */
        void consumeAmmo () ;

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
         * Establece los datos del elemento que tiene el jugador en el bolso
         * @param data Datos del elemento tomado por el jugador
         * @param takenItemImage Imagen del elemento tomado por el jugador
         * @param behavior Comportamiento del elemento tomado por el jugador
         */
        void assignTakenItem( ItemData* itemData, BITMAP* takenItemImage, const BehaviorId& behavior ) ;

        /**
         * Obtiene los datos del elemento que tiene el jugador en el bolso
         * @param data Devuelve los datos del elemento tomado por el jugador
         * @param behavior Devuelve el comportamiento del elemento tomado por el jugador
         */
        ItemData* consultTakenItem( BehaviorId* behavior ) ;

        /**
         * Obtiene la imagen del elemento tomado por el jugador
         * @return Una imagen ó 0 si el jugador no tiene ningún elemento
         */
        BITMAP* consultTakenItemImage () ;

        /**
         * Desencadena la grabación de la partida cuando este jugador coge el pez de la reencarnación
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
         * Comprueba si el jugador ha chocado con alguna puerta. En caso afirmativo moverá al jugador
         * hacia el vado de la puerta
         * @param direction Una puerta identificada por su posición en la sala
         * @param mediator El mediador de los elementos
         * @param id Identificador asignado por el motor de un elemento existente en la pila de colisiones
         * @return true si hubo colisión y por tanto desplazamiento del jugador o false en caso contrario
         */
        bool isCollidingWithDoor ( const Direction& direction, Mediator* mediator, int id, const PlayerItem& previousPosition ) ;

        /**
         * Comprueba que el jugador no esté debajo de una puerta o bien que la puerta no exista
         * @param direction Una puerta identificada por su posición en la sala
         * @param mediator El mediador de los elementos
         * @return true si el jugador está bajo la puerta indicada o false en caso contrario
         */
        bool isNotUnderDoor( const Direction& direction, Mediator* mediator ) ;

        /**
         * Comprueba si el jugador ha chocado con los límites de la sala. Implicaría un cambio de sala
         * @param direction Una puerta identificada por su posición en la sala
         * @param mediator El mediador de los elementos
         * @return true si ha alcanzado el límite de la sala o false en caso contrario
         */
        bool isCollidingWithRoomBorder( const Direction& direction, Mediator* mediator ) ;

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
         * Indica si el jugador tiene su útil (bocina o bolso)
         */
        std::vector< short > tools ;

        /**
         * Cantidad de munición que tiene el jugador (rosquillas)
         */
        unsigned short ammo ;

        /**
         * Dirección de salida del jugador al abandonar la sala
         */
        Direction exit ;

        /**
         * Orientación, dirección a la que mira el jugador al abandonar la sala
         */
        Direction orientation ;

        /**
         * Cronómetro de la inmunidad. Una vez se coja el conejo el jugador dispone de 25 segundos de inmunidad
         */
        HPC* shieldTimer ;

        /**
         * Tiempo en segundos que un jugador es inmune
         */
        double shieldTime ;

public: // Operaciones de consulta y actualización

        /**
         * Establece el número de vidas del jugador
         * @param lives Un número mayor o igual que cero
         */
        void setLives ( unsigned char lives ) {  this->lives = lives ;  }

        /**
         * Indica el número de vidas del jugador
         * @return Un número mayor o igual que cero
         */
        unsigned char getLives () const {  return this->lives ;  }

        /**
         * Tiempo restante de doble velocidad
         * @param highSpeed Un número entre 0 y 99
         */
        void setHighSpeed ( unsigned int highSpeed ) {  this->highSpeed = highSpeed ;  }

        /**
         * Devuelve el tiempo restante de doble velocidad
         * @return Un número entre 0 y 99
         */
        unsigned int getHighSpeed () const {  return this->highSpeed ;  }

        /**
         * Establece el número de grandes saltos del jugador
         * @param highSpeed Un número entre 0 y 10
         */
        void setHighJumps ( unsigned int highJumps ) {  this->highJumps = highJumps ;  }

        /**
         * Devuelve el número de grandes saltos del jugador
         * @return Un número entre 0 y 10
         */
        unsigned int getHighJumps () const {  return this->highJumps ;  }

        /**
         * Establece el tiempo total de inmunidad y activa el cronómetro si está activado
         * @param shield Un número de segundos
         */
        void setShieldTime ( double shield ) ;

        /**
         * Devuelve el tiempo total de inmunidad
         * @return Un número de segundos
         */
        double getShieldTime () const {  return this->shieldTime ;  }

        /**
         * Asigna los objetos de utilidad que tiene el jugador
         * @param hasTool true si lo posee o false en caso contrario
         */
        void setTools ( const std::vector< short >& tools ) {  this->tools = tools ;  }

        /**
         * Indica si el jugador tiene su útil (bocina o bolso)
         * @return true si tiene el útil o false en caso contrario
         */
        bool hasTool ( const MagicItem& label ) const ;

        /**
         * Establece la cantidad de munición que tiene el jugador (rosquillas)
         * @param ammo Un número mayor o igual que cero
         */
        void setAmmo ( const unsigned short ammo ) {  this->ammo = ammo ;  }

        /**
         * Indica la cantidad de munición que tiene el jugador (rosquillas)
         * @return Un número mayor o igual que cero
         */
        unsigned short getAmmo () const {  return this->ammo ;  }

        /**
         * Establece la dirección de salida del jugador al abandonar la sala
         * @param direction El suelo, el techo o la ubicación de alguna puerta
         */
        void setExit ( const Direction& direction ) {  this->exit = direction ;  }

        /**
         * Dirección de salida del jugador al abandonar la sala
         * @return El suelo, el techo o la ubicación de alguna puerta
         */
        Direction getExit () const {  return this->exit ;  }

        /**
         * Establece la orientación, dirección a la que mira el jugador al abandonar la sala
         * @param direction Algún punto cardinal
         */
        void setOrientation ( const Direction& direction ) {  this->orientation = direction ;  }

        /**
         * Orientación, dirección a la que mira el jugador al abandonar la sala
         * @return Algún punto cardinal
         */
        Direction getOrientation () const {  return this->orientation ;  }

};

}

#endif

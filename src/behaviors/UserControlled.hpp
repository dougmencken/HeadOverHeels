// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef UserControlled_hpp_
#define UserControlled_hpp_

#include <vector>
#include <utility>

#include "Behavior.hpp"
#include "HPC.hpp"


namespace isomot
{

class Item ;
class ItemData ;
class ItemDataManager ;
class PlayerItem ;


/**
 * Abstraction for every element controlled by the user. Contains attributes and actions which
 * a player may do, and what are used in the "update" action which is nothing more than a state machine
 * that together with the "behave" action models the behavior of the player
 */

class UserControlled : public Behavior
{

public:

       /**
        * @param item Elemento que tiene este comportamiento
        * @param id Identificador del comportamiento
        */
        UserControlled( Item * item, const BehaviorOfItem & id ) ;

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
        * Asigna el identificador del estado actual del comportamiento y cambia el estado
        * del comportamiento según el identificador
        */
        virtual void changeActivityOfItem ( const ActivityOfItem& activityOf, Item* sender = 0 ) ;

protected:

       /**
        * Pone en espera al jugador. Implica la presentación del primer fotograma de la
        * secuencia de animación según la orientación del jugador
        * @param player El jugador que va a cambiar su comportamiento
        */
        virtual void wait( PlayerItem * player ) ;

       /**
        * Mueve al jugador a la velocidad establecida en los datos del elemento y en la
        * dirección de alguno de los cuatro puntos cardinales
        * @param player El jugador que va a cambiar su comportamiento
        */
        virtual void move( PlayerItem * player ) ;

       /**
        * Mueve automáticamente al jugador a la velocidad establecida en los datos del elemento
        * y en la dirección de alguno de los cuatro puntos cardinales, una distancia igual al
        * número definido en los pasos automáticos (automaticSteps)
        * @param player El jugador que va a cambiar su comportamiento
        */
        virtual void autoMove( PlayerItem * player ) ;

       /**
        * Desplaza al jugador a la velocidad del elemento que lo empuja y en la dirección de alguno
        * de los cuatro puntos cardinales y sus puntos intermedios, es decir, en ocho direcciones
        * posibles
        * @param player El jugador que va a cambiar su comportamiento
        */
        virtual void displace( PlayerItem * player ) ;

       /**
        * Cancela el desplazamiento del jugador moviéndolo en la dirección contraria a la que está
        * siendo desplazado. Tiene el efecto de dejarlo parado en un punto al contrarrestar la fuerza
        * de desplazamiento. Este estado se utiliza normalmente cuando el jugador es arrastrado por
        * una cinta transportadora o algún elemento similar
        * @param player El jugador que va a cambiar su comportamiento
        */
        virtual void cancelDisplace( PlayerItem * player ) ;

       /**
        * El jugador cae hasta topar con algo a la velocidad establecida en los datos del elemento
        * @param player El jugador que va a cambiar su comportamiento
        */
        virtual void fall( PlayerItem * player ) ;

       /**
        * Hace saltar al jugador según la definición del salto realizada en las subclases
        * @param player El jugador que va a cambiar su comportamiento
        */
        virtual void jump( PlayerItem * player ) ;

       /**
        * Hace planear al jugador a la velocidad establecida en los datos del elemento
        * y en la dirección de alguno de los cuatro puntos cardinales
        * @pre El jugador debe estar en el aire porque esté cayendo o haya saltado
        * @param player El jugador que va a cambiar su comportamiento
        */
        virtual void glide( PlayerItem * player ) ;

       /**
        * Teletransporta al jugador desde otra sala
        * @param player El jugador que va a cambiar su comportamiento
        */
        virtual void wayInTeletransport( PlayerItem * player ) ;

       /**
        * Teletransporta al jugador hacia otra sala
        * @param player El jugador que va a cambiar su comportamiento
        */
        virtual void wayOutTeletransport( PlayerItem * player ) ;

       /**
        * Destruye al jugador por haber sido eliminado por un enemigo
        * @param player El jugador que va a cambiar su comportamiento
        */
        virtual void destroy( PlayerItem * player ) ;

       /**
        * El jugador dispara algo que paralizará a los elementos mortales móviles
        * @param player El jugador que va a cambiar su comportamiento
        */
        virtual void useHooter( PlayerItem * player ) ;

       /**
        * El jugador toma el elemento que haya debajo de él
        * @param player El jugador que va a cambiar su comportamiento
        */
        virtual void take( PlayerItem * player ) ;

       /**
        * El jugador deja un elemento justo debajo de él. Implica un ascenso del jugador en altura
        * @pre Debe tener un elemento que haya tomado con anterioridad
        * @param player El jugador que va a cambiar su comportamiento
        */
        virtual void drop( PlayerItem * player ) ;

protected:

       /**
        * Matriz que define el salto del jugador. Cada par de valores define el desplazamiento en el eje
        * X o Y y el desplazamiento en el eje Z en cada ciclo
        */
        std::vector < JumpMotion > jumpMatrix ;

       /**
        * Matriz que define el salto largo del jugador: aquel producido por un trampolín o por un conejito
        * de la velocidad. Cada par de valores define el desplazamiento en el eje X o Y y el desplazamiento
        * en el eje Z en cada ciclo
        */
        std::vector < JumpMotion > highJumpMatrix ;

       /**
        * Índice de la fase del salto que se está ejecutando
        */
        int jumpIndex ;

       /**
        * Número de fases de las que se compone el salto normal
        */
        int jumpFrames ;

       /**
        * Número de fases de las que se compone el salto largo
        */
        int highJumpFrames ;

       /**
        * Número total de pasos que dará el jugador en modo automático
        */
        int automaticSteps ;

       /**
        * Pasos automáticos que le restan por dar al jugador cuando accede a la sala por una puerta
        */
        int automaticStepsCounter ;

       /**
        * Número de pasos dado por el jugador a doble velocidad
        */
        int highSpeedSteps ;

       /**
        * Número de pasos dado por el jugador con inmunidad
        */
        int shieldSteps ;

       /**
        * Fotogramas de caída del jugador. Uno por cada punto cardinal
        */
        int fallFrames[ 4 ] ;

       /**
        * Fotograma en blanco del jugador. Se utiliza durante el cambio de sala a través de un telepuerto
        */
        int nullFrame ;

       /**
        * Is there a fire from hooter in the room
        */
        bool fireFromHooterIsPresent ;

       /**
        * Datos del elemento empleado como transición en el cambio de sala a través de un telepuerto
        */
        short transitionDataLabel ;

       /**
        * Item used as fire from hooter
        */
        short labelOfFireFromHooter ;

       /**
        * Gestor de datos de los elementos del juego
        */
        ItemDataManager* itemDataManager ;

       /**
        * Datos del jugador
        */
        ItemData* playerData ;

       /**
        * Datos del elemento tomado por el jugador
        */
        ItemData* takenItemData ;

       /**
        * Imagen del elemento tomado por el jugador
        */
        BITMAP* takenItemImage ;

       /**
        * Behavior of item taken by player
        */
        BehaviorOfItem takenItemBehavior ;

       /**
        * Cronómetro que controla la velocidad de movimiento del elemento
        */
        HPC* speedTimer ;

       /**
        * Cronómetro que controla la velocidad de caída del elemento
        */
        HPC* fallTimer ;

       /**
        * Timer for gliding speed of item
        */
        HPC* glideTimer ;

       /**
        * Timer for blinking of item
        */
        HPC* blinkingTimer ;

public:

       /**
        * Número de fases de las que se compone el salto
        */
        int getJumpFrames () const {  return jumpFrames ;  }

        void setFireFromHooter ( bool isHere ) {  this->fireFromHooterIsPresent = isHere ;  }

       /**
        * Almacena el gestor de datos de los elementos del juego
        * @param data Un puntero a un objeto o estructura de datos
        */
        void setMoreData ( void * data ) ;

       /**
        * Establece los datos del elemento que tiene el jugador en el bolso
        * @param itemData Datos del elemento tomado por el jugador
        * @param takenItemImage Imagen del elemento tomado por el jugador
        * @param behavior Comportamiento del elemento tomado por el jugador
        */
        void assignTakenItem ( ItemData * itemData, BITMAP * takenItemImage, const BehaviorOfItem & behavior ) ;

       /**
        * Datos del elemento tomado por el jugador
        * @return Un registro de datos de un elemento
        */
        ItemData* getTakenItemData () const {  return this->takenItemData ;  }

       /**
        * Imagen del elemento tomado por el jugador
        * @return Una imagen ó 0 si el jugador no tiene ningún elemento
        */
        BITMAP* getTakenItemImage () const {  return this->takenItemImage ;  }

        BehaviorOfItem getTakenItemBehavior () const {  return this->takenItemBehavior ;  }

};

}

#endif

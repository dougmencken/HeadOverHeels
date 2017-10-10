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

        UserControlled( Item * item, const std::string & behavior ) ;

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
        */
        virtual void wait( PlayerItem * player ) ;

       /**
        * Mueve al jugador a la velocidad establecida en los datos del elemento y en la
        * dirección de alguno de los cuatro puntos cardinales
        */
        virtual void move( PlayerItem * player ) ;

       /**
        * Move player automatically
        * at distance equal to number of automatic steps,
        * in direction of north or south or west or east,
        * at speed specified in item’s data
        */
        virtual void autoMove( PlayerItem * player ) ;

       /**
        * Move player at speed of item that pushes it in one of eight directions
        */
        virtual void displace( PlayerItem * player ) ;

       /**
        * Cancela el desplazamiento del jugador moviéndolo en la dirección contraria a la que está
        * siendo desplazado. Tiene el efecto de dejarlo parado en un punto al contrarrestar la fuerza
        * de desplazamiento. Este estado se utiliza normalmente cuando el jugador es arrastrado por
        * una cinta transportadora o algún elemento similar
        */
        virtual void cancelDisplace( PlayerItem * player ) ;

       /**
        * El jugador cae hasta topar con algo a la velocidad establecida en los datos del elemento
        */
        virtual void fall( PlayerItem * player ) ;

       /**
        * Hace saltar al jugador según la definición del salto realizada en las subclases
        */
        virtual void jump( PlayerItem * player ) ;

       /**
        * Hace planear al jugador a la velocidad establecida en los datos del elemento
        * y en la dirección de alguno de los cuatro puntos cardinales
        * @pre El jugador debe estar en el aire porque esté cayendo o haya saltado
        */
        virtual void glide( PlayerItem * player ) ;

       /**
        * Teletransporta al jugador desde otra sala
        */
        virtual void wayInTeletransport( PlayerItem * player ) ;

       /**
        * Teletransporta al jugador hacia otra sala
        */
        virtual void wayOutTeletransport( PlayerItem * player ) ;

       /**
        * Player collides with a mortal item
        */
        virtual void collideWithMortalItem( PlayerItem * player ) ;

       /**
        * Player releases something that freezes moving items
        */
        virtual void useHooter( PlayerItem * player ) ;

       /**
        * Take item underneath player
        */
        virtual void takeItem( PlayerItem * player ) ;

       /**
        * Drop item just below player
        */
        virtual void dropItem( PlayerItem * player ) ;

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
        * Item used as transition when changing room via teleport
        */
        std::string labelOfTransitionViaTeleport ;

       /**
        * Item used as fire from hooter
        */
        std::string labelOfFireFromHooter ;

       /**
        * Gestor de datos de los elementos del juego
        */
        ItemDataManager* itemDataManager ;

       /**
        * Datos del jugador
        */
        ItemData* playerData ;

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
        * Number of phases of jump
        */
        int getJumpFrames () const {  return jumpFrames ;  }

        void setFireFromHooter ( bool isHere ) {  this->fireFromHooterIsPresent = isHere ;  }

        void setMoreData ( void * data ) ;

};

}

#endif

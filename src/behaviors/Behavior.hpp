// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Behavior_hpp_
#define Behavior_hpp_

#include <string>
#include <stack>
#include "Ism.hpp"


namespace isomot
{

class Item ;
class BehaviorState ;

/**
 * Abstraction of behavior of an element. Elements of the game change activity in each cycle
 * of update. The transition between activities is defined by different kinds of behavior
 */

class Behavior
{

protected:

        /**
         * Constructor
         * @param item Elemento que tiene este comportamiento
         * @param id Identificador del comportamiento
         */
        Behavior( Item * item, const BehaviorId & id ) ;

public:

        virtual ~Behavior( ) ;

        /**
         * Crea el comportamiento del elemento
         * @param item Elemento que tiene este comportamiento
         * @param behaviorId Identificador del comportamiento del elemento
         * @param extra Datos extra necesarios para configurar el comportamiento
         * @return Un objeto de la clase con el comportamiento especificado
         */
        static Behavior* createBehavior ( Item* item, const BehaviorId& id, void* extraData ) ;

        /**
         * Updates behavior of the element in each cycle
         * @return false if this update causes destruction of the element or true otherwise
         */
        virtual bool update () = 0 ;

        /**
         * Asigna el identificador del estado actual del comportamiento y cambia el estado
         * del comportamiento según el identificador
         * @param Un identificador de estado
         * @param Elemento que emite el cambio de estado
         */
        virtual void changeStateId ( const StateId & stateId, Item * sender = 0 ) ;

protected:

        friend class BehaviorState ;

        /**
         * Cambia el estado del comportamiento
         * @param state El nuevo estado
         */
        void changeState ( BehaviorState * state ) {  this->state = state ;  }

        /**
         * Cambia el estado de todos los elementos que hayan colisionado con el emisor
         * @param sender Elemento que propaga el estado
         * @param stateId Identificador del estado
         */
        void propagateState ( Item * sender, const StateId & stateId ) ;

protected:

        /**
         * Identificador del comportamiento
         */
        BehaviorId id ;

        /**
         * Elemento que tiene este comportamiento
         */
        Item * item ;

        /**
         * Estado actual del comportamiento
         */
        BehaviorState * state ;

        /**
         * Elemento que cambia el estado del elemento con este comportamiento
         */
        Item * sender ;

        /**
         * Identificador del estado actual del comportamiento
         */
        StateId stateId ;

public: // Operaciones de consulta y actualización

        /**
         * Identificador del comportamiento
         * @return Un comportamiento predefinido
         */
        BehaviorId getId () const {  return id ;  }

        /**
         * Identificador del estado actual del comportamiento
         */
        StateId getStateId () const {  return stateId ;  }

        /**
         * Elemento con este comportamiento
         * @return Un elemento de la sala
         */
        Item* getItem () {  return item ;  }

        /**
         * Add some more data that may be needed for behavior
         */
        virtual void setMoreData ( void * data ) { }

};

}

#endif

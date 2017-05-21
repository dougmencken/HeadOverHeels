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
class KindOfActivity ;

/**
 * Abstraction of behavior of an element. Elements of the game change activity in each cycle
 * of update. The transition between activities is defined by different kinds of behavior
 */

class Behavior
{

protected:

        /**
         * Constructor
         * @param whichItem Elemento que tiene este comportamiento
         * @param behavior Identificador del comportamiento
         */
        Behavior( Item * whichItem, const BehaviorOfItem & behavior ) ;

public:

        virtual ~Behavior( ) ;

        /**
         * Crea el comportamiento del elemento
         * @param item Elemento que tiene este comportamiento
         * @param id Identificador del comportamiento del elemento
         * @param extraData Datos extra necesarios para configurar el comportamiento
         * @return Un objeto de la clase con el comportamiento especificado
         */
        static Behavior* createBehavior ( Item* item, const BehaviorOfItem& id, void* extraData ) ;

        /**
         * Updates behavior of the element in each cycle
         * @return false if this update causes destruction of the element or true otherwise
         */
        virtual bool update () = 0 ;

        /**
         * Asigna el identificador del estado actual del comportamiento y cambia el estado
         * del comportamiento según el identificador
         * @param Activity of item
         * @param Elemento que emite el cambio de estado
         */
        virtual void changeActivityOfItem ( const ActivityOfItem & activity, Item * sender = 0 ) ;

protected:

        friend class KindOfActivity ;

        /**
         * Change the kind of activity
         */
        void changeActivityTo ( KindOfActivity * kind ) {  this->whatToDo = kind ;  }

        /**
         * Change activity of every item collided with the sender
         */
        void propagateActivity ( Item * sender, const ActivityOfItem & activity ) ;

protected:

        BehaviorOfItem theBehavior ;

        /**
         * Elemento que tiene este comportamiento
         */
        Item * item ;

        /**
         * Current kind of activity
         */
        KindOfActivity * whatToDo ;

        /**
         * Current variant of activity
         */
        ActivityOfItem activity ;

        /**
         * Elemento que cambia el estado del elemento con este comportamiento
         */
        Item * sender ;

public:

        BehaviorOfItem getBehaviorOfItem () const {  return theBehavior ;  }

        ActivityOfItem getActivityOfItem () const {  return activity ;  }

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

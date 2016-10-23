// The free and open source remake of Head over Heels
//
// Copyright © 2016 Douglas Mencken dougmencken @ gmail.com
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

// Declaraciones adelantadas
class Item;
class BehaviorState;
class SoundManager;

/**
 * Abstracción del modelo de comportamiento de un elemento. Los elementos del juego cambiarán
 * de estado en cada ciclo de actualización. La transición entre estados está dirigida por las
 * diferentes clases de comportamiento
 */
class Behavior
{
protected:

        /**
         * Constructor
         * @param item Elemento que tiene este comportamiento
         * @param id Identificador del comportamiento
         */
        Behavior(Item* item, const BehaviorId& id);

public:

        virtual ~Behavior();

        /**
         * Crea el comportamiento del elemento
         * @param item Elemento que tiene este comportamiento
         * @param behaviorId Identificador del comportamiento del elemento
         * @param extra Datos extra necesarios para configurar el comportamiento
         * @return Un objeto de la clase con el comportamiento especificado
         */
        static Behavior* createBehavior(Item* item, const BehaviorId& id, void* extraData);

        /**
         * Actualiza el comportamiento del elemento en cada ciclo
         * @return false si la actualización implica la destrucción del elemento o true en caso contrario
         */
        virtual bool update() = 0;

        /**
         * Actualiza el estado del elemento en función de las órdenes dadas por el usuario
         */
        virtual void execute() {}

        /**
         * Asigna el identificador del estado actual del comportamiento y cambia el estado
         * del comportamiento según el identificador
         * @param Un identificador de estado
         * @param Elemento que emite el cambio de estado
         */
        virtual void changeStateId(const StateId& stateId, Item* sender = 0);

protected:

        friend class BehaviorState;

        /**
         * Cambia el estado del comportamiento
         * @param state El nuevo estado
         */
        void changeState(BehaviorState* state) { this->state = state; }

        /**
         * Cambia el estado de todos los elementos que hayan colisionado con el emisor
         * @param sender Elemento que propaga el estado
         * @param stateId Identificador del estado
         */
        void propagateState(Item* sender, const StateId& stateId);

protected:

        /**
         * Identificador del comportamiento
         */
        BehaviorId id;

        /**
         * Elemento que tiene este comportamiento
         */
        Item* item;

        /**
         * Estado actual del comportamiento
         */
        BehaviorState* state;

        /**
         * Elemento que cambia el estado del elemento con este comportamiento
         */
        Item* sender;

        /**
         * El gestor de sonido
         */
        SoundManager* soundManager;

        /**
         * Identificador del estado actual del comportamiento
         */
        StateId stateId;

public: // Operaciones de consulta y actualización

        /**
         * Identificador del comportamiento
         * @return Un comportamiento predefinido
         */
        BehaviorId getId() const { return id; }

        /**
         * Identificador del estado actual del comportamiento
         */
        StateId getStateId() const { return stateId; }

        /**
         * Elemento con este comportamiento
         * @return Un elemento de la sala
         */
        Item* getItem() { return item; }

        /**
         * Asigna datos extra que puedan necesitarse para ejecutar el comportamiento
         */
        virtual void setExtraData(void* data);
};

}

#endif

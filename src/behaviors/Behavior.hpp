// Head over Heels (A remake by helmántika.org)
//
// © Copyright 2008 Jorge Rodríguez Santos <jorge@helmantika.org>
// © Copyright 1987 Ocean Software Ltd. (Original game)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef BEHAVIOR_HPP_
#define BEHAVIOR_HPP_

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

#endif //BEHAVIOR_HPP_

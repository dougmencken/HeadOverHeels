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

#ifndef SPECIAL_HPP_
#define SPECIAL_HPP_

#include "Behavior.hpp"
#include "HPC.hpp"
#include "Ism.hpp"

namespace isomot
{

// Declaraciones adelantadas
class Item;
class ItemData;
class PlayerItem;

/**
 * Elemento que puede desplazarse sólo al ser empujado por otros, ya que
 * carece de movimiento autónomo
 */
class Special : public Behavior
{
public:

        /**
         * Constructor
         * @param item Elemento que tiene este comportamiento
         * @param id Identificador de comportamiento
         */
        Special( Item* item, const BehaviorId& id ) ;

        virtual ~Special () ;

        /**
         * Actualiza el comportamiento del elemento en cada ciclo
         * @return false si la actualización implica la destrucción del elemento o true en caso contrario
         */
        virtual bool update () ;

protected:

        /**
         * Comprueba si el elemento especial puede ser destruido. Depende del elemento en cuestión
         * y del jugador que lo tome
         * @param playerItem Jugador que toma el elemento
         */
        bool checkDestruction ( Item* sender ) ;

        /**
         * Ejecuta la acción asociada a la toma del elemento especial
         */
        void takeSpecial ( PlayerItem* sender ) ;

protected:

        /**
         * Datos del elemento empleado como disparo
         */
        ItemData* bubblesData ;

private:

        /**
         * Cronómetro que controla el tiempo de existencia del elemento volátil
         */
        HPC* destroyTimer ;

        /**
         * Cronómetro que controla la velocidad de movimiento del elemento
         */
        HPC* speedTimer ;

        /**
         * Cronómetro que controla la velocidad de caída del elemento
         */
        HPC* fallenTimer ;

public: // Operaciones de consulta y actualización

        /**
         * Asigna los datos del elemento usado como transición en la destrucción del volátil
         */
        void setExtraData ( void* data ) ;
};

}

#endif //SPECIAL_HPP_

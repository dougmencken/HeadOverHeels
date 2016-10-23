// The free and open source remake of Head over Heels
//
// Copyright © 2016 Douglas Mencken dougmencken @ gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Special_hpp_
#define Special_hpp_

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

#endif

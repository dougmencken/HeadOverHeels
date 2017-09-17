// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Hunter_hpp_
#define Hunter_hpp_

#include "Behavior.hpp"
#include "HPC.hpp"


namespace isomot
{

class Item ;
class ItemData ;

/**
 * Try to hunt for player by moving wherever it is
 */

class Hunter : public Behavior
{

public:

        Hunter( Item * item, const std::string & behavior ) ;

        virtual ~Hunter( ) ;

        virtual bool update () ;

private:

       /**
        * Calcula el sentido en el que debe avanzar el elemento para poder cazar al jugador activo
        * @return El estado que indica la dirección calculada
        */
        ActivityOfItem calculateDirection ( const ActivityOfItem & activity ) ;

       /**
        * Calcula el sentido en el que debe avanzar el elemento para poder cazar al jugador activo
        * La dirección está restringida a los puntos cardinales básicos
        * @return El estado que indica la dirección calculada
        */
        ActivityOfItem calculateDirection4 ( const ActivityOfItem & activity ) ;

       /**
        * Calcula el sentido en el que debe avanzar el elemento para poder cazar al jugador activo
        * La dirección puede ser cualquier de las ocho direcciones posibles
        * @return El estado que indica la dirección calculada
        */
        ActivityOfItem calculateDirection8 ( const ActivityOfItem & activity ) ;

       /**
        * Crea para los cazadores ocultos el guarda imperial completo
        * @return true si se creó o false si todavía no se puede crear
        */
        bool createFullBody () ;

       /**
        * Asigna los datos del elemento usado como disparo
        */
        void setMoreData ( void * data ) ;

private:

       /**
        * Cronómetro que controla la velocidad de movimiento del elemento
        */
        HPC * speedTimer ;

       /**
        * Datos del guarda imperial
        */
        ItemData * guardData ;

};

}

#endif

// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Switch_hpp_
#define Switch_hpp_

#include <vector>
#include "Behavior.hpp"
#include "HPC.hpp"


namespace isomot
{

class Item;

/**
 * Mueve al elemento en un único sentido. Cuando éste choca con algo gira 180º y prosigue la
 * marcha en sentido contrario
 */
class Switch : public Behavior
{

public:

       /**
        * Constructor
        * @param item Elemento que tiene este comportamiento
        * @param id Identificador del comportamiento
        */
        Switch( Item * item, const BehaviorId & id ) ;

        virtual ~Switch( ) ;

       /**
        * Actualiza el comportamiento del elemento en cada ciclo
        * @return false si la actualización implica la destrucción del elemento o true en caso contrario
        */
        virtual bool update () ;

private:

       /**
        * Comprueba si hay elementos junto al interruptor
        * @param Un vector donde almacenar el conjunto de elementos
        * @return true si se encontró algún elemento o false en caso contrario
        */
        bool checkSideItems ( std::vector< Item * > & sideItems ) ;

private:

       /**
        * Indica que hay un elemento encima
        */
        bool itemOver ;

       /**
        * Elementos que han accionado el interruptor
        */
        std::vector< Item * > triggerItems ;

};

}

#endif

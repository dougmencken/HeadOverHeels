// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef TransparencyManager_hpp_
#define TransparencyManager_hpp_

#include <vector>


namespace isomot
{

/**
 * Manages transparency of free items
 */

class TransparencyManager
{

public:

       /**
        * Initialize table of transparencies
        */
        TransparencyManager( ) ;

        virtual ~TransparencyManager( ) ;

       /**
        * Añade un nuevo elemento a la tabla
        * @param amount Porcentaje de transparencia del elemento
        */
        void add ( const unsigned char amount ) ;

       /**
        * Elimina un elemento de la tabla
        * @param amount Porcentaje de transparencia del elemento
        */
        void remove ( const unsigned char amount ) ;

       /**
        * Count elements in the table with a given degree of transparency
        * @param Porcentaje de transparencia consultado
        * @return Número de elementos con ese porcentaje de transparencia
        */
        unsigned int countItemsWithDegreeOfTransparency ( unsigned int degree ) const
        {
                return ( degree >= 0 && degree <= 100 ? this->table[ degree ] : 0 ) ;
        }

private:

       /**
        * Table of transparencies
        * It’s a vector of 101 elements where index means degree 0 to 100 of transparency,
        * and value presents the number of items that have this degree in the room
        */
        std::vector < unsigned int > table ;

};

}

#endif

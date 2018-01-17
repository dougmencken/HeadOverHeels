// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
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


namespace isomot
{

class Item ;


class Switch : public Behavior
{

public:

        Switch( Item * item, const std::string & behavior ) ;

        virtual ~Switch( ) ;

        virtual bool update () ;

private:

       /**
        * Comprueba si hay elementos junto al interruptor
        * @param Un vector donde almacenar el conjunto de elementos
        * @return true si se encontró algún elemento o false en caso contrario
        */
        bool checkSideItems ( std::vector< Item * > & sideItems ) ;

private:

        bool isItemAbove ;

       /**
        * Elementos que han accionado el interruptor
        */
        std::vector< Item * > triggerItems ;

};

}

#endif

// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef FallKindOfActivity_hpp_
#define FallKindOfActivity_hpp_

#include <stack>
#include "Ism.hpp"
#include "KindOfActivity.hpp"


namespace isomot
{

class Behavior ;
class FreeItem ;


/**
 * Caída de un elemento
 */

class FallKindOfActivity : public KindOfActivity
{

protected:

        FallKindOfActivity( ) ;

public:

        virtual ~FallKindOfActivity( ) ;

       /**
        * Único objeto de esta clase para toda la aplicación
        * @return Un puntero a la clase madre para hacer uso del polimorfismo de clase
        */
        static KindOfActivity * getInstance () ;

       /**
        * Estado que hace caer a un elemento
        * @param behavior Comportamiento usuario del estado de caída
        * @return true si se produjo la caída o false si hubo colisión
        */
        virtual bool fall ( Behavior * behavior ) ;

private:

        void assignAnchor ( FreeItem * freeItem, std::stack < int > bottomItems ) ;

private:

       /**
        * Único objeto de esta clase para toda la aplicación
        */
        static KindOfActivity * instance ;

};

}

#endif

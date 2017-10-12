// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef DisplaceKindOfActivity_hpp_
#define DisplaceKindOfActivity_hpp_

#include "Ism.hpp"
#include "KindOfActivity.hpp"


namespace isomot
{

class Behavior ;

/**
 * Desplazamiento de un elemento
 */

class DisplaceKindOfActivity : public KindOfActivity
{

protected:

        DisplaceKindOfActivity( ) ;

public:

        virtual ~DisplaceKindOfActivity( ) ;

       /**
        * Único objeto de esta clase para toda la aplicación
        * @return Un puntero a la clase madre para hacer uso del polimorfismo de clase
        */
        static KindOfActivity * getInstance() ;

       /**
        * Estado que desplaza a un elemento
        * @param behavior Comportamiento usuario del estado de desplazamiento
        * @param canFall Indica si el elemento puede caer. Si se dan las condiciones para la caída
        *                se producirá el cambio de estado correspondiente
        * @return true si se produjo el desplazamiento o el cambio de estado; o, false si hubo colisión
        */
        virtual bool displace ( Behavior * behavior, ActivityOfItem * activity, bool canFall ) ;

private:

       /**
        * Único objeto de esta clase para toda la aplicación
        */
        static KindOfActivity * instance ;

};

}

#endif

// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef FireDoughnut_hpp_
#define FireDoughnut_hpp_

#include "Behavior.hpp"
#include "HPC.hpp"


namespace isomot
{

class Item ;
class PlayerItem ;


class FireDoughnut : public Behavior
{

public:

        FireDoughnut( Item * item, const std::string & behavior ) ;

        virtual ~FireDoughnut( ) ;

        virtual bool update () ;

        void setPlayerItem ( PlayerItem * player ) ;

private:

        PlayerItem * playerItem ;

       /**
        * Cronómetro que controla la velocidad de movimiento del elemento
        */
        HPC * speedTimer ;

} ;

}

#endif

// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Doughnut_hpp_
#define Doughnut_hpp_

#include "Behavior.hpp"
#include "Timer.hpp"


namespace isomot
{

class Item ;
class PlayerItem ;


class Doughnut : public Behavior
{

public:

        Doughnut( Item * item, const std::string & behavior ) ;

        virtual ~Doughnut( ) ;

        virtual bool update () ;

        void setPlayerItem ( PlayerItem * player ) {  this->playerItem = player ;  }

private:

        PlayerItem * playerItem ;

       /**
        * Time for speed of movement
        */
        Timer * speedTimer ;

} ;

}

#endif

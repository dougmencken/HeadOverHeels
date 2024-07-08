// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef ShowSlideWithPlanets_hpp_
#define ShowSlideWithPlanets_hpp_

#include "Action.hpp"


namespace gui
{

class ShowSlideWithPlanets : public Action
{

public:

        /**
         * @param notNewGame true for a game in progress or false when a new game begins
         */
        explicit ShowSlideWithPlanets( bool notNewGame );

        virtual ~ShowSlideWithPlanets( ) {}

        void liberateBlacktooth() {  this->blacktoothFree = true ;  }

        void liberateEgyptus() {  this->egyptusFree = true ;  }

        void liberatePenitentiary() {  this->penitentiaryFree = true ;  }

        void liberateByblos() {  this->byblosFree = true ;  }

        void liberateSafari() {  this->safariFree = true ;  }

protected:

        virtual void act () ;

private:

        /**
         * false if a new game begins, true for a game already in progress
         */
        bool gameInProgress ;

        // was the planet Blacktooth liberated
        bool blacktoothFree ;

        // was the planet Egyptus liberated
        bool egyptusFree ;

        // was the planet Penitentiary liberated
        bool penitentiaryFree ;

        // was the planet Byblos liberated
        bool byblosFree ;

        // was the planet Safari liberated
        bool safariFree ;

} ;

}

#endif

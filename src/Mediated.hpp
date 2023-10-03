// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Mediated_hpp_
#define Mediated_hpp_


namespace iso
{

class Mediator ;

class Mediated
{

public:

        Mediated( ) {  this->mediator = 0 ;  }

        virtual ~Mediated( ) { }

protected:

       /**
        * Mediador entre los elementos pertenecientes a una sala
        */
        Mediator * mediator ;

public:

       /**
        * Establece el mediador entre los elementos pertenecientes a la sala
        * @param mediator El mediador de la sala
        */
        void setMediator ( Mediator* mediator ) {  this->mediator = mediator ;  }

       /**
        * Mediador entre los elementos pertenecientes a la sala
        * @return mediator El mediador de la sala
        */
        Mediator* getMediator () const {  return this->mediator ;  }

};

}

#endif

// The free and open source remake of Head over Heels
//
// Copyright © 2026 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Mediated_hpp_
#define Mediated_hpp_


class Mediator ;

class Mediated
{

public:

        Mediated( ) : mediator( 0 ) {}

        virtual ~Mediated( ) { }

private:

       /**
        * Intermediary between the elements of a single room
        */
        Mediator * mediator ;

public:

        virtual void setMediator ( Mediator* mediator ) {  this->mediator = mediator ;  }

        Mediator* getMediator () const {  return this->mediator ;  }

} ;

#endif

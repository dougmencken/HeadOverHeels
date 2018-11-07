// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Shady_hpp_
#define Shady_hpp_


namespace iso
{

class Shady
{

public:

        Shady( ) : wantShadow( true )  { }

        Shady( bool wanna ) : wantShadow( wanna )  { }

        virtual ~Shady( ) { }

        bool getWantShadow () const {  return wantShadow ;  }

        void setWantShadow ( bool wanna ) {  wantShadow = wanna ;  }

protected:

        bool wantShadow ;

};

}

#endif

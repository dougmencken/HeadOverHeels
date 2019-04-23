// The free and open source remake of Head over Heels
//
// Copyright © 2019 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef tribool_hpp_
#define tribool_hpp_

const unsigned int trifalse             = 0 ;
const unsigned int tritrue              = 11 ;
const unsigned int indeterminate        = 222 ;

class tribool
{

public:

        tribool( ) : value( indeterminate ) { }
        tribool( unsigned int boo ) : value( boo ) {  if ( value != trifalse && value != tritrue ) value = indeterminate ;  }

        bool isTrue () const {  return value == tritrue ;  }
        bool isFalse () const {  return value == trifalse ;  }
        bool isIndeterminate () const {  return value == indeterminate ;  }

        void setTrue () {  value = tritrue ;  }
        void setFalse () {  value = trifalse ;  }
        void setIndeterminate () {  value = indeterminate ;  }

protected:

        unsigned int value ;

} ;

#endif

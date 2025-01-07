// The free and open source remake of Head over Heels
//
// Copyright © 2025 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef AllCreatedActions_h_
#define AllCreatedActions_h_

#include <vector>


namespace gui
{

class Action ;

class AllCreatedActions
{

public :

        static AllCreatedActions & getInstance () {  return AllCreatedActions::instance ;  }

        static void add ( Action * newAction ) ;

        void addAction ( Action * toAdd ) ;

        void binFinishedActions () ;

        void dumpActions () const ;

private :

        static AllCreatedActions instance ;

        AllCreatedActions () {}

        ~AllCreatedActions () ;

        std::vector< Action * > actions ;

} ;

}

#endif

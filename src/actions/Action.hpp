// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Action_hpp_
#define Action_hpp_

#include <string>


namespace gui
{

class Action
{

public:

        Action( ) { }

        virtual ~Action( ) { }

        virtual void doIt () = 0 ;

        virtual std::string getNameOfAction () = 0 ;

};

class DoNothing : public Action
{

public:

        void doIt () {  }

        std::string getNameOfAction ()  {  return "DoNothing" ;  }

};

}

#endif

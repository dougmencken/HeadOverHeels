// The free and open source remake of Head over Heels
//
// Copyright © 2019 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Action_hpp_
#define Action_hpp_

#include <string>

#include "WrappersAllegro.hpp"

#include "Picture.hpp"


namespace gui
{

class Action
{

public:

        Action() : begin( false ), done( false ) { }

        virtual ~Action( ) { }

        void doIt () {  begin = true ;  doAction() ;  done = true ;  }

        virtual std::string getNameOfAction () const = 0 ;

        bool hasBegun() {  return begin ;  }

        bool isDone() {  return done ;  }

protected:

        virtual void doAction () = 0 ;

private:

        bool begin ;

        bool done ;

};

class DoNothing : public Action
{

public:

        std::string getNameOfAction () const {  return "DoNothing" ;  }

protected:

        virtual void doAction () {  }

};

}

#endif

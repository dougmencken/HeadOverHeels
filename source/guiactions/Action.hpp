// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Action_hpp_
#define Action_hpp_

#include <string>
#include <typeinfo>

#include "WrappersAllegro.hpp"

#include "Picture.hpp"


namespace gui
{

class Action
{

public:

        Action() : doing( false ), done( false ) { }

        virtual ~Action( ) { }

        void doIt () {  doing = true ;  act() ;  done = true ;  }

        bool hasBegun() {  return doing ;  }

        bool isDone() {  return done ;  }

        virtual std::string getNameOfAction () const {  return typeid( *this ).name () ;  }

protected:

        virtual void act () = 0 ;

private:

        bool doing ;

        bool done ;

};

class DoNothing : public Action
{

protected:

        virtual void act () {  }

};

}

#endif

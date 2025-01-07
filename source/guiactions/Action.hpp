// The free and open source remake of Head over Heels
//
// Copyright © 2025 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef Action_hpp_
#define Action_hpp_

#include <iostream>
#include <string>
#include <typeinfo>

#include "util.hpp"

#include "AllCreatedActions.hpp"


namespace gui
{

/* abstract */
class Action
{

public :

        Action() : doing( false ), done( false ) {  AllCreatedActions::add( this ) ;  }

        virtual ~Action( ) { }

        void doIt ()
        {
                IF_DEBUG( std::cout << "doing action " << getNameOfAction() << std::endl )

                this->doing = true ;
                act() ;
                this->done = true ;

                IF_DEBUG( std::cout << "done action " << getNameOfAction() << std::endl )

                AllCreatedActions::getInstance().binFinishedActions() ; // delete this ;
        }

        bool hasBegun() const {  return this->doing ;  }

        bool isDone() const {  return this->done ;  }

        virtual std::string getNameOfAction () const {  return util::demangle( typeid( *this ).name () );  }

protected :

        virtual void act () = 0 ;

private :

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

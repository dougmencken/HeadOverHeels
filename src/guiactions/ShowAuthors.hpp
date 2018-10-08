// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef ShowAuthors_hpp_
#define ShowAuthors_hpp_

#include "Action.hpp"


namespace gui
{

class TextField ;


/**
 * Show authors of this game
 */

class ShowAuthors : public Action
{

public:

        ShowAuthors( Picture * picture ) ;

	virtual ~ShowAuthors( ) ;

        std::string getNameOfAction () const {  return "ShowAuthors" ;  }

protected:

        virtual void doAction () ;

private:

        TextField * linesOfCredits ;

        unsigned int initialY ;

        Picture * loadingScreen ;

};

}

#endif

// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef ShowAuthors_hpp_
#define ShowAuthors_hpp_

#include "Action.hpp"
#include "Picture.hpp"


namespace gui
{

class LanguageText ;

class TextField ;


/**
 * Show authors of this game
 */

class ShowAuthors : public Action
{

public:

        ShowAuthors( ) ;

	virtual ~ShowAuthors( ) ;

        virtual std::string getNameOfAction () const {  return "ShowAuthors" ;  }

protected:

        void readCreditsText( const std::string & fileName ) ;

        virtual void doAction () ;

private:

        LanguageText * creditsText ;

        TextField * linesOfCredits ;

        unsigned int initialY ;

        PicturePtr loadingScreen ;

};

}

#endif

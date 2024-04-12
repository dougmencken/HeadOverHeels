// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef CreateLanguageMenu_hpp_
#define CreateLanguageMenu_hpp_

#include <map>
#include "Action.hpp"


namespace gui
{

/**
 * Crea el menú de selección de idioma
 */

class CreateLanguageMenu : public Action
{

public:

        CreateLanguageMenu( ) ;

        ~CreateLanguageMenu( ) ;

        virtual std::string getNameOfAction () const {  return "CreateLanguageMenu" ;  }

protected:

        /**
         * Show the language menu
         */
        virtual void act () ;

private:

        /**
         * Read the list of languages from the XML file
         */
        void readListOfLanguages ( const std::string & nameOfXMLFile ) ;

        std::map < std::string /* iso */, std::string /* text */ > languages ;

};

}

#endif

// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
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

        CreateLanguageMenu( Picture * picture ) ;

        ~CreateLanguageMenu( ) ;

        std::string getNameOfAction () const {  return "CreateLanguageMenu" ;  }

protected:

        /**
         * Show the language menu
         */
        virtual void doAction () ;

private:

        /**
         * Read list of languages for this game
         * @param fileName XML file with list of languages
         */
        void parse ( const std::string& fileName ) ;

        std::map < std::string /* iso */, std::string /* text */ > languages ;

};

}

#endif

// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef CreateLanguageMenu_hpp_
#define CreateLanguageMenu_hpp_

#include <string>
#include <list>
#include <allegro.h>
#include "Action.hpp"
#include "csxml/LanguageXML.hpp"


namespace gui
{

class LanguageText ;

/**
 * Crea el menú de selección de idioma
 */

class CreateLanguageMenu : public Action
{

public:

        /**
         * Constructor
         * @param picture Imagen donde se dibujará la interfaz gráfica
         */
        CreateLanguageMenu( BITMAP* picture ) ;

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

        BITMAP* where ;

        /**
         * Strings for interface, each element contains its unique identifier and string of text
         */
        std::list< LanguageText* > texts ;

};

}

#endif

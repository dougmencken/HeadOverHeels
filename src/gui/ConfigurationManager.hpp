// The free and open source remake of Head over Heels
//
// Copyright © 2017 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef ConfigurationManager_hpp_
#define ConfigurationManager_hpp_

#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <allegro.h>
#include "csxml/ConfigurationXML.hpp"
#include "Ism.hpp"


namespace gui
{

/**
 * Deals with game preferences
 */

class ConfigurationManager
{

public:

        /**
         * Constructor
         * @param fileName Name of XML file with settings of game
         */
        ConfigurationManager( const std::string& fileName ) ;

        void read () ;

        void write () ;

private:

        /**
         * Name of XML file with settings of game
         */
        std::string fileName ;

};

}

#endif

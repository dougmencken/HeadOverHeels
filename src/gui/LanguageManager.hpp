// The free and open source remake of Head over Heels
//
// Copyright © 2023 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef LanguageManager_hpp_
#define LanguageManager_hpp_

#include <string>
#include <vector>


namespace gui
{

class LanguageText ;

/**
 * Read strings of text from XML file and store them in data structure
 */

class LanguageManager
{

public:

        LanguageManager( const std::string & file, const std::string& fileWithGuaranteedStrings ) ;

        ~LanguageManager( );

        LanguageText * findLanguageStringForAlias ( const std::string & alias ) ;

private:

       /**
        * Parse the XML file so each string pairs with its alias
        */
        void parse ( const std::string & file, const std::string& fileWithGuaranteedStrings ) ;

        void parseFile ( const std::string & fileName, std::vector < LanguageText * > & strings ) ;

        std::vector< LanguageText * > strings ;

        std::vector< LanguageText * > backupStrings ;

};

}

#endif

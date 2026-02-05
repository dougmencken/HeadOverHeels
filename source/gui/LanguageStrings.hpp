// The free and open source remake of Head over Heels
//
// Copyright © 2026 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef LanguageStrings_hpp_
#define LanguageStrings_hpp_

#include "LanguageText.hpp"

#include <string>
#include <vector>


namespace gui
{

/**
 * The strings of text from a language XML file
 */

class LanguageStrings
{

public:

        LanguageStrings( const std::string & file )
        {
                makeLanguageStrings( file, file );
        }

        // "fileWithMoreStrings" has more strings than the "file"
        LanguageStrings( const std::string & file, const std::string & fileWithMoreStrings )
        {
                makeLanguageStrings( file, fileWithMoreStrings );
        }

        ~LanguageStrings( );

        const LanguageText & getTranslatedTextByAlias ( const std::string & alias ) ;

private:

        void makeLanguageStrings( const std::string & file, const std::string & fileWithGuaranteedStrings ) ;

        void parse ( const std::string & file, const std::string & fileWithGuaranteedStrings ) ;

        void parseFile ( const std::string & fileName, std::vector < LanguageText * > & strings ) ;

        std::vector< LanguageText * > strings ;

        std::vector< LanguageText * > backupStrings ;

        std::vector< LanguageText * > untranslatedStrings ;

};

}

#endif

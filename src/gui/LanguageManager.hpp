// The free and open source remake of Head over Heels
//
// Copyright © 2018 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef LanguageManager_hpp_
#define LanguageManager_hpp_

#include <string>
#include <list>


namespace gui
{

class LanguageText ;

/**
 * Read strings of text from XML file and store them in data structure
 */

class LanguageManager
{

public:

       /**
        * Constructor
        * @param fileName XML file with localized strings used in the game
        * @param fileWithGuaranteedStrings The file with more strings than in fileName
        */
        LanguageManager( const std::string & fileName, const std::string& fileWithGuaranteedStrings ) ;

        ~LanguageManager( );

        LanguageText * findLanguageString ( const std::string & id ) ;

private:

       /**
        * Parse the XML file so each string pairs with its identifier
        * @param fileName XML file with localized strings used in the game
        * @param fileWithGuaranteedStrings The file with more strings than in fileName
        */
        void parse ( const std::string & fileName, const std::string& fileWithGuaranteedStrings ) ;

        std::list< LanguageText * > texts ;

        std::list< LanguageText * > backupTexts ;

};

/**
 * Object-function used as a predicate in the search for a language string
 */
struct EqualLanguageString : public std::binary_function< LanguageText *, std::string, bool >
{
        bool operator() ( const LanguageText* lang, const std::string& id ) const;
};

}

#endif

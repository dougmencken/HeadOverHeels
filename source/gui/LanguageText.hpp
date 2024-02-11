// The free and open source remake of Head over Heels
//
// Copyright © 2024 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game by Jon Ritman and Bernie Drummond
//
// This program is free software
// You may redistribute it and~or modify it under the terms of the GNU General Public License
// either version 3 of the License or at your option any later version

#ifndef LanguageText_hpp_
#define LanguageText_hpp_

#include <string>
#include <vector>


namespace gui
{

class LanguageLine
{

private:

        std::string text ;

        std::string fontName ;

        std::string color ;

public:

        const std::string & getText() const {  return this->text ;  }

        const std::string & getFontName() const {  return this->fontName ;  }

        const std::string & getColor() const {  return this->color ;  }

        LanguageLine( const std::string & theText )
                : text( theText )
                , fontName( "" )
                , color( "" )
        {}

        LanguageLine( const std::string & theText, const std::string & whichFont, const std::string & whichColor )
                : text( theText )
                , fontName( whichFont )
                , color( whichColor )
        {}

};

class LanguageText
{

private:

        std::string alias ;

        std::vector< LanguageLine > lines ;

public:

        LanguageText( const std::string & theAlias )
                : alias( theAlias )
        {}

        void addLine ( const LanguageLine & line )
        {
                this->lines.push_back( line );
        }

        void addEmptyLine ()
        {
                this->lines.push_back( LanguageLine( "" ) );
        }

        const std::string & getAlias () const {  return this->alias ;  }

        const LanguageLine & getFirstLine () const {  return this->lines[ 0 ] ;  }

        const LanguageLine & getNthLine ( size_t number ) const {  return this->lines[ number ] ;  }

        const std::string & getText () const {  return this->getFirstLine().getText() ;  }

        unsigned int howManyLinesOfText () {  return this->lines.size() ;  }

};

}

#endif

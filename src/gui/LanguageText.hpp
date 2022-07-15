// The free and open source remake of Head over Heels
//
// Copyright © 2022 Douglas Mencken dougmencken@gmail.com
// Copyright © 2008 Jorge Rodríguez Santos
// Original game copyright © 1987 Ocean Software Ltd.
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

struct LanguageLine
{

        std::string text ;

        std::string font ;

        std::string color ;

};

class LanguageText
{

public:

        LanguageText( const std::string& alias ) ;

private:

        std::string alias ;

        std::vector< LanguageLine > lines ;

public:

        void addLine ( const std::string& text ) ;

        void addLine ( const std::string& text, const std::string& font, const std::string& color ) ;

        std::string getAlias () const {  return this->alias ;  }

        std::string getFirstLineText () const {  return this->lines[ 0 ].text ;  }

        std::string getFirstLineFont () const {  return this->lines[ 0 ].font ;  }

        std::string getFirstLineColor () const {  return this->lines[ 0 ].color ;  }

        std::string getText () const {  return this->getFirstLineText() ;  }

        std::string getFont () const {  return this->getFirstLineFont() ;  }

        std::string getColor () const {  return this->getFirstLineColor() ;  }

        LanguageLine* getFirstLine () {  return &( this->lines[ 0 ] ) ;  }

        LanguageLine* getLine ( size_t number ) {  return &( this->lines[ number ] ) ;  }

        size_t getLinesCount () {  return this->lines.size() ;  }

};

}

#endif

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

#include <sstream>


namespace gui
{

class LanguageLine
{

private:

        std::string line ;

        std::string fontName ;

        std::string color ;

public:

        const std::string & getString () const {  return this->line ;  }

        void setString( const std::string & newString ) {  this->line = newString ;  }

        bool isBigHeight () const {  return this->fontName == "big" ;  }

        const std::string & getColor () const {  return this->color ;  }

        LanguageLine( const std::string & theLine )
                : line( theLine )
                , fontName( "" )
                , color( "" )
        {}

        LanguageLine( const std::string & theLine, const std::string & whichFont, const std::string & whichColor )
                : line( theLine )
                , fontName( whichFont )
                , color( whichColor )
        {
                if ( this->fontName == "plain" ) this->fontName = "" ;
        }

        std::string toXml( const std::string & tag = "string" ) const
        {
                std::ostringstream out ;

                out << "<" << tag ;
                if ( ! this->fontName.empty() ) out << " font=\"" << this->fontName << "\"" ;
                if ( ! this->color.empty() ) out << " color=\"" << this->color << "\"" ;
                out << ">" ;

                if ( ! this->line.empty() ) out << this->line ;

                out << "</" << tag << ">" ;

                return out.str ();
        }

} ;


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

        std::string getText () const
        {
                unsigned int howManyLines = howManyLinesOfText() ;

                if ( howManyLines == 0 ) return "" ;
                if ( howManyLines == 1 ) return getFirstLine().getString () ;

                std::ostringstream text ;
                for ( unsigned int n = 0 ; n < howManyLines ; ++ n ) {
                        text << getNthLine( n ).getString() ;
                        if ( ( n + 1 ) < howManyLines ) text << std::endl ;
                }
                return text.str ();
        }

        unsigned int howManyLinesOfText () const {  return this->lines.size() ;  }

        void prefixWith( const std::string & prefix )
        {
                for ( unsigned int i = 0 ; i < this->lines.size() ; ++ i )
                        this->lines[ i ].setString( prefix + this->lines[ i ].getString () );
        }

        std::string toXml( const std::string & tag = "text" ) const
        {
                std::ostringstream out ;

                std::string spacing = "     " ;
                std::string moreSpacing = "   " ;

                out << spacing << "<" << tag ;
                if ( ! this->alias.empty() ) out << " alias=\"" << this->alias << "\"" ;
                out << ">" ;

                unsigned int howManyLines = howManyLinesOfText() ;
                for ( unsigned int l = 0 ; l < howManyLines ; ++ l ) {
                        out << std::endl ;
                        out << spacing << moreSpacing ;
                        out << getNthLine( l ).toXml() ;
                }
                if ( howManyLines > 0 ) out << std::endl ;

                out << spacing << "</" << tag << ">" ;

                return out.str ();
        }

} ;

}

#endif

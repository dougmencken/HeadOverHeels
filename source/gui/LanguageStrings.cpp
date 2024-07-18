
#include "LanguageStrings.hpp"

#include <iostream>
#include <algorithm> // std::for_each

#include <tinyxml2.h>

#include "util.hpp"
#include "ospaths.hpp"

#define DUMP_XML                0
#define DUMP_UPDATE_XML         1


namespace gui
{

LanguageStrings::LanguageStrings( const std::string & file, const std::string & fileWithGuaranteedStrings )
{
        parseFile( file, this->strings );

        if ( file != fileWithGuaranteedStrings ) {
                parseFile( fileWithGuaranteedStrings, this->backupStrings );

                for ( unsigned int i = 0 ; i < backupStrings.size () ; i ++ )
                        backupStrings[ i ]->prefixWith( "_*" + fileWithGuaranteedStrings.substr( 0, 2 ) + "*_ " );

        #if defined( DUMP_UPDATE_XML ) && DUMP_UPDATE_XML
                std::cout << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl << std::endl ;

                const char * linguonym = nilPointer ;
                const char * iso = nilPointer ;

                tinyxml2::XMLDocument translatedStringsXml ;
                tinyxml2::XMLError result = translatedStringsXml.LoadFile( ospaths::pathToFile( ospaths::sharePath() + "text", file ).c_str () );
                if ( result == tinyxml2::XML_SUCCESS ) {
                        tinyxml2::XMLElement * root = translatedStringsXml.FirstChildElement( "language" );
                        linguonym = root->Attribute( "name" );
                        iso = root->Attribute( "iso" );
                }

                std::cout << "<language" ;
                if ( linguonym != nilPointer ) std::cout << " name=\"" << linguonym << "\"" ;
                if ( iso != nilPointer ) std::cout << " iso=\"" << iso << "\"" ;
                std::cout << ">" << std::endl << std::endl ;

                for ( unsigned int i = 0 ; i < this->backupStrings.size () ; ++ i )
                {
                        LanguageText * text = this->backupStrings[ i ];
                        const std::string & alias = text->getAlias() ;

                        for ( unsigned int j = 0 ; j < this->strings.size () ; ++ j )
                                if ( this->strings[ j ]->getAlias() == alias ) {
                                        text = this->strings[ j ];
                                        break ;
                                }

                        std::cout << text->toXml() << std::endl ;
                }

                std::cout << std::endl << "</language>" << std::endl ;
        #endif
        }
}

LanguageStrings::~LanguageStrings()
{
        std::for_each( strings.begin(), strings.end(), DeleteIt() );
        strings.clear();

        std::for_each( backupStrings.begin(), backupStrings.end(), DeleteIt() );
        backupStrings.clear();
}

LanguageText* LanguageStrings::getTranslatedTextByAlias( const std::string & alias ) const
{
        for ( size_t i = 0 ; i < strings.size () ; i ++ )
                if ( strings[ i ]->getAlias() == alias ) return strings[ i ];

        for ( size_t i = 0 ; i < backupStrings.size () ; i ++ )
                if ( backupStrings[ i ]->getAlias() == alias ) return backupStrings[ i ] ;

        LanguageText* untranslated = new LanguageText( alias ) ;
        untranslated->addLine( "(" + alias + ")" );
        return untranslated ;
}

void LanguageStrings::parseFile( const std::string & fileName, std::vector< LanguageText * > & strings )
{
        std::string filePath = ospaths::pathToFile( ospaths::sharePath() + "text", fileName );
        std::cout << "parsing \"" << fileName << "\" (" << filePath << ")" << std::endl ;

        tinyxml2::XMLDocument translatedStringsXml ;
        tinyxml2::XMLError result = translatedStringsXml.LoadFile( filePath.c_str () );
        if ( result != tinyxml2::XML_SUCCESS ) {
                std::cerr << "can’t parse language strings file \"" << fileName << "\" (" << filePath << ")" << std::endl ;
                return ;
        }

        tinyxml2::XMLElement* root = translatedStringsXml.FirstChildElement( "language" );

        #if defined( DUMP_XML ) && DUMP_XML
                std::cout << "<language" ;
                const char * linguonym = root->Attribute( "name" );
                if ( linguonym != nilPointer ) std::cout << " name=\"" << linguonym << "\"" ;
                const char * iso = root->Attribute( "iso" );
                if ( iso != nilPointer ) std::cout << " iso=\"" << iso << "\"" ;
                std::cout << ">" << std::endl << std::endl ;
        #endif

        for ( tinyxml2::XMLElement* text = root->FirstChildElement( "text" ) ;
                        text != nilPointer ;
                                text = text->NextSiblingElement( "text" ) )
        {
                std::string alias = text->Attribute( "alias" );
                LanguageText* langText = new LanguageText( alias );

        tinyxml2::XMLElement* properties = text->FirstChildElement( "properties" );

        if ( properties != nilPointer ) // the old and obsolete format
        {
                while ( properties != nilPointer )
                {
                        const char * font = properties->Attribute( "font" );
                        const char * color = properties->Attribute( "color" );

                        for ( tinyxml2::XMLElement* string = properties->FirstChildElement( "string" ) ;
                                        string != nilPointer ;
                                                string = string->NextSiblingElement( "string" ) )
                        {
                                if ( string->FirstChild() != nilPointer )
                                        langText->addLine( LanguageLine( string->FirstChild()->ToText()->Value(),
                                                                           font != nilPointer ? font : "", color != nilPointer ? color : "" ) );
                                else
                                        langText->addEmptyLine() ;
                        }

                        properties = properties->NextSiblingElement( "properties" );
                }
        }
        else // the new format
        {
                for ( tinyxml2::XMLElement* string = text->FirstChildElement( "string" ) ;
                                string != nilPointer ;
                                        string = string->NextSiblingElement( "string" ) )
                {
                        const char * font = string->Attribute( "font" );
                        const char * color = string->Attribute( "color" );

                        if ( string->FirstChild() != nilPointer )
                                langText->addLine( LanguageLine( string->FirstChild()->ToText()->Value(),
                                                                   font != nilPointer ? font : "", color != nilPointer ? color : "" ) );
                        else
                                langText->addEmptyLine() ;
                }
        }
                #if defined( DUMP_XML ) && DUMP_XML
                        std::cout << langText->toXml() << std::endl ;
                #endif

                strings.push_back( langText );
        }
        #if defined( DUMP_XML ) && DUMP_XML
                std::cout << std::endl << "</language>" << std::endl ;
        #endif
}

}

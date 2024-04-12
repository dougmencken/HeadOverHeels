
#include "LanguageStrings.hpp"

#include "LanguageText.hpp"

#include <iostream>
#include <algorithm> // std::for_each

#include <tinyxml2.h>

#include "util.hpp"

#define DUMP_XML        0


namespace gui
{

LanguageStrings::LanguageStrings( const std::string & file, const std::string & fileWithGuaranteedStrings )
{
        parseFile( file, this->strings );

        if ( file != fileWithGuaranteedStrings ) // file is not the same as
        // 
                parseFile( fileWithGuaranteedStrings, this->backupStrings );
}

LanguageStrings::~LanguageStrings()
{
        std::for_each( strings.begin(), strings.end(), DeleteIt() );
        strings.clear();

        std::for_each( backupStrings.begin(), backupStrings.end(), DeleteIt() );
        backupStrings.clear();
}

LanguageText* LanguageStrings::findLanguageStringForAlias( const std::string & alias )
{
        for ( size_t i = 0 ; i < strings.size () ; i ++ )
        {
                if ( strings[ i ]->getAlias() == alias ) return strings[ i ];
        }

        for ( size_t i = 0 ; i < backupStrings.size () ; i ++ )
        {
                if ( backupStrings[ i ]->getAlias() == alias ) return backupStrings[ i ];
        }

        LanguageText* translated = new LanguageText( alias ) ;
        translated->addLine( "(" + alias + ")" );
        return translated ;
}

void LanguageStrings::parseFile( const std::string & fileName, std::vector< LanguageText * > & strings )
{
        std::cout << "parsing \"" << fileName << "\"" << std::endl ;

        tinyxml2::XMLDocument languageXml ;
        tinyxml2::XMLError result = languageXml.LoadFile( fileName.c_str () );
        if ( result != tinyxml2::XML_SUCCESS )
        {
                return ;
        }

        tinyxml2::XMLElement* root = languageXml.FirstChildElement( "language" );
        #if defined( DUMP_XML ) && DUMP_XML
                std::cout << "   <language>" << std::endl ;
        #endif

        for ( tinyxml2::XMLElement* text = root->FirstChildElement( "text" ) ;
                        text != nilPointer ;
                                text = text->NextSiblingElement( "text" ) )
        {
                std::string alias = text->Attribute( "alias" );
                LanguageText* langText = new LanguageText( alias );

                #if defined( DUMP_XML ) && DUMP_XML
                        std::cout << "     <text alias=\"" << alias << "\">" << std::endl ;
                #endif

                for ( tinyxml2::XMLElement* properties = text->FirstChildElement( "properties" ) ;
                                properties != nilPointer ;
                                        properties = properties->NextSiblingElement( "properties" ) )
                {
                        const char * font = properties->Attribute( "font" );
                        const char * color = properties->Attribute( "color" );

                        #if defined( DUMP_XML ) && DUMP_XML
                                std::cout << "       <properties" <<
                                                ( font != nilPointer ? std::string( " font=\"" ) + font + "\"" : "" ) <<
                                                ( color != nilPointer ? std::string( " color=\"" ) + color + "\"" : "" ) <<
                                                ">" << std::endl ;
                        #endif

                        for ( tinyxml2::XMLElement* string = properties->FirstChildElement( "string" ) ;
                                        string != nilPointer ;
                                                string = string->NextSiblingElement( "string" ) )
                        {
                                if ( string->FirstChild() != nilPointer )
                                {
                                #if defined( DUMP_XML ) && DUMP_XML
                                        std::cout << "         <string>" << string->FirstChild()->ToText()->Value() << "</string>" << std::endl ;
                                #endif
                                        langText->addLine( LanguageLine( string->FirstChild()->ToText()->Value(),
                                                                           font != nilPointer ? font : "", color != nilPointer ? color : "" ) );
                                }
                                else
                                {
                                #if defined( DUMP_XML ) && DUMP_XML
                                        std::cout << "         <string></string>" << std::endl ;
                                #endif
                                        langText->addEmptyLine() ;
                                }
                        }
                        #if defined( DUMP_XML ) && DUMP_XML
                                std::cout << "       </properties>" << std::endl ;
                        #endif
                }
                #if defined( DUMP_XML ) && DUMP_XML
                        std::cout << "     </text>" << std::endl ;
                #endif

                strings.push_back( langText );
        }
        #if defined( DUMP_XML ) && DUMP_XML
                std::cout << "   </language>" << std::endl ;
        #endif
}

}

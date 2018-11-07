
#include "LanguageText.hpp"
#include "LanguageManager.hpp"
#include "Ism.hpp"

#include <iostream>
#include <algorithm> // std::for_each
#include <tinyxml2.h>

#if defined( DEBUG ) && DEBUG
    # define DEBUG_XML          0
#endif


namespace gui
{

LanguageManager::LanguageManager( const std::string& file, const std::string& fileWithGuaranteedStrings )
{
        this->parse( file, fileWithGuaranteedStrings );
}

LanguageManager::~LanguageManager()
{
        std::for_each( strings.begin(), strings.end(), iso::DeleteIt() );
        strings.clear();

        std::for_each( backupStrings.begin(), backupStrings.end(), iso::DeleteIt() );
        backupStrings.clear();
}

LanguageText* LanguageManager::findLanguageStringForAlias( const std::string& alias )
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

void LanguageManager::parse( const std::string& file, const std::string& fileWithGuaranteedStrings )
{
        parseFile( file, this->strings );

        if ( file != fileWithGuaranteedStrings ) // file is not the same as backup file with more strings
        {
                parseFile( fileWithGuaranteedStrings, this->backupStrings );
        }
}

void LanguageManager::parseFile( const std::string& fileName, std::vector< LanguageText * >& strings )
{
        std::cout << "parsing \"" << fileName << "\"" << std::endl ;

        tinyxml2::XMLDocument languageXml;
        tinyxml2::XMLError result = languageXml.LoadFile( fileName.c_str () );
        if ( result != tinyxml2::XML_SUCCESS )
        {
                return;
        }

        tinyxml2::XMLElement* root = languageXml.FirstChildElement( "language" );
        #if defined( DEBUG_XML ) && DEBUG_XML
                std::cout << "   <language>" << std::endl ;
        #endif

        for ( tinyxml2::XMLElement* text = root->FirstChildElement( "text" ) ;
                        text != nilPointer ;
                        text = text->NextSiblingElement( "text" ) )
        {
                std::string alias = text->Attribute( "alias" );
                LanguageText* langText = new LanguageText( alias );

                #if defined( DEBUG_XML ) && DEBUG_XML
                        std::cout << "     <text alias=\"" << alias << "\">" << std::endl ;
                #endif

                for ( tinyxml2::XMLElement* properties = text->FirstChildElement( "properties" ) ;
                                properties != nilPointer ;
                                properties = properties->NextSiblingElement( "properties" ) )
                {
                        const char * font = properties->Attribute( "font" );
                        const char * color = properties->Attribute( "color" );

                        #if defined( DEBUG_XML ) && DEBUG_XML
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
                                #if defined( DEBUG_XML ) && DEBUG_XML
                                        std::cout << "         <string>" << string->FirstChild()->ToText()->Value() << "</string>" << std::endl ;
                                #endif
                                        langText->addLine( string->FirstChild()->ToText()->Value(),
                                                           font != nilPointer ? font : "", color != nilPointer ? color : "" );
                                }
                                else
                                {
                                #if defined( DEBUG_XML ) && DEBUG_XML
                                        std::cout << "         <string></string>" << std::endl ;
                                #endif
                                        langText->addLine( "" );
                                }
                        }
                        #if defined( DEBUG_XML ) && DEBUG_XML
                                std::cout << "       </properties>" << std::endl ;
                        #endif
                }
                #if defined( DEBUG_XML ) && DEBUG_XML
                        std::cout << "     </text>" << std::endl ;
                #endif

                strings.push_back( langText );
        }
        #if defined( DEBUG_XML ) && DEBUG_XML
                std::cout << "   </language>" << std::endl ;
        #endif
}

}

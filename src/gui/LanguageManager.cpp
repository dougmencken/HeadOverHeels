
#include "LanguageManager.hpp"
#include "Gui.hpp"


namespace gui
{

LanguageManager::LanguageManager( const std::string& fileName, const std::string& fileWithGuaranteedStrings )
{
        this->parse( fileName, fileWithGuaranteedStrings );
}

LanguageManager::~LanguageManager()
{
        std::for_each( texts.begin(), texts.end(), DeleteObject() );
        texts.clear();
}

LanguageText* LanguageManager::findLanguageString( const std::string& id )
{
        std::list< LanguageText * >::iterator i = std::find_if( texts.begin (), texts.end (), std::bind2nd( EqualLanguageString(), id ) );
        if ( i == texts.end () )
        {
                i = std::find_if( backupTexts.begin (), backupTexts.end (), std::bind2nd( EqualLanguageString(), id ) );
                if ( i != backupTexts.end () )
                {
                        return *i ;
                }
                else
                {
                        LanguageText* text = new LanguageText( id ) ;
                        text->addLine( "?notfound" );
                        return text ;
                }
        }

        return *i ;
}

void LanguageManager::parse( const std::string& fileName, const std::string& fileWithGuaranteedStrings )
{
        // Carga el archivo XML especificado y almacena los datos XML en la lista
        try
        {
                std::auto_ptr< lxml::LanguageXML > languageXML( lxml::language( fileName.c_str() ) );

                // Almacena todos los registros del archivo en la lista
                for ( lxml::LanguageXML::text_const_iterator t = languageXML->text().begin (); t != languageXML->text().end (); ++t )
                {
                        LanguageText* lang = new LanguageText( ( *t ).id (), ( *t ).x (), ( *t ).y () );

                        for ( lxml::text::properties_const_iterator p = ( *t ).properties().begin (); p != ( *t ).properties().end (); ++p )
                        {
                                std::string font;
                                if ( ( *p ).font().present() )
                                {
                                        font = ( *p ).font().get();
                                }

                                std::string color;
                                if ( ( *p ).color().present() )
                                {
                                        color = ( *p ).color().get();
                                }

                                for ( lxml::properties::ustring_const_iterator u = ( *p ).ustring().begin (); u != ( *p ).ustring().end (); ++u )
                                {
                                        lang->addLine( *u, font, color );
                                }
                        }

                        this->texts.push_back( lang );
                }

                if ( fileName.compare( fileWithGuaranteedStrings ) != 0 )
                { // file is not the same as backup file with more strings
                        std::auto_ptr< lxml::LanguageXML > backupLanguageXML( lxml::language( fileWithGuaranteedStrings.c_str() ) );

                        for ( lxml::LanguageXML::text_const_iterator t = backupLanguageXML->text().begin (); t != backupLanguageXML->text().end (); ++t )
                        {
                                LanguageText* lang = new LanguageText( ( *t ).id (), ( *t ).x (), ( *t ).y () );

                                for ( lxml::text::properties_const_iterator p = ( *t ).properties().begin (); p != ( *t ).properties().end (); ++p )
                                {
                                        std::string font;
                                        if ( ( *p ).font().present() )
                                        {
                                                font = ( *p ).font().get();
                                        }

                                        std::string color;
                                        if ( ( *p ).color().present() )
                                        {
                                                color = ( *p ).color().get();
                                        }

                                        for ( lxml::properties::ustring_const_iterator u = ( *p ).ustring().begin (); u != ( *p ).ustring().end (); ++u )
                                        {
                                                lang->addLine( *u, font, color );
                                        }
                                }

                                this->backupTexts.push_back( lang );
                        }
                }
        }
        catch ( const xml_schema::exception& e )
        {
                std::cerr << e << std::endl ;
        }
}

LanguageText::LanguageText( const std::string& id, unsigned int x, unsigned int y ) :
        id( id ) ,
        x( x ) ,
        y( y )
{

}

LanguageText::LanguageText( const std::string& id ) :
        id( id ) ,
        x( 0 ) ,
        y( 0 )
{

}

void LanguageText::addLine( const std::string& text )
{
        LanguageLine line;
        line.text = text;
        line.font = std::string();
        line.color = std::string();
        lines.push_back( line );
}

void LanguageText::addLine( const std::string& text, const std::string& font, const std::string& color )
{
        LanguageLine line;
        line.text = text;
        line.font = font;
        line.color = color;
        lines.push_back( line );
}

inline bool EqualLanguageString::operator() ( const LanguageText* lang, const std::string& id ) const
{
        return ( lang->getId().compare( id ) == 0 );
}

}

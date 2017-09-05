
#include "CreateLanguageMenu.hpp"
#include "Ism.hpp"
#include "GuiManager.hpp"
#include "LanguageManager.hpp"
#include "ConfigurationManager.hpp"
#include "Font.hpp"
#include "LanguageText.hpp"
#include "Screen.hpp"
#include "MenuWithMultipleColumns.hpp"
#include "Label.hpp"
#include "Icon.hpp"
#include "SelectLanguage.hpp"
#include "CreateMainMenu.hpp"

using gui::CreateLanguageMenu;
using gui::LanguageManager;
using gui::LanguageText;
using gui::SelectLanguage;


CreateLanguageMenu::CreateLanguageMenu( BITMAP* picture )
: Action(),
  where( picture )
{
        // Read list of languages available for this game
        this->parse( isomot::sharePath() + "text/language.xml" );

        language = GuiManager::getInstance()->getConfigurationManager()->getLanguage();
        if ( language.compare( "en_UK" ) == 0 )
        { // for backwards compatibility
                language = "en_US";
        }
}

CreateLanguageMenu::~CreateLanguageMenu( )
{
        this->texts.clear();
}

void CreateLanguageMenu::doIt ()
{
        Screen* screen = GuiManager::getInstance()->findOrCreateScreenForAction( this, this->where );
        if ( screen->countWidgets() == 0 )
        {
                screen->setEscapeAction( new gui::CreateMainMenu( this->where ) );

                Label* label = 0;

                label = new Label( 200, 24, "Head" );
                label->changeFontAndColor( "big", "yellow" );
                screen->addWidget( label );

                label = new Label( 280, 45, "over", "regular", "multicolor" );
                screen->addWidget( label );

                label = new Label( 360, 24, "Heels" );
                label->changeFontAndColor( "big", "yellow" );
                screen->addWidget( label );

                screen->addIconOfHeadAt( 66, 24 );
                screen->addIconOfHeelsAt( 518, 24 );

                // Presenta los idiomas disponibles
                MenuWithMultipleColumns * menu = new MenuWithMultipleColumns( 40, 120, 300, 11 );
                for ( std::list< LanguageText * >::iterator i = this->texts.begin (); i != this->texts.end (); ++i )
                {
                        label = new Label( ( *i )->getText() );
                        label->setAction( new SelectLanguage( this->where, ( *i )->getId() ) );

                        menu->addOption( label );

                        if ( ( this->language.empty() && i == this->texts.begin() ) || this->language.compare( ( *i )->getId() ) == 0 )
                        {
                                menu->setActiveOption( label );
                        }
                }

                screen->addWidget( menu );
                screen->setKeyHandler( menu );
        }

        GuiManager::getInstance()->changeScreen( screen );
}

void CreateLanguageMenu::parse( const std::string& fileName )
{
        // Carga el archivo XML especificado y almacena los datos XML en la lista
        try
        {
                std::auto_ptr< lxml::LanguageXML > languageXML( lxml::language( fileName.c_str() ) );

                // Almacena todos los registros del archivo en la lista
                for ( lxml::LanguageXML::text_const_iterator t = languageXML->text().begin (); t != languageXML->text().end (); ++t )
                {
                        LanguageText* lang = new LanguageText( ( *t ).id () );

                        for ( lxml::text::properties_const_iterator p = ( *t ).properties().begin (); p != ( *t ).properties().end (); ++p )
                        {
                                for ( lxml::properties::ustring_const_iterator u = ( *p ).ustring().begin (); u != ( *p ).ustring().end (); ++u )
                                {
                                        lang->addLine( *u, std::string(), std::string() );
                                }
                        }

                        this->texts.push_back( lang );
                }
        }
        catch( const xml_schema::exception& e )
        {
                std::cerr << e << std::endl ;
        }
}

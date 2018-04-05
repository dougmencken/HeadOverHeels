
#include "CreateLanguageMenu.hpp"
#include "Ism.hpp"
#include "GuiManager.hpp"
#include "LanguageManager.hpp"
#include "Font.hpp"
#include "Screen.hpp"
#include "MenuWithTwoColumns.hpp"
#include "Label.hpp"
#include "SelectLanguage.hpp"
#include "CreateMainMenu.hpp"

#include <tinyxml2.h>

using gui::CreateLanguageMenu;
using gui::GuiManager;
using gui::LanguageManager;
using gui::SelectLanguage;


CreateLanguageMenu::CreateLanguageMenu( BITMAP* picture )
        : Action( picture )
{
        // read list of languages available for this game
        this->parse( isomot::sharePath() + "text" + pathSeparator + "language.xml" );
}

CreateLanguageMenu::~CreateLanguageMenu( )
{
        this->languages.clear();
}

void CreateLanguageMenu::doAction ()
{
        Screen* screen = GuiManager::getInstance()->findOrCreateScreenForAction( this );
        if ( screen->countWidgets() > 0 )
        {
                screen->freeWidgets();
        }

        screen->setEscapeAction( new gui::CreateMainMenu( getWhereToDraw() ) );

        Label* label = nilPointer;

        label = new Label( "Head", "big", "yellow" );
        label->moveTo( 200, 24 );
        screen->addWidget( label );

        label = new Label( "over", "regular", "multicolor" );
        label->moveTo( 280, 45 );
        screen->addWidget( label );

        label = new Label( "Heels", "big", "yellow" );
        label->moveTo( 360, 24 );
        screen->addWidget( label );

        screen->addPictureOfHeadAt( 66, 24 );
        screen->addPictureOfHeelsAt( 518, 24 );

        // presenta los idiomas disponibles

        std::string language = GuiManager::getInstance()->getLanguage();

        MenuWithTwoColumns * menu = new MenuWithTwoColumns( /* space between columns */ 60 );
        menu->setVerticalOffset( 50 ); // adjust for header over heelser

        for ( std::map< std::string, std::string >::const_iterator it = languages.begin () ; it != languages.end () ; ++ it )
        {
                label = new Label( ( *it ).second );
                label->setAction( new SelectLanguage( getWhereToDraw(), ( *it ).first ) );

                menu->addOption( label );

                if ( ( language.empty() && it == languages.begin() ) || language == ( *it ).first )
                {
                        menu->setActiveOption( label );
                }
        }

        screen->addWidget( menu );
        screen->setKeyHandler( menu );

        Screen::randomPixelFadeIn( Color::blackColor(), screen );
        GuiManager::getInstance()->changeScreen( screen, true );
}

void CreateLanguageMenu::parse( const std::string& fileName )
{
        // read from XML file
        tinyxml2::XMLDocument languages;
        tinyxml2::XMLError result = languages.LoadFile( fileName.c_str () );
        if ( result != tinyxml2::XML_SUCCESS )
        {
                return;
        }

        tinyxml2::XMLElement* root = languages.FirstChildElement( "languages" );

        for ( tinyxml2::XMLElement* language = root->FirstChildElement( "language" ) ;
                        language != nilPointer ;
                        language = language->NextSiblingElement( "language" ) )
        {
                std::string iso = language->Attribute( "iso" );
                std::string text = language->FirstChildElement( "text" )->FirstChild()->ToText()->Value() ;
                this->languages[ iso ] = text;
        }
}

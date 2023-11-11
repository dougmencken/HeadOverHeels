
#include "CreateLanguageMenu.hpp"

#include "GuiManager.hpp"
#include "LanguageManager.hpp"
#include "Font.hpp"
#include "Screen.hpp"
#include "MenuWithTwoColumns.hpp"
#include "Label.hpp"
#include "SelectLanguage.hpp"
#include "CreateMainMenu.hpp"
#include "GamePreferences.hpp"

#include "ospaths.hpp"

#include <tinyxml2.h>

using gui::CreateLanguageMenu;
using gui::GuiManager;
using gui::LanguageManager;
using gui::SelectLanguage;


CreateLanguageMenu::CreateLanguageMenu( ) : Action( )
{
        // read list of languages available for this game
        parse( ospaths::sharePath() + "text" + ospaths::pathSeparator() + "language.xml" );
}

CreateLanguageMenu::~CreateLanguageMenu( )
{
        languages.clear();
}

void CreateLanguageMenu::doAction ()
{
        ScreenPtr screenptr = GuiManager::getInstance().findOrCreateScreenForAction( this );
        if ( screenptr == nilPointer ) {
                std::cout << "nil screen for " << this->getNameOfAction() ;
                return ;
        }
        Screen & screen = *screenptr ;

        if ( screen.countWidgets() > 0 )
        {
                screen.freeWidgets();
        }

        screen.setEscapeAction( new gui::CreateMainMenu() );

        const unsigned int screenWidth = GamePreferences::getScreenWidth();
        const unsigned int space = ( screenWidth / 20 ) - 10;

        Label* Head = new Label( "Head", "big", "yellow" );
        Label* over = new Label( "over", "plain", "multicolor" );
        Label* Heels = new Label( "Heels", "big", "yellow" );

        over->moveTo( ( screenWidth - over->getWidth() - 20 ) >> 1, space + Head->getHeight() - over->getHeight() - 8 );
        screen.addWidget( over );

        unsigned int widthOfSpace = over->getFont().getWidthOfLetter ( " " );
        int spaceWithoutSpacing = widthOfSpace - over->getSpacing ();

        Head->moveTo( over->getX() - Head->getWidth() - spaceWithoutSpacing, space );
        screen.addWidget( Head );

        Heels->moveTo( over->getX() + over->getWidth() + spaceWithoutSpacing, space );
        screen.addWidget( Heels );

        const unsigned int headHeelsWidth = 48;
        screen.addPictureOfHeadAt( Head->getX() - ( headHeelsWidth << 1 ) - space, space + 5 );
        screen.addPictureOfHeelsAt( Heels->getX() + Heels->getWidth() + headHeelsWidth + space, space + 5 );

        // presenta los idiomas disponibles

        std::string language = GuiManager::getInstance().getLanguage();

        MenuWithTwoColumns * menu = new MenuWithTwoColumns( /* space between columns */ ( screenWidth >> 3 ) - 20 );
        menu->setVerticalOffset( 50 ); // adjust for header over heelser

        for ( std::map< std::string, std::string >::const_iterator it = languages.begin () ; it != languages.end () ; ++ it )
        {
                Label* tongue = new Label( ( *it ).second );
                tongue->setAction( new SelectLanguage( ( *it ).first ) );

                menu->addOption( tongue );

                if ( ( language.empty() && it == languages.begin() ) || language == ( *it ).first )
                {
                        menu->setActiveOption( tongue );
                }
        }

        screen.addWidget( menu );
        screen.setKeyHandler( menu );

        Screen::randomPixelFadeIn( Color::blackColor(), screen );
        GuiManager::getInstance().changeScreen( screen, true );
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


#include "CreateLanguageMenu.hpp"

#include "GuiManager.hpp"
#include "LanguageStrings.hpp"
#include "Font.hpp"
#include "Screen.hpp"
#include "MenuWithTwoColumns.hpp"
#include "Label.hpp"
#include "ChooseLanguage.hpp"
#include "CreateMainMenu.hpp"
#include "GamePreferences.hpp"

#include "ospaths.hpp"

#include <tinyxml2.h>


namespace gui {

CreateLanguageMenu::CreateLanguageMenu( ) : Action( )
{
        readListOfLanguages( ospaths::sharePath() + "text" + ospaths::pathSeparator() + "language.xml" );
}

CreateLanguageMenu::~CreateLanguageMenu( )
{
        languages.clear();
}

void CreateLanguageMenu::act ()
{
        Screen & screen = * GuiManager::getInstance().findOrCreateScreenForAction( *this );

        if ( ! screen.isNewAndEmpty() ) screen.freeWidgets ();

        screen.setEscapeAction( new CreateMainMenu() );

        const unsigned int screenWidth = GamePreferences::getScreenWidth();
        const unsigned int space = ( screenWidth / 20 ) - 10;

        Label* Head = new Label( "Head", Font::fontWith2xHeightAndColor( "yellow" ) );
        Label* over = new Label( "over", Font::fontWithColor( "" ), /* multicolor */ true );
        Label* Heels = new Label( "Heels", Font::fontWith2xHeightAndColor( "yellow" ) );

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

        // present the language menu

        std::string chosenLanguage = GuiManager::getInstance().getLanguage() ;

        MenuWithTwoColumns * menu = new MenuWithTwoColumns( /* between columns */ ( screenWidth >> 3 ) - 20 );
        menu->setVerticalOffset( 50 ); // adjust for header over heelser

        for ( std::map< std::string, std::string >::const_iterator it = languages.begin () ; it != languages.end () ; ++ it )
        {
                Label* tongue = new Label( ( *it ).second );
                tongue->setAction( new ChooseLanguage( ( *it ).first ) );

                menu->addOption( tongue );

                if ( chosenLanguage == ( *it ).first || ( chosenLanguage.empty() && it == languages.begin() ) )
                        menu->setActiveOption( ( *it ).second );
        }

        screen.addWidget( menu );
        screen.setNextKeyHandler( menu );

        GuiManager::getInstance().changeScreen( screen, true );
}

void CreateLanguageMenu::readListOfLanguages( const std::string & nameOfXMLFile )
{
        tinyxml2::XMLDocument languages ;
        tinyxml2::XMLError result = languages.LoadFile( nameOfXMLFile.c_str () );
        if ( result != tinyxml2::XML_SUCCESS ) return ;

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

}

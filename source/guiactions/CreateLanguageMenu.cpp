
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

#include <dirent.h> // opendir, readdir, closedir


namespace gui {

CreateLanguageMenu::CreateLanguageMenu( ) : Action( )
{
        CreateLanguageMenu::makeMapOfLanguages( ospaths::sharePath() + "text", this->languages );
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
                        menu->setActiveOptionByText( ( *it ).second );
        }

        screen.addWidget( menu );
        screen.setNextKeyHandler( menu );

        GuiManager::getInstance().changeScreen( screen, true );
}

/* public static */
void CreateLanguageMenu::makeMapOfLanguages( const std::string & languagesFolder, std::map < std::string, std::string > & languages )
{
        DIR * dir = opendir( languagesFolder.c_str () );

        struct dirent * entry ;
        while ( ( entry = readdir( dir ) ) != nilPointer ) {
                std::string nameOfEntry = entry->d_name ;
                if ( util::stringEndsWith( nameOfEntry, ".xml" ) )
                {
                        tinyxml2::XMLDocument tongue ;
                        tinyxml2::XMLError result = tongue.LoadFile( ospaths::pathToFile( languagesFolder, nameOfEntry ).c_str () );
                        if ( result != tinyxml2::XML_SUCCESS ) continue ;

                        tinyxml2::XMLElement * language = tongue.FirstChildElement( "language" );
                        if ( language == nilPointer ) continue ;

                        const char * linguonym = language->Attribute( "name" );
                        const char * iso = language->Attribute( "iso" );
                        if ( linguonym != nilPointer && iso != nilPointer )
                                languages[ iso ] = linguonym ;
                        else {
                                std::string nameWithoutXmlSuffix = nameOfEntry.substr( 0, nameOfEntry.size() - 4 );
                                if ( linguonym != nilPointer )
                                        languages[ nameWithoutXmlSuffix ] = linguonym ;
                                else
                                        languages[ nameWithoutXmlSuffix ] = nameWithoutXmlSuffix ;
                        }
                }
        }

        closedir( dir );
}

}

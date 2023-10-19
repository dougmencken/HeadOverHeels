
#include "ShowAuthors.hpp"

#include "ospaths.hpp"
#include "sleep.hpp"
#include "screen.hpp"

#include "GuiManager.hpp"
#include "LanguageText.hpp"
#include "SoundManager.hpp"

#include "Font.hpp"
#include "Screen.hpp"
#include "Label.hpp"
#include "PictureWidget.hpp"
#include "TextField.hpp"
#include "CreateMainMenu.hpp"

#include <tinyxml2.h>

using gui::ShowAuthors ;


ShowAuthors::ShowAuthors( )
        : Action( )
        , creditsText( nilPointer )
        , linesOfCredits( nilPointer )
        , initialY( 0 )
        , loadingScreen( )
{
        std::string pathToText = ospaths::sharePath() + "text" + ospaths::pathSeparator() ;
        this->readCreditsText( pathToText + "credits.xml" );
}

ShowAuthors::~ShowAuthors( )
{
        delete creditsText ;
        delete linesOfCredits ;
}

void ShowAuthors::readCreditsText( const std::string & fileName )
{
        std::cout << "reading the credits from \"" << fileName << "\"" << std::endl ;

        tinyxml2::XMLDocument creditsXml ;
        tinyxml2::XMLError result = creditsXml.LoadFile( fileName.c_str () );
        if ( result != tinyxml2::XML_SUCCESS ) {
                std::cerr << "can't read XML file \"" << fileName << "\"" << std::endl ;

                this->creditsText = new LanguageText( "credits" ) ;
                this->creditsText->addLine( "and where is my credits.xml ?", "big", "white" );
                return ;
        }

        tinyxml2::XMLElement* root = creditsXml.FirstChildElement( "credits" );

        for ( tinyxml2::XMLElement* text = root->FirstChildElement( "text" ) ;
                        text != nilPointer ;
                                text = text->NextSiblingElement( "text" ) )
        {
                std::string alias = text->Attribute( "alias" );
                this->creditsText = new LanguageText( alias );

                for ( tinyxml2::XMLElement* properties = text->FirstChildElement( "properties" ) ;
                                properties != nilPointer ;
                                        properties = properties->NextSiblingElement( "properties" ) )
                {
                        const char * font = properties->Attribute( "font" );
                        const char * color = properties->Attribute( "color" );

                        for ( tinyxml2::XMLElement* string = properties->FirstChildElement( "string" ) ;
                                        string != nilPointer ;
                                                string = string->NextSiblingElement( "string" ) )
                        {
                                if ( string->FirstChild() != nilPointer )
                                {
                                        this->creditsText->addLine( string->FirstChild()->ToText()->Value(),
                                                                        font != nilPointer ? font : "",
                                                                        color != nilPointer ? color : "" );
                                } else
                                {
                                        this->creditsText->addLine( "" );
                                }
                        }
                }
        }
}

void ShowAuthors::doAction ()
{
        SoundManager::getInstance().playOgg( "music/CreditsTheme.ogg", /* loop */ true );

        Screen& screen = * GuiManager::getInstance().findOrCreateScreenForAction( this );
        if ( screen.countWidgets() == 0 )
        {
                if ( this->linesOfCredits != nilPointer ) delete this->linesOfCredits ;
                this->linesOfCredits = new TextField( variables::getScreenWidth(), "center" );

                this->initialY = screen.getImageOfScreen().getHeight() ;
                this->linesOfCredits->moveTo( 0, initialY );

                size_t howManyLines = this->creditsText->getLinesCount() ;
                std::cout << "credits-text has " << howManyLines << " lines" << std::endl ;

                for ( size_t i = 0; i < howManyLines; i++ )
                {
                        LanguageLine* line = this->creditsText->getLine( i );
                        linesOfCredits->addLine( line->text, line->font, line->color );
                }

                screen.addWidget( this->linesOfCredits );
        }
        else {
                // restore the initial position
                this->linesOfCredits->moveTo( this->linesOfCredits->getX(), initialY );
        }

        screen.setEscapeAction( new CreateMainMenu() );

        GuiManager::getInstance().changeScreen( screen, true );

        const unsigned int widthOfSlide = screen.getImageOfScreen().getWidth() ;
        const unsigned int heightOfSlide = screen.getImageOfScreen().getHeight() ;

        const unsigned int heightOfCredits = ( ( this->linesOfCredits->getHeightOfField() + 1 ) >> 1 ) << 1;
        const unsigned int verticalSpace = ( heightOfSlide * 3 ) >> 2;
        const int whenToReloop = - ( heightOfCredits + verticalSpace ) ;

        std::cout << "height of credits-text is " << heightOfCredits << std::endl ;

        PictureWidget* widgetForLoadingScreen = nilPointer;

        // move text up

        while ( ! screen.getEscapeAction()->hasBegun() )
        {
                int yNow = linesOfCredits->getY() - 1;

                if ( allegro::isKeyPushed( "Space" ) && allegro::isShiftKeyPushed() )
                {
                        if ( allegro::isAltKeyPushed() )
                                yNow += 2 ;
                        else
                                yNow ++ ;
                }

                if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "Left" ) )
                {
                        linesOfCredits->moveTo( linesOfCredits->getX () - 1, linesOfCredits->getY () );
                }
                if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "Right" ) )
                {
                        linesOfCredits->moveTo( linesOfCredits->getX () + 1, linesOfCredits->getY () );
                }
                if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "Pad 0" ) )
                {
                        linesOfCredits->moveTo( 0, linesOfCredits->getY () );
                }

                if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "l" ) )
                {
                        linesOfCredits->setAlignment( "left" );
                }
                if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "c" ) )
                {
                        linesOfCredits->setAlignment( "center" );
                }
                if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "r" ) )
                {
                        linesOfCredits->setAlignment( "right" );
                }

                if ( yNow <= whenToReloop )
                {
                        // loop it
                        yNow = initialY ;
                }

                linesOfCredits->moveTo( linesOfCredits->getX(), yNow );

                if ( yNow == static_cast< int >( heightOfSlide ) - static_cast< int >( heightOfCredits )
                                && widgetForLoadingScreen == nilPointer )
                {
                        if ( loadingScreen == nilPointer )
                        {
                                autouniqueptr< allegro::Pict > png( allegro::Pict::fromPNGFile (
                                        ospaths::pathToFile( ospaths::sharePath(), "loading-screen.png" )
                                ) );
                                loadingScreen = PicturePtr( new Picture( * png ) );
                                loadingScreen->setName( "image of loading screen from original speccy version" );
                        }

                        if ( loadingScreen->getAllegroPict().isNotNil() )
                        {
                                widgetForLoadingScreen = new PictureWidget (
                                                ( widthOfSlide - loadingScreen->getWidth() ) >> 1, heightOfSlide,
                                                loadingScreen,
                                                "loading screen from original speccy version"
                                ) ;
                                screen.addWidget( widgetForLoadingScreen );
                        }
                }
                else if ( yNow < static_cast< int >( heightOfSlide ) - static_cast< int >( heightOfCredits ) && widgetForLoadingScreen != nilPointer )
                {
                        widgetForLoadingScreen->moveTo( widgetForLoadingScreen->getX(), yNow + heightOfCredits );
                }
                else if ( widgetForLoadingScreen != nilPointer )
                {
                        screen.removeWidget( widgetForLoadingScreen );
                        widgetForLoadingScreen = nilPointer;
                }

                GuiManager::getInstance().redraw ();

                if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "f" ) )
                {
                        gui::GuiManager::getInstance().toggleFullScreenVideo ();
                }

                if ( ! allegro::isKeyPushed( "Space" ) )
                {
                        somn::milliSleep( 20 );
                }

                if ( allegro::isKeyPushed( "Escape" ) )
                {
                        allegro::emptyKeyboardBuffer();
                        screen.handleKey( "Escape" );
                }
        }

        if ( widgetForLoadingScreen != nilPointer )
        {
                screen.removeWidget( widgetForLoadingScreen );
        }
}

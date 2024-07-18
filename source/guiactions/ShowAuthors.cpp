
#include "ShowAuthors.hpp"

#include "ospaths.hpp"
#include "sleep.hpp"

#include "GamePreferences.hpp"
#include "GuiManager.hpp"
#include "SoundManager.hpp"

#include "Font.hpp"
#include "Screen.hpp"
#include "Label.hpp"
#include "PictureWidget.hpp"
#include "LanguageText.hpp"
#include "TextField.hpp"
#include "CreateMainMenu.hpp"

#include <tinyxml2.h>

#define REGENERATE_CREDITS_FILE         0

#if defined( REGENERATE_CREDITS_FILE ) && REGENERATE_CREDITS_FILE
#  include <fstream>
#endif

using gui::ShowAuthors ;


ShowAuthors::ShowAuthors( )
        : Action( )
        , creditsText( nilPointer )
        , linesOfCredits( nilPointer )
        , initialX( 0 )
        , initialY( 0 )
        , loadingScreen( )
{
        readCreditsText( "credits.xml" );
}

ShowAuthors::~ShowAuthors( )
{
        delete creditsText ;
        delete linesOfCredits ;
}

void ShowAuthors::readCreditsText( const std::string & fileName )
{
        std::string filePath = ospaths::pathToFile( ospaths::sharePath() + "text", fileName );
        std::cout << "reading the credits from \"" << fileName << "\" (" << filePath << ")" << std::endl ;

        tinyxml2::XMLDocument creditsXml ;
        tinyxml2::XMLError result = creditsXml.LoadFile( filePath.c_str () );
        if ( result != tinyxml2::XML_SUCCESS ) {
                std::cout << "can't read XML file " << filePath << std::endl ;

                this->creditsText = new LanguageText( "absent-credits" ) ;
                this->creditsText->addLine( LanguageLine( "and where is my credits.xml ?", "big", "white" ) );
                return ;
        }

        tinyxml2::XMLElement* root = creditsXml.FirstChildElement( "credits" );

        for ( tinyxml2::XMLElement* text = root->FirstChildElement( "text" ) ;
                        text != nilPointer ;
                                text = text->NextSiblingElement( "text" ) )
        {
                std::string alias = text->Attribute( "alias" );
                this->creditsText = new LanguageText( alias );

                for ( tinyxml2::XMLElement* string = text->FirstChildElement( "string" ) ;
                                string != nilPointer ;
                                        string = string->NextSiblingElement( "string" ) )
                {
                        const char * font = string->Attribute( "font" );
                        const char * color = string->Attribute( "color" );

                        if ( string->FirstChild() != nilPointer )
                                this->creditsText->addLine( LanguageLine ( string->FirstChild()->ToText()->Value(),
                                                                                        font != nilPointer ? font : "",
                                                                                        color != nilPointer ? color : "" ) );
                        else
                                this->creditsText->addEmptyLine() ;
                }

        #if defined( REGENERATE_CREDITS_FILE ) && REGENERATE_CREDITS_FILE
                std::string newFileName = fileName + ".new" ;
                std::string newFilePath = ospaths::pathToFile( ospaths::homePath(), newFileName );
                std::ofstream file( newFilePath );
                if ( file.is_open() ) {
                        file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl << std::endl ;
                        std::string rootTag = "credits" ;
                        file << "<" << rootTag << ">" << std::endl << std::endl ;

                        const std::string & creditsTextXml = this->creditsText->toXml() ;
                        file << creditsTextXml << std::endl ;

                        file << std::endl << "</" << rootTag << ">" << std::endl ;
                        file.close() ;

                        std::cout << "wrote \"" << newFileName << "\" (" << newFilePath << ")" << std::endl ;
                }
        #endif
        }
}

void ShowAuthors::act ()
{
        SoundManager::getInstance().playOgg( "music/CreditsTheme.ogg", /* loop */ true );

        Screen & credits = * GuiManager::getInstance().findOrCreateSlideForAction( getNameOfAction() );

        if ( credits.isNewAndEmpty() )
        {
                if ( this->linesOfCredits != nilPointer ) delete this->linesOfCredits ;

                this->linesOfCredits = new TextField( GamePreferences::getScreenWidth(), "center" );

                this->initialX = this->linesOfCredits->getX() ;
                this->initialY = credits.getImageOfScreen().getHeight() ;
                this->linesOfCredits->moveTo( this->initialX, this->initialY );

                size_t howManyLines = this->creditsText->howManyLinesOfText () ;
                std::cout << "credits-text has " << howManyLines << " lines" << std::endl ;

                for ( size_t n = 0; n < howManyLines; ++ n ) {
                        const LanguageLine & line = this->creditsText->getNthLine( n );
                        linesOfCredits->appendText( line.getString(), line.isBigHeight(), line.getColor() );
                }

                credits.addWidget( this->linesOfCredits );
        }
        else
                // restore the initial position
                this->linesOfCredits->moveTo( this->initialX, this->initialY );

        alignRandom () ;

        credits.setEscapeAction( new CreateMainMenu() );

        GuiManager::getInstance().changeSlide( getNameOfAction(), true );

        const unsigned int widthOfSlide = credits.getImageOfScreen().getWidth() ;
        const unsigned int heightOfSlide = credits.getImageOfScreen().getHeight() ;

        const unsigned int heightOfCredits = ( ( this->linesOfCredits->getHeightOfField() + 1 ) >> 1 ) << 1;
        const unsigned int verticalSpace = ( heightOfSlide * 3 ) >> 2;
        const int whenToReloop = - ( heightOfCredits + verticalSpace ) ;

        std::cout << "height of credits-text is " << heightOfCredits << std::endl ;

        PictureWidget* widgetForLoadingScreen = nilPointer ;

        while ( ! credits.getEscapeAction()->hasBegun() )
        {
                int yNow = linesOfCredits->getY() - 1 ; // move it up

                if ( allegro::isKeyPushed( "b" )
                                || ( allegro::isKeyPushed( "Space" ) && allegro::isShiftKeyPushed() )
                ) {
                        yNow ++ ;

                        if ( allegro::isAltKeyPushed() || allegro::isKeyPushed( "b" ) )
                                yNow ++ ;
                }

                if ( allegro::isShiftKeyPushed() && ( allegro::isKeyPushed( "Left" ) || allegro::isKeyPushed( "Pad 4" ) ) )
                {
                        linesOfCredits->moveTo( linesOfCredits->getX () - 1, linesOfCredits->getY () );
                }
                if ( allegro::isShiftKeyPushed() && ( allegro::isKeyPushed( "Right" ) || allegro::isKeyPushed( "Pad 6" ) ) )
                {
                        linesOfCredits->moveTo( linesOfCredits->getX () + 1, linesOfCredits->getY () );
                }
                if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "Pad 0" ) )
                {
                        linesOfCredits->moveTo( 0, linesOfCredits->getY () );
                }

                if ( allegro::isKeyPushed( "l" ) )
                {
                        alignLeft() ;
                        allegro::releaseKey( "l" );
                }
                if ( allegro::isKeyPushed( "c" ) )
                {
                        alignCenter() ;
                        allegro::releaseKey( "c" );
                }
                if ( allegro::isKeyPushed( "r" ) )
                {
                        alignRight() ;
                        allegro::releaseKey( "r" );
                }

                if ( yNow <= whenToReloop )
                {
                        // loop it
                        yNow = this->initialY ;
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
                                this->loadingScreen = PicturePtr( new Picture( * png ) );
                                this->loadingScreen->setName( "snap of the original speccy version’s tape loading screen" );
                        }

                        if ( loadingScreen->getAllegroPict().isNotNil() )
                        {
                                widgetForLoadingScreen = new PictureWidget (
                                                ( widthOfSlide - loadingScreen->getWidth() ) >> 1, heightOfSlide,
                                                this->loadingScreen,
                                                "PictureWidget for the tape loading screen"
                                ) ;
                                credits.addWidget( widgetForLoadingScreen );
                        }
                }
                else if ( yNow < static_cast< int >( heightOfSlide ) - static_cast< int >( heightOfCredits ) && widgetForLoadingScreen != nilPointer )
                {
                        widgetForLoadingScreen->moveTo( widgetForLoadingScreen->getX(), yNow + heightOfCredits );
                }
                else if ( widgetForLoadingScreen != nilPointer )
                {
                        credits.removeWidget( widgetForLoadingScreen );
                        widgetForLoadingScreen = nilPointer;
                }

                GuiManager::getInstance().redraw ();

                if ( allegro::isAltKeyPushed() && allegro::isShiftKeyPushed() && allegro::isKeyPushed( "f" ) )
                {
                        gui::GuiManager::getInstance().toggleFullScreenVideo ();
                }

                if ( ! allegro::isKeyPushed( "Space" ) && ! allegro::isKeyPushed( "b" ) )
                {
                        somn::milliSleep( 20 );
                }

                if ( allegro::isKeyPushed( "Escape" ) )
                {
                        allegro::emptyKeyboardBuffer();
                        credits.handleKey( "Escape" );
                }
        }

        if ( widgetForLoadingScreen != nilPointer )
                credits.removeWidget( widgetForLoadingScreen );
}

/* private */
void ShowAuthors::alignLeft ()
{
        linesOfCredits->setAlignment( "left" );
        linesOfCredits->moveTo( this->initialX + 100, linesOfCredits->getY () );
        std::cout << "credits are aligned left" << std::endl ;
}

/* private */
void ShowAuthors::alignCenter ()
{
        linesOfCredits->setAlignment( "center" );
        linesOfCredits->moveTo( this->initialX, linesOfCredits->getY () );
        std::cout << "credits are centered" << std::endl ;
}

/* private */
void ShowAuthors::alignRight ()
{
        linesOfCredits->setAlignment( "right" );
        linesOfCredits->moveTo( this->initialX - 100, linesOfCredits->getY () );
        std::cout << "credits are aligned right" << std::endl ;
}

/* private */
void ShowAuthors::alignRandom ()
{
        int random012 = ( rand() % 3 );
             if ( random012 == 0 ) alignCenter() ;
        else if ( random012 == 1 ) alignLeft() ;
        else if ( random012 == 2 ) alignRight() ;
}


#include "CreateGraphicsAreaSizeMenu.hpp"

#include "GamePreferences.hpp"
#include "GuiManager.hpp"
#include "LanguageStrings.hpp"
#include "Label.hpp"
#include "Slide.hpp"

#include "sleep.hpp"


/* public */ /* static */
std::vector< std::pair< unsigned int, unsigned int > > gui::CreateGraphicsAreaSizeMenu::popularScreenSizes ;

void gui::CreateGraphicsAreaSizeMenu::fillSlide ( Slide & slideToFill )
{
        if ( ! slideToFill.isNewAndEmpty() ) return ;

        slideToFill.setEscapeAction( this->actionOnEscape );

        slideToFill.placeHeadAndHeels( /* icons */ false, /* copyrights */ false );

        slideToFill.drawSpectrumColorBoxes( true );

        ////LanguageStrings & languageStrings = gui::GuiManager::getInstance().getOrMakeLanguageStrings() ;

        /////Label* customSize = new Label( languageStrings.getTranslatedTextByAlias( "custom-size" ).getText() );

        //////customSize->setAction( new PickCustomScreenSize() );

        this->menuOfScreenSizes = new Menu( );
        this->menuOfScreenSizes->setVerticalOffset( 40 );

        ///////this->menuOfScreenSizes->addOption( customSize );

        unsigned int howManyPopularScreenSizes = popularScreenSizes.size ();
        for ( unsigned int i = 0 ; i < howManyPopularScreenSizes ; ++ i ) {
                std::string popularSize = util::number2string( popularScreenSizes[ i ].first )
                                                + "×"
                                                + util::number2string( popularScreenSizes[ i ].second ) ;
                this->menuOfScreenSizes->addOption( new Label( popularSize ) );
        }

        slideToFill.addWidget( this->menuOfScreenSizes );
}

void gui::CreateGraphicsAreaSizeMenu::act ()
{
        if ( popularScreenSizes.size () == 0 )
        {       // fill the list of popular screen sizes

                popularScreenSizes.push_back( std::pair< unsigned int, unsigned int >( 640, 480 ) );
                popularScreenSizes.push_back( std::pair< unsigned int, unsigned int >( 800, 600 ) );
                popularScreenSizes.push_back( std::pair< unsigned int, unsigned int >( 1024, 600 ) );
                popularScreenSizes.push_back( std::pair< unsigned int, unsigned int >( 1024, 768 ) );
                popularScreenSizes.push_back( std::pair< unsigned int, unsigned int >( 1280, 720 ) );
                popularScreenSizes.push_back( std::pair< unsigned int, unsigned int >( 1280, 1024 ) );
                popularScreenSizes.push_back( std::pair< unsigned int, unsigned int >( 1366, 768 ) );
                popularScreenSizes.push_back( std::pair< unsigned int, unsigned int >( 1536, 864 ) );
                popularScreenSizes.push_back( std::pair< unsigned int, unsigned int >( 1600, 900 ) );
                popularScreenSizes.push_back( std::pair< unsigned int, unsigned int >( 1920, 1080 ) );
        }

        Slide & slideToPickScreenSize = gui::GuiManager::getInstance().findOrCreateSlideForAction( getNameOfAction() );

        fillSlide( slideToPickScreenSize );

        updateOptions ();

        slideToPickScreenSize.setKeyHandler( this );

        gui::GuiManager::getInstance().changeSlide( getNameOfAction(), true );
}

void gui::CreateGraphicsAreaSizeMenu::handleKey ( const std::string & theKey )
{
        bool doneWithKey = false ;

        if ( theKey == "Enter" || theKey == "Space" )
        {
                std::string chosenSize = this->menuOfScreenSizes->getActiveOption()->getText() ;
                size_t whereX = chosenSize.find( "×" ) ;

                std::string chosenWidth = chosenSize.substr( 0, whereX ) ;
                std::string chosenHeight = chosenSize.substr( whereX + std::string( "×" ).length() );
                unsigned int newWidth = std::atoi( chosenWidth.c_str() );
                unsigned int newHeight = std::atoi( chosenHeight.c_str() );

                if ( GamePreferences::getScreenWidth() != newWidth || GamePreferences::getScreenHeight() != newHeight )
                {
                        GamePreferences::setScreenWidth( newWidth );
                        GamePreferences::setScreenHeight( newHeight );

                        std::cout << "the new size of the game’s graphics area is "
                                        << GamePreferences::getScreenWidth() << "×" << GamePreferences::getScreenHeight()
                                                << std::endl ;

                        gui::GuiManager::getInstance().freeSlides() ;

                        bool inFullScreen = gui::GuiManager::getInstance().isInFullScreen() ;
                        if ( inFullScreen )
                                gui::GuiManager::getInstance().toggleFullScreenVideo ();

                        GamePreferences::allegroWindowSizeToScreenSize () ;

                        if ( inFullScreen )
                                gui::GuiManager::getInstance().toggleFullScreenVideo ();

                        if ( this->actionOnEscape != nilPointer )
                                this->actionOnEscape->doIt ();
                        else
                                this->doIt () ;

                        return ; // don’t update options after deleting the slide
                }

                doneWithKey = true ;
        }

        if ( doneWithKey ) {
                allegro::releaseKey( theKey );
                updateOptions ();
        } else
                this->menuOfScreenSizes->handleKey( theKey ) ;
}

void gui::CreateGraphicsAreaSizeMenu::updateOptions ()
{
        std::string currentSize = util::number2string( GamePreferences::getScreenWidth() )
                                                        + "×"
                                + util::number2string( GamePreferences::getScreenHeight() ) ;

        const std::vector< Label * > & everySize = this->menuOfScreenSizes->getEveryOption ();
        for ( unsigned int i = 0 ; i < everySize.size() ; ++ i ) {
                if ( everySize[ i ] == nilPointer ) continue ;
                Label & option = * everySize[ i ] ;

                if ( option.getText() == currentSize ) {
                        this->menuOfScreenSizes->setNthOptionAsActive( i );
                        option.getFontToChange().setColor( "cyan" );
                } else
                        option.getFontToChange().setColor( "white" );
        }

        this->menuOfScreenSizes->redraw ();
}

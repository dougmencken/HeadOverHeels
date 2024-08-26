
#include "CreateGraphicsAreaSizeMenu.hpp"

#include "GamePreferences.hpp"
#include "GuiManager.hpp"
#include "LanguageStrings.hpp"
#include "Label.hpp"
#include "Slide.hpp"

#include "sleep.hpp"


/* public */ /* static */
std::vector< std::pair< unsigned int, unsigned int > > gui::CreateGraphicsAreaSizeMenu::popularScreenSizes ;

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

        Slide & slideToPickScreenSize = * GuiManager::getInstance().findOrCreateSlideForAction( getNameOfAction() );

        if ( slideToPickScreenSize.isNewAndEmpty() ) {
                slideToPickScreenSize.setEscapeAction( this->actionOnEscape );

                slideToPickScreenSize.placeHeadAndHeels( /* icons */ false, /* copyrights */ false );

                slideToPickScreenSize.drawSpectrumColorBoxes( true );

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

                slideToPickScreenSize.addWidget( this->menuOfScreenSizes );
        }

        slideToPickScreenSize.setKeyHandler( this );

        GuiManager::getInstance().changeSlide( getNameOfAction(), true );
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

                        bool inFullScreen = gui::GuiManager::getInstance().isInFullScreen() ;
                        if ( inFullScreen )
                                gui::GuiManager::getInstance().toggleFullScreenVideo ();

                        GamePreferences::allegroWindowSizeToScreenSize () ;

                        if ( inFullScreen )
                                gui::GuiManager::getInstance().toggleFullScreenVideo ();
                }

                doneWithKey = true ;
        }

        if ( doneWithKey ) {
                allegro::releaseKey( theKey );
                this->menuOfScreenSizes->redraw ();
        } else
                this->menuOfScreenSizes->handleKey( theKey ) ;
}

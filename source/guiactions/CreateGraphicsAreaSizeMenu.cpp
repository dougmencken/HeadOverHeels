
#include "CreateGraphicsAreaSizeMenu.hpp"

#include "GamePreferences.hpp"
#include "GuiManager.hpp"
#include "LanguageStrings.hpp"
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

        this->customSize = new Label( "(custom)" );

        this->menuOfScreenSizes = new Menu( );
        this->menuOfScreenSizes->setVerticalOffset( 40 );

        this->menuOfScreenSizes->addOption( this->customSize );

        unsigned int howManyPopularScreenSizes = popularScreenSizes.size() ;
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

        gui::GuiManager::getInstance().changeSlide( getNameOfAction(), false );
}

void gui::CreateGraphicsAreaSizeMenu::handleKey ( const std::string & theKey )
{
        bool doneWithKey = false ;

        if ( theKey == "Enter" || theKey == "Space" )
        {
                std::string chosenSize = this->menuOfScreenSizes->getActiveOption()->getText() ;

                bool isCustomSize = chosenSize.find( " " ) != std::string::npos ;
                if ( isCustomSize ) {
                        pickCustomSize() ;
                        return ;
                }

                size_t whereX = chosenSize.find( "×" ) ;

                std::string chosenWidth = chosenSize.substr( 0, whereX ) ;
                std::string chosenHeight = chosenSize.substr( whereX + std::string( "×" ).length() );

                if ( CreateGraphicsAreaSizeMenu::applySize( chosenWidth, chosenHeight ) )
                {
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

void gui::CreateGraphicsAreaSizeMenu::pickCustomSize ()
{
        std::string currentWidth = util::number2string( GamePreferences::getScreenWidth() ) ;
        std::string currentHeight = util::number2string( GamePreferences::getScreenHeight() ) ;

        LanguageStrings & languageStrings = gui::GuiManager::getInstance().getOrMakeLanguageStrings() ;
        std::string customSizeText = languageStrings.getTranslatedTextByAlias( "custom-size" ).getText() ;

        std::string chosenWidth = currentWidth ;
        std::string chosenHeight = currentHeight ;

        bool pickingWidth = true ; // first the width then the height
        bool applyingCustomSize = false ; // and finally applying

        Timer customSizeBlinkTimer ;
        customSizeBlinkTimer.go () ;

        std::string key ;
        do {
                if ( customSizeBlinkTimer.getValue() > 1.5 )
                        customSizeBlinkTimer.go () ;

                if ( customSizeBlinkTimer.getValue() > .9 /* 0.9 to 1.5 */ ) {
                        if ( applyingCustomSize )
                                this->customSize->setText(
                                        customSizeText + " "
                                                + std::string( utf8StringLength( chosenWidth + "×" + chosenHeight ), ' ' ) );
                        else
                                this->customSize->setText(
                                        customSizeText + " "
                                                + ( pickingWidth ? std::string( chosenWidth.length(), ' ' ) : chosenWidth )
                                                + "×"
                                                + ( pickingWidth ? chosenHeight : std::string( chosenHeight.length(), ' ' ) ) );
                } else /* 0 to 0.9 */
                        this->customSize->setText( customSizeText + " " + chosenWidth + "×" + chosenHeight );

                if ( applyingCustomSize )
                        this->customSize->getFontToChange().setColor( "yellow" );
                else
                        this->customSize->getFontToChange().setColor(
                                ( currentWidth != chosenWidth || currentHeight != chosenHeight )
                                        ? "white" : "cyan" );

                this->menuOfScreenSizes->redraw ();

                key = allegro::nextKey() ;

                if ( key == "Enter" || key == "Space" ) {
                        if ( applyingCustomSize ) {
                                if ( CreateGraphicsAreaSizeMenu::applySize( chosenWidth, chosenHeight ) )
                                {
                                        this->doIt () ;
                                        break ;
                                }
                        }
                        else // not applying custom size
                        if ( pickingWidth ) pickingWidth = false ; // now picking the height
                        else
                        if ( chosenWidth != currentWidth || chosenHeight != currentHeight )
                                applyingCustomSize = true ;
                        else break ;
                }
                else
                if ( ! applyingCustomSize ) {
                        if ( key == "Up" || key == "q" ) {
                                if ( pickingWidth )
                                        chosenWidth = util::number2string( 2 + std::atoi( chosenWidth.c_str() ) );
                                else
                                        chosenHeight = util::number2string( 2 + std::atoi( chosenHeight.c_str() ) );
                        }
                        else if ( key == "Down" || key == "a" ) {
                                if ( pickingWidth )
                                        chosenWidth = util::number2string( std::atoi( chosenWidth.c_str() ) - 2 );
                                else
                                        chosenHeight = util::number2string( std::atoi( chosenHeight.c_str() ) - 2 );
                        }
                        else if ( key == "Left" || key == "o" || key == "Right" || key == "p" )
                                pickingWidth = ! pickingWidth ;
                }
        }
        while ( key != "Escape" ) ;

        allegro::releaseKey( "Escape" );

        updateOptions ();
}

/* static */
bool gui::CreateGraphicsAreaSizeMenu::applySize ( const std::string & chosenWidth, const std::string & chosenHeight )
{
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

                return true ;
        }

        return false ;
}

void gui::CreateGraphicsAreaSizeMenu::updateOptions ()
{
        std::string currentSize = util::number2string( GamePreferences::getScreenWidth() )
                                                        + "×"
                                + util::number2string( GamePreferences::getScreenHeight() ) ;

        const std::vector< Label * > & everySize = this->menuOfScreenSizes->getEveryOption ();
        bool currentSizeIsPopular = false ;
        for ( unsigned int i = 0 ; i < everySize.size() ; ++ i ) {
                if ( everySize[ i ] == nilPointer ) continue ;
                Label & option = * everySize[ i ] ;

                if ( option.getText() == currentSize ) {
                        this->menuOfScreenSizes->setNthOptionAsActive( i );
                        option.getFontToChange().setColor( "cyan" );
                        currentSizeIsPopular = true ;
                } else
                        option.getFontToChange().setColor( "white" );
        }

        LanguageStrings & languageStrings = gui::GuiManager::getInstance().getOrMakeLanguageStrings() ;
        this->customSize->setText( languageStrings.getTranslatedTextByAlias( "custom-size" ).getText() );
        this->customSize->getFontToChange().setColor( currentSizeIsPopular ? "white" : "cyan" ) ;
        if ( ! currentSizeIsPopular )
                this->customSize->setText( this->customSize->getText() + " " + currentSize );

        this->menuOfScreenSizes->redraw ();
}

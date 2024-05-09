
#include "CreateVideoMenu.hpp"

#include "CreateMenuOfGraphicsSets.hpp"
#include "GuiManager.hpp"
#include "GameManager.hpp"
#include "GamePreferences.hpp"
#include "LanguageStrings.hpp"
#include "Screen.hpp"
#include "MenuWithValues.hpp"
#include "Label.hpp"
#include "CreateOptionsMenu.hpp"

#include "sleep.hpp"

using gui::CreateVideoMenu ;
using gui::CreateMenuOfGraphicsSets ;


static std::multimap< unsigned int, unsigned int > sizesOfScreen ;

CreateVideoMenu::CreateVideoMenu( )
        : Action( )
        , listOfOptions ( nilPointer )
        , screenSize ( nilPointer )
        , fullScreen ( nilPointer )
        , drawShadows ( nilPointer )
        , drawSceneryDecor ( nilPointer )
        , drawRoomMiniatures ( nilPointer )
        , centerCameraOn ( nilPointer )
        , chooseGraphics ( nilPointer )
{
        if ( sizesOfScreen.size () == 0 )
        {
                // fill the list of screen sizes

                sizesOfScreen.insert( std::pair< unsigned int, unsigned int >( 640, 480 ) );
                sizesOfScreen.insert( std::pair< unsigned int, unsigned int >( 800, 600 ) );
                /* sizesOfScreen.insert( std::pair< unsigned int, unsigned int >( 1024, 576 ) ); */
                sizesOfScreen.insert( std::pair< unsigned int, unsigned int >( 1024, 600 ) );
                sizesOfScreen.insert( std::pair< unsigned int, unsigned int >( 1024, 768 ) );
                sizesOfScreen.insert( std::pair< unsigned int, unsigned int >( 1280, 720 ) );
                sizesOfScreen.insert( std::pair< unsigned int, unsigned int >( 1280, 1024 ) );
                sizesOfScreen.insert( std::pair< unsigned int, unsigned int >( 1366, 768 ) ); // my laptop
                sizesOfScreen.insert( std::pair< unsigned int, unsigned int >( 1600, 900 ) );
        }
}

void CreateVideoMenu::act ()
{
        Screen & screen = * GuiManager::getInstance().findOrCreateScreenForAction( *this );

        if ( screen.isNewAndEmpty() )
        {
                screen.placeHeadAndHeels( /* icons */ false, /* copyrights */ false );

                LanguageStrings* languageStrings = gui::GuiManager::getInstance().getLanguageStrings() ;

                LanguageText* textScreenSize = languageStrings->getTranslatedTextByAlias( "screen-size" );
                LanguageText* textFullscreen = languageStrings->getTranslatedTextByAlias( "full-screen" );
                LanguageText* textDrawShadows = languageStrings->getTranslatedTextByAlias( "draw-shadows" );
                LanguageText* textDrawSceneryDecor = languageStrings->getTranslatedTextByAlias( "draw-decor" );
                LanguageText* textDrawRoomMiniatures = languageStrings->getTranslatedTextByAlias( "show-miniatures" );
                LanguageText* textCenterCameraOn = languageStrings->getTranslatedTextByAlias( "center-camera-on" );

                std::string widtHxHeight = util::number2string( GamePreferences::getScreenWidth() )
                                                        + "×"
                                                + util::number2string( GamePreferences::getScreenHeight() );
                this->screenSize = new Label( textScreenSize->getText() + " " + widtHxHeight );
                this->screenSize->changeColor( "cyan" );

                this->fullScreen = new Label( textFullscreen->getText() );
                this->drawShadows = new Label( textDrawShadows->getText() );
                this->drawSceneryDecor = new Label( textDrawSceneryDecor->getText() );
                this->drawRoomMiniatures = new Label( textDrawRoomMiniatures->getText() );
                this->centerCameraOn = new Label( textCenterCameraOn->getText() );

                LanguageText* textChooseGraphics = languageStrings->getTranslatedTextByAlias( "graphic-set" );
                this->chooseGraphics = new Label( textChooseGraphics->getText(), Font::fontWithColor( "yellow" ) );
                this->chooseGraphics->setAction( new CreateMenuOfGraphicsSets( this ) );

                this->listOfOptions = new MenuWithValues( ' ', 1 );

                listOfOptions->addOption( screenSize );
                listOfOptions->addOption( fullScreen );
                listOfOptions->addOption( drawShadows );
                listOfOptions->addOption( drawSceneryDecor );
                listOfOptions->addOption( drawRoomMiniatures );
                listOfOptions->addOption( centerCameraOn );
                listOfOptions->addOption( chooseGraphics );

                updateLabels ();

                listOfOptions->setVerticalOffset( 33 );

                screen.addWidget( listOfOptions );
        }
        else
                updateLabels();

        screen.setEscapeAction( new CreateOptionsMenu() );

        if ( screen.getNextKeyHandler() == nilPointer )
                screen.setNextKeyHandler( listOfOptions );

        screen.drawSpectrumColorBoxes( true );

        gui::GuiManager::getInstance().changeScreen( screen, true );

        allegro::emptyKeyboardBuffer();

        while ( ! screen.getEscapeAction()->hasBegun() )
        {
                if ( allegro::areKeypushesWaiting() )
                {
                        // get the key pressed by the user
                        std::string theKey = allegro::nextKey() ;

                        if ( theKey == "Escape" )
                        {
                                allegro::emptyKeyboardBuffer();
                                screen.handleKey( theKey );
                                break;
                        }
                        else
                        {
                                bool doneWithKey = false;

                                if ( theKey == "Left" || theKey == "Right" || theKey == "o" || theKey == "p" )
                                {
                                        if ( listOfOptions->getActiveOption () == fullScreen )
                                        {
                                                gui::GuiManager::getInstance().toggleFullScreenVideo ();
                                                doneWithKey = true;
                                        }
                                        else if ( listOfOptions->getActiveOption () == drawShadows )
                                        {
                                                GameManager::getInstance().toggleCastShadows ();
                                                doneWithKey = true;
                                        }
                                        else if ( listOfOptions->getActiveOption () == drawSceneryDecor )
                                        {
                                                GameManager::getInstance().toggleSceneryDecor ();
                                                doneWithKey = true;
                                        }
                                        else if ( listOfOptions->getActiveOption () == drawRoomMiniatures )
                                        {
                                                GameManager::getInstance().toggleRoomMiniatures ();
                                                doneWithKey = true;
                                        }
                                        else if ( listOfOptions->getActiveOption () == centerCameraOn )
                                        {
                                                GameManager::getInstance().getIsomot().toggleCameraFollowsCharacter ();
                                                doneWithKey = true;
                                        }
                                }

                                if ( ! doneWithKey )
                                        screen.getNextKeyHandler()->handleKey( theKey );

                                allegro::emptyKeyboardBuffer();

                                // update options now
                                updateLabels();
                        }

                        // no te comas la CPU
                        // do not eat the CPU
                        somn::milliSleep( 25 );
                }
        }
}

void CreateVideoMenu::updateLabels ()
{
        LanguageStrings* languageStrings = gui::GuiManager::getInstance().getLanguageStrings() ;

        std::string yeah = languageStrings->getTranslatedTextByAlias( "yep" )-> getText ();
        std::string nope = languageStrings->getTranslatedTextByAlias( "nope" )->getText ();

        listOfOptions->setValueOf( drawRoomMiniatures, GameManager::getInstance().drawRoomMiniatures () ? yeah : nope );
        listOfOptions->setValueOf( drawSceneryDecor, GameManager::getInstance().drawSceneryDecor () ? yeah : nope );
        listOfOptions->setValueOf( drawShadows, GameManager::getInstance().getCastShadows () ? yeah : nope );
        listOfOptions->setValueOf( fullScreen, gui::GuiManager::getInstance().isAtFullScreen () ? yeah : nope );

        std::string room = languageStrings->getTranslatedTextByAlias( "room" )-> getText ();
        std::string character = languageStrings->getTranslatedTextByAlias( "character" )-> getText ();

        listOfOptions->setValueOf( centerCameraOn, GameManager::getInstance().getIsomot().doesCameraFollowCharacter () ? character : room );

        // chooseGraphics has no value but action

        listOfOptions->redraw ();
}

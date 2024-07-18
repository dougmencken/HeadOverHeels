
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


void gui::CreateVideoMenu::act ()
{
        Screen & slideWithVideoMenu = * GuiManager::getInstance().findOrCreateSlideForAction( getNameOfAction() );

        if ( slideWithVideoMenu.isNewAndEmpty() )
        {
                slideWithVideoMenu.placeHeadAndHeels( /* icons */ false, /* copyrights */ false );

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
                this->screenSize = new Label( textScreenSize->getText() + " " + widtHxHeight, new Font( "cyan" ) );

                this->fullScreen = new Label( textFullscreen->getText() );
                this->drawShadows = new Label( textDrawShadows->getText() );
                this->drawSceneryDecor = new Label( textDrawSceneryDecor->getText() );
                this->drawRoomMiniatures = new Label( textDrawRoomMiniatures->getText() );
                this->centerCameraOn = new Label( textCenterCameraOn->getText() );

                LanguageText* textChooseGraphics = languageStrings->getTranslatedTextByAlias( "graphic-set" );
                this->chooseGraphics = new Label( textChooseGraphics->getText(), new Font( "yellow" ) );
                this->chooseGraphics->setAction( new CreateMenuOfGraphicsSets( this ) );

                this->videoOptions = new MenuWithValues( ' ', 1 );

                videoOptions->addOption( screenSize );
                videoOptions->addOption( fullScreen );
                videoOptions->addOption( drawShadows );
                videoOptions->addOption( drawSceneryDecor );
                videoOptions->addOption( drawRoomMiniatures );
                videoOptions->addOption( centerCameraOn );
                videoOptions->addOption( chooseGraphics );

                videoOptions->setVerticalOffset( 33 );

                slideWithVideoMenu.addWidget( this->videoOptions );
        }

        updateOptions ();

        slideWithVideoMenu.setEscapeAction( new CreateOptionsMenu() );

        if ( slideWithVideoMenu.getKeyHandler() == nilPointer )
                slideWithVideoMenu.setKeyHandler( this->videoOptions );

        slideWithVideoMenu.drawSpectrumColorBoxes( true );

        gui::GuiManager::getInstance().changeSlide( getNameOfAction(), true );

        allegro::emptyKeyboardBuffer();

        while ( ! slideWithVideoMenu.getEscapeAction()->hasBegun() )
        {
                if ( allegro::areKeypushesWaiting() )
                {
                        // get the key typed by the user
                        std::string theKey = allegro::nextKey() ;

                        if ( theKey == "Escape" )
                        {
                                allegro::emptyKeyboardBuffer();
                                slideWithVideoMenu.handleKey( theKey );
                                break;
                        }
                        else
                        {
                                bool doneWithKey = false;

                                if ( theKey == "Left" || theKey == "Right" || theKey == "o" || theKey == "p" )
                                {
                                        if ( videoOptions->getActiveOption () == fullScreen )
                                        {
                                                gui::GuiManager::getInstance().toggleFullScreenVideo ();
                                                doneWithKey = true;
                                        }
                                        else if ( videoOptions->getActiveOption () == drawShadows )
                                        {
                                                GameManager::getInstance().toggleCastShadows ();
                                                doneWithKey = true;
                                        }
                                        else if ( videoOptions->getActiveOption () == drawSceneryDecor )
                                        {
                                                GameManager::getInstance().toggleSceneryDecor ();
                                                doneWithKey = true;
                                        }
                                        else if ( videoOptions->getActiveOption () == drawRoomMiniatures )
                                        {
                                                GameManager::getInstance().toggleRoomMiniatures ();
                                                doneWithKey = true;
                                        }
                                        else if ( videoOptions->getActiveOption () == centerCameraOn )
                                        {
                                                GameManager::getInstance().getIsomot().toggleCameraFollowsCharacter ();
                                                doneWithKey = true;
                                        }
                                }

                                if ( ! doneWithKey )
                                        slideWithVideoMenu.getKeyHandler()->handleKey( theKey );

                                allegro::emptyKeyboardBuffer();

                                updateOptions ();
                        }

                        // no te comas la CPU
                        // do not eat the CPU
                        somn::milliSleep( 25 );
                }
        }
}

void gui::CreateVideoMenu::updateOptions ()
{
        LanguageStrings* languageStrings = gui::GuiManager::getInstance().getLanguageStrings() ;

        std::string yeah = languageStrings->getTranslatedTextByAlias( "yep" )-> getText ();
        std::string nope = languageStrings->getTranslatedTextByAlias( "nope" )->getText ();

        videoOptions->setValueOf( drawRoomMiniatures, GameManager::getInstance().drawRoomMiniatures () ? yeah : nope );
        videoOptions->setValueOf( drawSceneryDecor, GameManager::getInstance().drawSceneryDecor () ? yeah : nope );
        videoOptions->setValueOf( drawShadows, GameManager::getInstance().getCastShadows () ? yeah : nope );
        videoOptions->setValueOf( fullScreen, gui::GuiManager::getInstance().isInFullScreen () ? yeah : nope );

        std::string room = languageStrings->getTranslatedTextByAlias( "room" )-> getText ();
        std::string character = languageStrings->getTranslatedTextByAlias( "character" )-> getText ();

        videoOptions->setValueOf( centerCameraOn, GameManager::getInstance().getIsomot().doesCameraFollowCharacter () ? character : room );

        // chooseGraphics has no value but action

        videoOptions->redraw ();
}

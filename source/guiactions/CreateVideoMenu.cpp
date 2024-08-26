
#include "CreateVideoMenu.hpp"

#include "CreateGraphicsAreaSizeMenu.hpp"
#include "CreateMenuOfGraphicsSets.hpp"
#include "GuiManager.hpp"
#include "GameManager.hpp"
#include "GamePreferences.hpp"
#include "LanguageStrings.hpp"
#include "Slide.hpp"
#include "MenuWithValues.hpp"
#include "Label.hpp"
#include "CreateOptionsMenu.hpp"

#include "sleep.hpp"


void gui::CreateVideoMenu::act ()
{
        Slide & slideWithVideoMenu = * GuiManager::getInstance().findOrCreateSlideForAction( getNameOfAction() );

        if ( slideWithVideoMenu.isNewAndEmpty() ) {
                slideWithVideoMenu.setEscapeAction( new CreateOptionsMenu() );

                slideWithVideoMenu.placeHeadAndHeels( /* icons */ false, /* copyrights */ false );

                slideWithVideoMenu.drawSpectrumColorBoxes( true );

                LanguageStrings & languageStrings = gui::GuiManager::getInstance().getOrMakeLanguageStrings() ;

                std::string widtHxHeight = util::number2string( GamePreferences::getScreenWidth() )
                                                        + "×"
                                                + util::number2string( GamePreferences::getScreenHeight() );
                this->screenSize = new Label( languageStrings.getTranslatedTextByAlias( "screen-size" ).getText() + " " + widtHxHeight );
                this->screenSize->getFontToChange().setColor( "cyan" );
                this->screenSize->setAction( new CreateGraphicsAreaSizeMenu( this ) );

                this->fullScreen = new Label( languageStrings.getTranslatedTextByAlias( "full-screen" ).getText() );
                this->drawShadows = new Label( languageStrings.getTranslatedTextByAlias( "draw-shadows" ).getText() );
                this->drawSceneryDecor = new Label( languageStrings.getTranslatedTextByAlias( "draw-decor" ).getText() );
                this->drawRoomMiniatures = new Label( languageStrings.getTranslatedTextByAlias( "show-miniatures" ).getText() );
                this->centerCameraOn = new Label( languageStrings.getTranslatedTextByAlias( "center-camera-on" ).getText() );

                const LanguageText & textChooseGraphics = languageStrings.getTranslatedTextByAlias( "graphic-set" );
                this->chooseGraphics = new Label( textChooseGraphics.getText(), new Font( "yellow" ) );
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

        slideWithVideoMenu.setKeyHandler( this );

        gui::GuiManager::getInstance().changeSlide( getNameOfAction(), true );
}

void gui::CreateVideoMenu::handleKey ( const std::string & theKey )
{
        bool doneWithKey = false ;

        if ( theKey == "Left" || theKey == "Right" || theKey == "o" || theKey == "p" )
        {
                if ( videoOptions->getActiveOption () == fullScreen )
                {
                        gui::GuiManager::getInstance().toggleFullScreenVideo ();
                        doneWithKey = true ;
                }
                else if ( videoOptions->getActiveOption () == drawShadows )
                {
                        GameManager::getInstance().toggleCastShadows ();
                        doneWithKey = true ;
                }
                else if ( videoOptions->getActiveOption () == drawSceneryDecor )
                {
                        GameManager::getInstance().toggleSceneryDecor ();
                        doneWithKey = true ;
                }
                else if ( videoOptions->getActiveOption () == drawRoomMiniatures )
                {
                        GameManager::getInstance().toggleRoomMiniatures ();
                        doneWithKey = true ;
                }
                else if ( videoOptions->getActiveOption () == centerCameraOn )
                {
                        GameManager::getInstance().getIsomot().toggleCameraFollowsCharacter ();
                        doneWithKey = true ;
                }
        }

        if ( doneWithKey ) {
                allegro::releaseKey( theKey );
                updateOptions ();
        } else
                this->videoOptions->handleKey( theKey ) ;
}

void gui::CreateVideoMenu::updateOptions ()
{
        LanguageStrings & languageStrings = gui::GuiManager::getInstance().getOrMakeLanguageStrings() ;

        std::string yeah = languageStrings.getTranslatedTextByAlias( "yep" ).getText() ;
        std::string nope = languageStrings.getTranslatedTextByAlias( "nope" ).getText() ;

        videoOptions->setValueOf( drawRoomMiniatures, GameManager::getInstance().drawRoomMiniatures () ? yeah : nope );
        videoOptions->setValueOf( drawSceneryDecor, GameManager::getInstance().drawSceneryDecor () ? yeah : nope );
        videoOptions->setValueOf( drawShadows, GameManager::getInstance().getCastShadows () ? yeah : nope );
        videoOptions->setValueOf( fullScreen, gui::GuiManager::getInstance().isInFullScreen () ? yeah : nope );

        std::string room = languageStrings.getTranslatedTextByAlias( "room" ).getText ();
        std::string character = languageStrings.getTranslatedTextByAlias( "character" ).getText ();

        videoOptions->setValueOf( centerCameraOn, GameManager::getInstance().getIsomot().doesCameraFollowCharacter () ? character : room );

        // chooseGraphics has no value but action

        videoOptions->redraw ();
}

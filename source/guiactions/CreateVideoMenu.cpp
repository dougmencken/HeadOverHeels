
#include "CreateVideoMenu.hpp"

#include "CreateGraphicsAreaSizeMenu.hpp"
#include "CreateMenuOfGraphicsSets.hpp"
#include "GuiManager.hpp"
#include "GameManager.hpp"
#include "GamePreferences.hpp"
#include "LanguageStrings.hpp"
#include "SlideWithHeadAndHeels.hpp"
#include "Label.hpp"
#include "CreateOptionsMenu.hpp"

#include "sleep.hpp"


void gui::CreateVideoMenu::act ()
{
        SlideWithHeadAndHeels & slideWithVideoMenu = GuiManager::getInstance().findOrCreateSlideWithHeadAndHeelsForAction( getNameOfAction() );

        if ( slideWithVideoMenu.isNewAndEmpty() ) {
                slideWithVideoMenu.setEscapeAction( new CreateOptionsMenu() );

                slideWithVideoMenu.placeHeadAndHeels( /* icons */ false, /* copyrights */ false );

                this->videoOptions.deleteAllOptions() ;

                slideWithVideoMenu.drawSpectrumColorBoxes( true );

                this->screenSize = this->videoOptions.addOptionByLanguageTextAlias( "screen-size" );
                this->screenSize->getFontToChange().setColor( "cyan" );
                std::string widtHxHeight = util::number2string( GamePreferences::getScreenWidth() )
                                                        + "×"
                                                + util::number2string( GamePreferences::getScreenHeight() );
                this->screenSize->setText( this->screenSize->getText() + " " + widtHxHeight );
                this->screenSize->setAction( new CreateGraphicsAreaSizeMenu( this ) );

                this->fullScreen = this->videoOptions.addOptionByLanguageTextAlias( "full-screen" );
                this->drawShadows = this->videoOptions.addOptionByLanguageTextAlias( "draw-shadows" );
                this->drawSceneryDecor = this->videoOptions.addOptionByLanguageTextAlias( "draw-decor" );
                this->drawRoomMiniatures = this->videoOptions.addOptionByLanguageTextAlias( "show-miniatures" );
                this->centerCameraOn = this->videoOptions.addOptionByLanguageTextAlias( "center-camera-on" );

                this->chooseGraphics = this->videoOptions.addOptionByLanguageTextAlias( "graphic-set" );
                this->chooseGraphics->getFontToChange().setColor( "yellow" );
                this->chooseGraphics->setAction( new CreateMenuOfGraphicsSets( this ) );

                slideWithVideoMenu.addWidget( & this->videoOptions );
        }

        updateOptions ();

        slideWithVideoMenu.setKeyHandler( this );

        gui::GuiManager::getInstance().changeToSlideFor( getNameOfAction(), true );
}

void gui::CreateVideoMenu::handleKey ( const std::string & theKey )
{
        bool doneWithKey = false ;

        if ( theKey == "Left" || theKey == "Right" || theKey == "o" || theKey == "p" )
        {
                Label * activeOption = videoOptions.getActiveOption() ;

                if ( activeOption == fullScreen )
                {
                        gui::GuiManager::getInstance().toggleFullScreenVideo ();
                        doneWithKey = true ;
                }
                else if ( activeOption == drawShadows )
                {
                        GameManager::getInstance().toggleCastShadows ();
                        doneWithKey = true ;
                }
                else if ( activeOption == drawSceneryDecor )
                {
                        GameManager::getInstance().toggleSceneryDecor ();
                        doneWithKey = true ;
                }
                else if ( activeOption == drawRoomMiniatures )
                {
                        GameManager::getInstance().toggleRoomMiniatures ();
                        doneWithKey = true ;
                }
                else if ( activeOption == centerCameraOn )
                {
                        GameManager::getInstance().getIsomot().toggleCameraFollowsCharacter ();
                        doneWithKey = true ;
                }
        }

        if ( doneWithKey ) {
                allegro::releaseKey( theKey );
                updateOptions ();
        } else
                this->videoOptions.handleKey( theKey ) ;
}

void gui::CreateVideoMenu::updateOptions ()
{
        LanguageStrings & languageStrings = gui::GuiManager::getInstance().getOrMakeLanguageStrings() ;

        std::string yeah = languageStrings.getTranslatedTextByAlias( "yep" ).getText() ;
        std::string nope = languageStrings.getTranslatedTextByAlias( "nope" ).getText() ;

        videoOptions.setValueOf( drawRoomMiniatures->getText(), GameManager::getInstance().drawRoomMiniatures () ? yeah : nope );
        videoOptions.setValueOf( drawSceneryDecor->getText(), GameManager::getInstance().drawSceneryDecor () ? yeah : nope );
        videoOptions.setValueOf( drawShadows->getText(), GameManager::getInstance().getCastShadows () ? yeah : nope );
        videoOptions.setValueOf( fullScreen->getText(), gui::GuiManager::getInstance().isInFullScreen () ? yeah : nope );

        std::string room = languageStrings.getTranslatedTextByAlias( "room" ).getText ();
        std::string character = languageStrings.getTranslatedTextByAlias( "character" ).getText ();

        videoOptions.setValueOf( centerCameraOn->getText(), GameManager::getInstance().getIsomot().doesCameraFollowCharacter () ? character : room );

        // chooseGraphics has no value but action

        videoOptions.redraw ();
}

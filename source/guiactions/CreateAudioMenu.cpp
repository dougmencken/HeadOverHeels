
#include "CreateAudioMenu.hpp"

#include "GuiManager.hpp"
#include "GameManager.hpp"
#include "LanguageStrings.hpp"
#include "SoundManager.hpp"
#include "Slide.hpp"
#include "MenuWithValues.hpp"
#include "Label.hpp"
#include "CreateOptionsMenu.hpp"

#include "sleep.hpp"


void gui::CreateAudioMenu::act ()
{
        LanguageStrings & languageStrings = GuiManager::getInstance().getOrMakeLanguageStrings() ;

        Slide & slideWithAudioMenu = GuiManager::getInstance().findOrCreateSlideForAction( getNameOfAction() );

        if ( slideWithAudioMenu.isNewAndEmpty() ) {
                slideWithAudioMenu.setEscapeAction( new CreateOptionsMenu() );

                slideWithAudioMenu.placeHeadAndHeels( /* icons */ false, /* copyrights */ false );

                // efectos sonoros
                this->labelEffects = new Label( languageStrings.getTranslatedTextByAlias( "soundfx" ).getText() );

                // música
                this->labelMusic = new Label( languageStrings.getTranslatedTextByAlias( "music" ).getText() );

                // play the melody of scenery on entry to a room or don’t
                this->playRoomTunes = new Label( languageStrings.getTranslatedTextByAlias( "play-room-melodies" ).getText() );

                // create the menu
                this->audioOptions = new MenuWithValues( );

                audioOptions->addOption( this->labelEffects );
                audioOptions->addOption( this->labelMusic );
                audioOptions->addOption( this->playRoomTunes );

                audioOptions->setVerticalOffset( 33 );
                slideWithAudioMenu.addWidget( this->audioOptions );
        }

        updateOptions ();

        slideWithAudioMenu.setKeyHandler( this );

        GuiManager::getInstance().changeSlide( getNameOfAction(), true );
}

void gui::CreateAudioMenu::handleKey ( const std::string & theKey )
{
        bool doneWithKey = false ;

        if ( audioOptions->getActiveOption () == labelMusic || audioOptions->getActiveOption () == labelEffects )
        {
                int value = ( audioOptions->getActiveOption () == labelMusic )
                                        ? SoundManager::getInstance().getVolumeOfMusic()
                                        : SoundManager::getInstance().getVolumeOfEffects() ;
                int previousValue = value ;

                if ( theKey == "Left" || theKey == "o" ) {
                        value = ( value > 0 ) ? value - 1 : 0 ; // decrease volume
                        doneWithKey = true ;
                }
                else if ( theKey == "Right" || theKey == "p" ) {
                        value = ( value < 99 ) ? value + 1 : 99 ; // increase volume
                        doneWithKey = true ;
                }

                if ( value != previousValue ) {
                        if ( audioOptions->getActiveOption () == labelMusic )
                        {
                                SoundManager::getInstance().setVolumeOfMusic( value );
                        }
                        else if ( audioOptions->getActiveOption () == labelEffects )
                        {
                                SoundManager::getInstance().setVolumeOfEffects( value );
                                SoundManager::getInstance().stopEverySound ();
                                SoundManager::getInstance().play ( "menus", "effect", /* loop */ false );
                        }
                }
        }
        else if ( audioOptions->getActiveOption () == playRoomTunes )
        {
                if ( theKey == "Left" || theKey == "Right" || theKey == "o" || theKey == "p" )
                {
                        GameManager::getInstance().togglePlayMelodyOfScenery() ;
                        doneWithKey = true ;
                }
        }

        if ( doneWithKey ) {
                allegro::releaseKey( theKey );
                updateOptions ();
        } else
                this->audioOptions->handleKey( theKey ) ;
}

void gui::CreateAudioMenu::updateOptions ()
{
        LanguageStrings & languageStrings = GuiManager::getInstance().getOrMakeLanguageStrings() ;

        std::string yeah = languageStrings.getTranslatedTextByAlias( "yep" ).getText ();
        std::string nope = languageStrings.getTranslatedTextByAlias( "nope" ).getText ();

        audioOptions->setValueOf( labelEffects, util::number2string( SoundManager::getInstance().getVolumeOfEffects() ) );
        audioOptions->setValueOf( labelMusic, util::number2string( SoundManager::getInstance().getVolumeOfMusic() ) );
        audioOptions->setValueOf( playRoomTunes, GameManager::getInstance().playMelodyOfScenery () ? yeah : nope );

        audioOptions->redraw ();
}

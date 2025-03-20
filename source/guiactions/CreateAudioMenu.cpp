
#include "CreateAudioMenu.hpp"

#include "GuiManager.hpp"
#include "GameManager.hpp"
#include "LanguageStrings.hpp"
#include "SoundManager.hpp"
#include "SlideWithHeadAndHeels.hpp"
#include "Label.hpp"
#include "CreateOptionsMenu.hpp"

#include "sleep.hpp"


void gui::CreateAudioMenu::act ()
{
        SlideWithHeadAndHeels & slideWithAudioMenu = GuiManager::getInstance().findOrCreateSlideWithHeadAndHeelsForAction( getNameOfAction() );

        if ( slideWithAudioMenu.isNewAndEmpty() ) {
                slideWithAudioMenu.setEscapeAction( new CreateOptionsMenu() );

                slideWithAudioMenu.placeHeadAndHeels( /* icons */ false, /* copyrights */ false );

                this->audioOptions.deleteAllOptions() ;

                // efectos sonoros
                this->labelEffects = audioOptions.addOptionByLanguageTextAlias( "soundfx" );

                // música
                this->labelMusic = audioOptions.addOptionByLanguageTextAlias( "music" ) ;

                // play the melody of scenery on entry to a room or don’t
                this->playRoomTunes = audioOptions.addOptionByLanguageTextAlias( "play-room-melodies" );

                slideWithAudioMenu.addWidget( & this->audioOptions );
        }

        updateOptions ();

        slideWithAudioMenu.setKeyHandler( this );

        GuiManager::getInstance().changeToSlideFor( getNameOfAction(), true );
}

void gui::CreateAudioMenu::handleKey ( const std::string & theKey )
{
        bool doneWithKey = false ;

        Label * activeOption = audioOptions.getActiveOption() ;

        if ( activeOption == labelMusic || activeOption == labelEffects )
        {
                int value = ( activeOption == labelMusic )
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
                        if ( activeOption == labelMusic ) {
                                SoundManager::getInstance().setVolumeOfMusic( value );
                        }
                        else if ( activeOption == labelEffects ) {
                                SoundManager::getInstance().setVolumeOfEffects( value );
                                SoundManager::getInstance().stopEverySound ();
                                SoundManager::getInstance().play ( "menus", "effect", /* loop */ false );
                        }
                }
        }
        else if ( activeOption == playRoomTunes )
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
                this->audioOptions.handleKey( theKey ) ;
}

void gui::CreateAudioMenu::updateOptions ()
{
        LanguageStrings & languageStrings = GuiManager::getInstance().getOrMakeLanguageStrings() ;

        std::string yeah = languageStrings.getTranslatedTextByAlias( "yep" ).getText ();
        std::string nope = languageStrings.getTranslatedTextByAlias( "nope" ).getText ();

        audioOptions.setValueOf( labelEffects->getText(), util::number2string( SoundManager::getInstance().getVolumeOfEffects() ) );
        audioOptions.setValueOf( labelMusic->getText(), util::number2string( SoundManager::getInstance().getVolumeOfMusic() ) );
        audioOptions.setValueOf( playRoomTunes->getText(), GameManager::getInstance().playMelodyOfScenery () ? yeah : nope );

        audioOptions.redraw ();
}
